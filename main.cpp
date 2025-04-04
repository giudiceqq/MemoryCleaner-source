#include "mainwindow.h"

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
#include <ApplicationConfig.cpp>





int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    // 454 572


    w.setFixedSize(454, 572);

    w.setWindowIcon(QIcon(":/new/p/icon.ico"));



    w.show();

    ApplicationConfig* Config = new ApplicationConfig();
    return a.exec();
}


