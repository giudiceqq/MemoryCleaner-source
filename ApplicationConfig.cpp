#include "mainwindow.h"

#include <QApplication>
#include <QApplication>
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <iomanip>


class ApplicationConfig {
public:
    std::string APP_NAME;
    std::string APP_VERSION;


    ApplicationConfig()
    {
        this-> APP_NAME = " Memory Cleaner";
        this->APP_VERSION = "1.0 ";

    }

};

