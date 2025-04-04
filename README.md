# 🧠 MemoryCleaner – Source Code

This repository contains the full open-source source code of [MemoryCleaner](https://github.com/giudiceqq/MemoryCleaner) — a lightweight, fast, and powerful RAM cleaner for Windows.

MemoryCleaner analyzes RAM usage in real time and allows you to free up memory from background processes using native WinAPI and Qt.

---

## ⚙️ Tech Stack

- 🧠 C++17
- 📚 Qt 6.x (Widgets, Core, GUI)
- 🛠 Windows API (EmptyWorkingSet, Process Snapshots)
- 🔧 Compiles with MinGW (Qt Creator)

---

## 🚀 Features

- Clean memory from unused processes
- Process memory usage overview
- Manual and automatic cleaning
- Logging with memory stats
- Compact and modern Qt UI

---

## 🧱 How to Build

1. Open the `.pro` file in **Qt Creator**
2. Select the **MinGW 64-bit** kit
3. Click **Build & Run**
4. Done — you’ll get your own `MemoryCleaner.exe`

You can also build via console:

```bash
qmake MemoryCleaner.pro
mingw32-make release
```

---

## 📁 Project Structure

```
MemoryCleaner-Source/
├── main.cpp
├── mainwindow.cpp / .h / .ui
├── MemoryCleaner.pro
├── resources.qrc
├── icon/
│   └── icon.ico
└── ...
```

---

## 🔗 Binary / Release version

To download the compiled `.exe` and ready-to-run version of MemoryCleaner, visit:  
👉 [https://github.com/giudiceqq/MemoryCleaner](https://github.com/giudiceqq/MemoryCleaner)

---

## 📜 License

Feel free to use, fork, or contribute. Add your own features and share improvements!
