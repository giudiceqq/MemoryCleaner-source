/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *CleanVersion;
    QListWidget *processList;
    QLabel *totalMemoryConsumption;
    QPushButton *cleanButton;
    QCheckBox *AutoStartCheckBox;
    QCheckBox *WorkInBackGroundCheckBox;
    QProgressBar *progressCleanBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(454, 572);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayoutWidget = new QWidget(centralwidget);
        gridLayoutWidget->setObjectName("gridLayoutWidget");
        gridLayoutWidget->setGeometry(QRect(0, 0, 451, 81));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        CleanVersion = new QLabel(gridLayoutWidget);
        CleanVersion->setObjectName("CleanVersion");
        CleanVersion->setEnabled(true);
        QFont font;
        font.setFamilies({QString::fromUtf8("Arial")});
        font.setPointSize(24);
        font.setBold(false);
        CleanVersion->setFont(font);
        CleanVersion->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        CleanVersion->setAutoFillBackground(false);
        CleanVersion->setScaledContents(true);
        CleanVersion->setAlignment(Qt::AlignmentFlag::AlignCenter);
        CleanVersion->setWordWrap(false);

        gridLayout->addWidget(CleanVersion, 0, 0, 1, 1);

        processList = new QListWidget(centralwidget);
        processList->setObjectName("processList");
        processList->setGeometry(QRect(10, 120, 211, 411));
        totalMemoryConsumption = new QLabel(centralwidget);
        totalMemoryConsumption->setObjectName("totalMemoryConsumption");
        totalMemoryConsumption->setGeometry(QRect(10, 80, 211, 36));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Arial")});
        font1.setPointSize(16);
        totalMemoryConsumption->setFont(font1);
        totalMemoryConsumption->setAutoFillBackground(false);
        totalMemoryConsumption->setScaledContents(false);
        totalMemoryConsumption->setAlignment(Qt::AlignmentFlag::AlignCenter);
        cleanButton = new QPushButton(centralwidget);
        cleanButton->setObjectName("cleanButton");
        cleanButton->setGeometry(QRect(260, 490, 131, 41));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Arial")});
        font2.setPointSize(12);
        cleanButton->setFont(font2);
        AutoStartCheckBox = new QCheckBox(centralwidget);
        AutoStartCheckBox->setObjectName("AutoStartCheckBox");
        AutoStartCheckBox->setGeometry(QRect(260, 140, 171, 21));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("Arial")});
        font3.setPointSize(14);
        AutoStartCheckBox->setFont(font3);
        WorkInBackGroundCheckBox = new QCheckBox(centralwidget);
        WorkInBackGroundCheckBox->setObjectName("WorkInBackGroundCheckBox");
        WorkInBackGroundCheckBox->setGeometry(QRect(260, 180, 191, 21));
        WorkInBackGroundCheckBox->setFont(font3);
        progressCleanBar = new QProgressBar(centralwidget);
        progressCleanBar->setObjectName("progressCleanBar");
        progressCleanBar->setGeometry(QRect(10, 540, 431, 23));
        progressCleanBar->setValue(0);
        progressCleanBar->setTextVisible(true);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        CleanVersion->setText(QCoreApplication::translate("MainWindow", "Memory Cleaner V1.0 ", nullptr));
        totalMemoryConsumption->setText(QCoreApplication::translate("MainWindow", "18000/64000", nullptr));
        cleanButton->setText(QCoreApplication::translate("MainWindow", "[ Clean Memory ]", nullptr));
        AutoStartCheckBox->setText(QCoreApplication::translate("MainWindow", "Auto-start on boot ", nullptr));
        WorkInBackGroundCheckBox->setText(QCoreApplication::translate("MainWindow", " Background mode ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
