#include "qforeach.h"
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <vector>
#include <QString>
#include <QIcon>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QDebug>
#include <unordered_set>
#include <QRegularExpression>
#include <QThread>


#include <QCoreApplication>
#include <QDir>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QDebug>

struct MemoryStats
{
    DWORDLONG totalPhysical;
    DWORDLONG availablePhysical;
    DWORDLONG usedPhysical;
    DWORDLONG processPrivate;
    DWORDLONG pageFileUsage;
};



class MemoryInfo {
public:
    int PID;
    QString PROCESS_NAME;
    QIcon ICON;
    int RAM_CONSUMPTION;
    int PROCESS_COUNT;

    MemoryInfo(int pid, const QString& name, const QIcon& icon, int ram)
        : PID(pid), PROCESS_NAME(name), ICON(icon),
        RAM_CONSUMPTION(ram), PROCESS_COUNT(1) {}
};



class MemoryAnalyser {
public:

    std::vector<DWORD> systemPids;
    std::chrono::steady_clock::time_point lastCleanTime;



    int CLEAN_THRESHOLD_MB = 100;
    int SYSTEM_CLEANING_PERCENT = 80;
    int MONITOR_INTERVAL_MS = 5000;
    int FORCE_CLEAN_LIMIT = 90;

    void ChangeCleanLimit(int value)
    {
        if(value<=100)
        {
             this->SYSTEM_CLEANING_PERCENT=value;
            if(value<95)
             {
                 this->FORCE_CLEAN_LIMIT=value+5;
            }
            else
            {
                this->FORCE_CLEAN_LIMIT=95;
            }
        }
        else
        {
            this->SYSTEM_CLEANING_PERCENT=90;
            this->FORCE_CLEAN_LIMIT=95;
        }

    }

    const std::vector<std::wstring> PROTECTED_PROCESSES = {
        L"System", L"System Idle Process", L"Registry",
        L"smss.exe", L"csrss.exe", L"wininit.exe",
        L"services.exe", L"lsass.exe", L"svchost.exe",
        L"taskhostw.exe", L"dwm.exe", L"explorer.exe",
        L"ctfmon.exe", L"SecurityHealthService.exe"
    };



    MemoryAnalyser() {
        CLEAN_FLAGS = PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_QUOTA | PROCESS_VM_READ;
        READ_FLAGS = PROCESS_VM_READ;
    }

    std::vector<MemoryInfo> memories;


    std::vector<MemoryInfo> SortMemoryList(std::vector<MemoryInfo> memories) {
        std::unordered_map<QString, MemoryInfo> groupedProcesses;

        for (const auto& process : memories) {
            // Нормализация имени процесса: удаление версий, цифр и модификаторов
            QString normalizedName = QString(process.PROCESS_NAME)
                                         .replace(QRegularExpression("(\\d+|\\.\\S*|\\s+\\*\\d+)$"), "") // Удаляем версии и модификаторы
                                         .trimmed()
                                         .toLower();


            auto it = groupedProcesses.find(normalizedName);
            if (it != groupedProcesses.end()) {
                // Суммируем память и обновляем количество экземпляров
                it->second.RAM_CONSUMPTION += process.RAM_CONSUMPTION;
                it->second.PROCESS_COUNT++;  // Добавляем счетчик процессов
            } else {
                MemoryInfo newProcess = process;
                newProcess.PROCESS_COUNT = 1;
                newProcess.PROCESS_NAME = normalizedName;  // Сохраняем нормализованное имя
                groupedProcesses.emplace(normalizedName, newProcess);
            }
        }

        std::vector<MemoryInfo> sortedProcesses;
        for (auto& entry : groupedProcesses) {
            // Восстанавливаем оригинальное имя для отображения
            entry.second.PROCESS_NAME = entry.second.PROCESS_NAME
                                            .section(".", 0, 0)  // Убираем расширение файла
                                            .toUpper();          // Первая буква заглавная

            sortedProcesses.push_back(entry.second);
        }

        std::sort(sortedProcesses.begin(), sortedProcesses.end(),
                  [](const MemoryInfo& a, const MemoryInfo& b) {
                      return a.RAM_CONSUMPTION > b.RAM_CONSUMPTION;
                  });

        return sortedProcesses;
    }





    DWORDLONG performDeepClean() {

        DWORDLONG before = getMemoryInfo().usedPhysical;

        cleanProcessesMemory();



        DWORDLONG after = getMemoryInfo().usedPhysical;
        DWORDLONG freed = before - after;
        memories = GetProcessList();



        QString logEntry;
        logEntry += "🕒 " + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "\n";
        logEntry += QString("🔹 Memory Before: %1 MB\n").arg(before / (1024 * 1024));
        logEntry += QString("🔹 Memory After:  %1 MB\n").arg(after / (1024 * 1024));
        logEntry += QString("✅ Freed:         %1 MB\n").arg(freed / (1024 * 1024));

        // Топ 10 процессов
        std::vector<MemoryInfo> top = memories;
        std::sort(top.begin(), top.end(), [](const MemoryInfo& a, const MemoryInfo& b) {
            return a.RAM_CONSUMPTION > b.RAM_CONSUMPTION;
        });

        logEntry += "--- Top Processes ---\n";
        int count = 0;
        int restMemory = 0;

        for (const MemoryInfo& info : top) {
            if (count < 10) {
                logEntry += QString("• %1 [%2 MB] (PID: %3)\n")
                                .arg(info.PROCESS_NAME)
                                .arg(info.RAM_CONSUMPTION)
                                .arg(info.PID);
            } else {
                restMemory += info.RAM_CONSUMPTION;
            }
            count++;
        }

        if (top.size() > 10) {
            logEntry += QString("• + %1 other processes, total: %2 MB\n")
                            .arg(top.size() - 10)
                            .arg(restMemory);
        }

        logEntry += "============================\n";

        writeToLog(logEntry);
        return freed;

    }




    void cleanProcessesMemory() {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32W pe{ sizeof(PROCESSENTRY32W) };

        if (Process32FirstW(snapshot, &pe)) {
            do {
                if (std::find(systemPids.begin(), systemPids.end(), pe.th32ProcessID) != systemPids.end())
                    continue;

                HANDLE hProcess = OpenProcess(CLEAN_FLAGS, FALSE, pe.th32ProcessID);
                if (!hProcess) continue;

                EmptyWorkingSet(hProcess);
                SetProcessWorkingSetSizeEx(hProcess, -1, -1, 0x01);
                SetProcessWorkingSetSizeEx(hProcess, -1, -1, 0x02);
                CloseHandle(hProcess);
            } while (Process32NextW(snapshot, &pe));
        }
        CloseHandle(snapshot);
    }



    MemoryStats getMemoryInfo() {
        MEMORYSTATUSEX memStatus{ sizeof(MEMORYSTATUSEX) };
        GlobalMemoryStatusEx(&memStatus);

        MemoryStats stats{};
        stats.totalPhysical = memStatus.ullTotalPhys;
        stats.availablePhysical = memStatus.ullAvailPhys;
        stats.usedPhysical = memStatus.ullTotalPhys - memStatus.ullAvailPhys;
        stats.pageFileUsage = memStatus.ullTotalPageFile - memStatus.ullAvailPageFile;
        stats.processPrivate = getTotalProcessMemory();
        return stats;
    }


    DWORDLONG getTotalProcessMemory() {
        DWORDLONG total = 0;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32W pe{ sizeof(PROCESSENTRY32W) };

        if (Process32FirstW(snapshot, &pe)) {
            do {
                if (std::find(systemPids.begin(), systemPids.end(), pe.th32ProcessID) != systemPids.end())
                    continue;

                HANDLE hProcess = OpenProcess(CLEAN_FLAGS, FALSE, pe.th32ProcessID);
                if (!hProcess) continue;

                PROCESS_MEMORY_COUNTERS_EX pmc{};
                if (GetProcessMemoryInfo(hProcess, reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc))) {
                    total += pmc.PrivateUsage;
                }
                CloseHandle(hProcess);
            } while (Process32NextW(snapshot, &pe));
        }
        CloseHandle(snapshot);
        return total;
    }


    std::vector<MemoryInfo> GetProcessList() {
        std::vector<MemoryInfo> processList;
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (snap != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W entry = {};
            entry.dwSize = sizeof(entry);

            if (Process32FirstW(snap, &entry)) {
                do {
                    // Извлекаем чистое имя процесса из пути
                    QString fullName = QString::fromWCharArray(entry.szExeFile);
                    QString processName = QFileInfo(fullName).fileName();

                    if (!processName.isEmpty()) {
                        QString iconPath = GetProcessIconPath(entry.th32ProcessID);
                        int memory = GetProcessMemoryConsumption(entry.th32ProcessID);

                        processList.emplace_back(
                            entry.th32ProcessID,
                            processName,
                            GetProcessIcon(iconPath),
                            memory
                            );
                    }
                } while (Process32NextW(snap, &entry));
            }
            CloseHandle(snap);
        }
        return SortMemoryList(processList);
    }


    QString GetProcessFilePath(DWORD processID) {
        WCHAR exePath[MAX_PATH] = {0};
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID);

        if (hProcess) {
            if (GetModuleFileNameExW(hProcess, NULL, exePath, MAX_PATH)) {
                CloseHandle(hProcess);
                return QString::fromWCharArray(exePath);
            }
            CloseHandle(hProcess);
        }
        return "";
    }

    QIcon GetProcessIcon(QString filePath) {
        QFileIconProvider provider;
        QFileInfo fileInfo(filePath);

        if (!fileInfo.exists()) {
            return QIcon(":/icons/default.png");
        }

        return provider.icon(fileInfo);
    }


    bool needCleaning(const MemoryStats& stats) {
        auto currentTime = std::chrono::steady_clock::now();
        double usedPercent = (stats.usedPhysical * 100.0) / stats.totalPhysical;


        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        qDebug() << "Checking if cleaning is needed at:" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        qDebug() << "Used memory percent:" << usedPercent << "% (Thresholds - Force:"
                 << FORCE_CLEAN_LIMIT << "%, System:" << SYSTEM_CLEANING_PERCENT << "%)";

        if (usedPercent >= FORCE_CLEAN_LIMIT) {
            qDebug() << "Memory usage exceeded FORCE_CLEAN_LIMIT! Cleaning required.";
            return true;
        }

        auto timeSinceLastClean = std::chrono::duration_cast<std::chrono::minutes>(currentTime - lastCleanTime);
        qDebug() << "Time since last cleaning:" << timeSinceLastClean.count() << "minutes";

        if (usedPercent >= SYSTEM_CLEANING_PERCENT && timeSinceLastClean > std::chrono::minutes(5)) {
            qDebug() << "Memory usage exceeded SYSTEM_CLEANING_PERCENT and last cleaning was more than 5 minutes ago. Cleaning required.";
            return true;
        }

        qDebug() << "No cleaning needed.";
        return false;
    }


    QString GetProcessIconPath(DWORD processID) {
        QString iconPath;
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID);

        if (hProcess) {
            WCHAR exePath[MAX_PATH] = {0};
            if (GetModuleFileNameExW(hProcess, NULL, exePath, MAX_PATH)) {
                QFileInfo fileInfo(QString::fromWCharArray(exePath));
                if (fileInfo.exists()) {
                    iconPath = fileInfo.absoluteFilePath();
                }
            }
            CloseHandle(hProcess);
        }

        if (iconPath.isEmpty()) {
            return "C:/Windows/System32/imageres.dll";
        }

        return iconPath;
    }

    int GetProcessMemoryConsumption(DWORD PID) {


        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);

        if (!hProcess) {

            return 0;
        }

        PROCESS_MEMORY_COUNTERS_EX pmc{};
        if (GetProcessMemoryInfo(hProcess, reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc))) {
            CloseHandle(hProcess);

            return pmc.PrivateUsage / 1024 / 1024;
        }



        CloseHandle(hProcess);
        return 0;
    }

    void writeToLog(const QString& text) {
        QString logsDirPath = QCoreApplication::applicationDirPath() + "/Cleaning Logs";


        QDir dir;
        if (!dir.exists(logsDirPath)) {
            dir.mkpath(logsDirPath);
        }


        QString fileName = "log_" + QDate::currentDate().toString("yyyy-MM-dd") + ".txt";
        QString fullPath = logsDirPath + "/" + fileName;

        QFile logFile(fullPath);
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&logFile);
            out << text << "\n";
            logFile.close();
        } else {
            qDebug() << "Failed to write to log file:" << fullPath;
        }
    }




private:
    DWORD CLEAN_FLAGS;
    DWORD READ_FLAGS;
};
