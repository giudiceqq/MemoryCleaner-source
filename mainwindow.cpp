#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ApplicationConfig.cpp"
#include <QListWidget>
#include <QListWidgetItem>
#include <QIcon>
#include <MemoryAnalyser.cpp>
#include <QMessageBox>
#include <QTimer>
#include <QCloseEvent>
#include <QDebug>
#include <QSettings>

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QStyle>
#include <QFileInfo>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <QCoreApplication>
#include <QDir>

MemoryAnalyser analyzer;
MemoryStats memory_stats;


void MainWindow::populateProcessList() {
    MemoryAnalyser* analyzer = new MemoryAnalyser();
    ui->processList->clear();

    for (const auto& process : analyzer->GetProcessList()) {

        QString displayName;

        displayName+= process.PROCESS_NAME;

        displayName += QString(" [%1 MB]").arg(process.RAM_CONSUMPTION);


        QListWidgetItem* item = new QListWidgetItem(process.ICON, displayName);
        item->setData(Qt::UserRole, process.RAM_CONSUMPTION);
        ui->processList->addItem(item);

    }


    memory_stats = analyzer->getMemoryInfo();


    ui->totalMemoryConsumption->setText("[ " + QString::number(memory_stats.usedPhysical/1024/1024) +
                                        "/" + QString::number(memory_stats.totalPhysical/1024/1024) + " MB ]");

    delete analyzer;
}


void MainWindow::checkAndCreateLogFile() {

    QString exeDir = QCoreApplication::applicationDirPath();
    QString logFilePath = exeDir + "/log.txt";

    QDir dir(exeDir);
    if (!dir.exists("log.txt")) {
        QFile logFile(logFilePath);
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            logFile.write("Log file created.\n");
            logFile.close();
            qDebug() << "Log file was missing. Created new log.txt";
        } else {
            qDebug() << "Failed to create log.txt!";
        }
    } else {
        qDebug() << "Log file already exists.";
    }
}








MainWindow::~MainWindow()
{
    delete ui;
}




QSystemTrayIcon *trayIcon;
QMenu *trayMenu;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
        this->setWindowTitle("Memory Cleaner");

    populateProcessList();


    ApplicationConfig config;
    ui->CleanVersion->setText(QString::fromStdString(config.APP_NAME) +
                              " v" + QString::fromStdString(config.APP_VERSION));

    connect(ui->AutoStartCheckBox, &QCheckBox::toggled, this, &MainWindow::toggleAutoStart);
    connect(ui->MemoryThreshold, &QCheckBox::toggled, this, &MainWindow::toggleThresholdInput);

    ui->MemoryThreshold->setToolTip("Enable this option to set a memory threshold.");
    ui->AutoStartCheckBox->setToolTip("Enable this option to start the application automatically when Windows boots.");
    ui->WorkInBackGroundCheckBox->setToolTip("Enable this to keep the app running in the background after closing.");


    connect(ui->thresholdInput, &QLineEdit::editingFinished, this, &MainWindow::validateThresholdInput);


    if(ui->MemoryThreshold->checkState() == false)
    {
        ui->thresholdInput->setVisible(false);
        ui->labelPercent->setVisible(false);
    }



    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::populateProcessList);
    timer->start(1000);


    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    trayIcon->setToolTip("Memory Cleaner - Running in Background");


    trayMenu = new QMenu(this);
    QAction *restoreAction = new QAction("Restore", this);
    QAction *exitAction = new QAction("Exit", this);

    connect(restoreAction, &QAction::triggered, this, &MainWindow::show);
    connect(exitAction, &QAction::triggered, this, &MainWindow::exitApp);

    trayMenu->addAction(restoreAction);
    trayMenu->addAction(exitAction);
    trayIcon->setContextMenu(trayMenu);

    trayIcon->show();

    startMonitoring();
}


void MainWindow::startMonitoring() {
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::run);
    timer->start(analyzer.MONITOR_INTERVAL_MS);
    qDebug() << "Memory monitoring started with interval:" << analyzer.MONITOR_INTERVAL_MS << "ms";
}

QString exePath;

void MainWindow::run() {
    qDebug() << "Checking memory status at:" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    if (ui->MemoryThreshold->isChecked()) {
        qDebug() << "MemoryThreshold is enabled. Analyzing memory...";

        auto memoryInfo = analyzer.getMemoryInfo();
        qDebug() << "Checking if cleaning is needed..." << analyzer.SYSTEM_CLEANING_PERCENT;

        if (analyzer.needCleaning(memoryInfo)) {
            qDebug() << "Cleaning required! Performing deep clean...";
            analyzer.performDeepClean();
            qDebug() << "Deep clean completed at:" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        } else {
            qDebug() << "No cleaning needed.";
        }
    } else {
        qDebug() << "MemoryThreshold is disabled. Skipping memory check.";
    }
}





void MainWindow::validateThresholdInput() {
    bool ok;
    int threshold = ui->thresholdInput->text().toInt(&ok);

    if (ok && threshold >= 10 && threshold <= 100) {
        qDebug() << "✅ Memory threshold set to: " << threshold << "%";
        analyzer.ChangeCleanLimit(threshold);

         qDebug() << "New value is " << analyzer.SYSTEM_CLEANING_PERCENT << "%";
    } else {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid percentage (10-100).");
        ui->thresholdInput->setText("");
    }
}

void MainWindow::exitApp()
{
    qDebug() << "Exiting application...";
    QApplication::quit();
}



void MainWindow::toggleThresholdInput(bool checked) {
    ui->thresholdInput->setVisible(checked);
    ui->labelPercent->setVisible(checked);
}

void MainWindow::on_CleanVersion_linkActivated(const QString &link)
{

}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (ui->WorkInBackGroundCheckBox->isChecked()) {
        event->ignore();
        this->hide();
    } else {
        event->accept();
    }
}


bool isAutoStartEnabled() {
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                       QSettings::NativeFormat);
    return settings.contains("MemoryCleaner");
}

void checkStartupStatus() {
    if (isAutoStartEnabled()) {
        qDebug() << "Auto-start is ENABLED";
    } else {
        qDebug() << "Auto-start is DISABLED";
    }
}

void MainWindow::setAutoStart(bool enable) {
    QString appName = "MemoryCleaner";
    QString exePath = QCoreApplication::applicationFilePath();

    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                       QSettings::NativeFormat);

    if (enable) {
        settings.setValue(appName, exePath);
        qDebug() << "Auto-start enabled";
    } else {
        settings.remove(appName);
        qDebug() << "Auto-start disabled";
    }
}




void MainWindow::toggleAutoStart(bool checked) {
    setAutoStart(checked);
}





void MainWindow::on_cleanButton_clicked()
{
    ui->progressCleanBar->setValue(0);



    DWORDLONG cleaned = analyzer.performDeepClean();


    qDebug() << "Memory cleaned: " << cleaned / 1024 / 1024 << " MB";

    if (cleaned > 0) {

        QTimer *timer = new QTimer(this);
        int *progress = new int(0);

        connect(timer, &QTimer::timeout, this, [=]() mutable {
            if (*progress < 100) {
                *progress += 5;
                ui->progressCleanBar->setValue(*progress);
            } else {
                timer->stop();
                delete progress;


                QMessageBox::information(this, "Deep Cleaning",
                                         "Was cleared: " + QString::number(cleaned / 1024 / 1024) + " MB");

                populateProcessList();
                    ui->progressCleanBar->setValue(0);
            }
        });

        timer->start(50);
    } else {
        QMessageBox::information(this, "Deep Cleaning", "No memory was cleared!");
            ui->progressCleanBar->setValue(0);
    }




}
