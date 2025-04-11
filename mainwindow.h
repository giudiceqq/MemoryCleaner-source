#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>  
#include <QCloseEvent>     

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon; 

    void populateProcessList(); 
    void setAutoStart(bool enable);

private slots:
    void on_CleanVersion_linkActivated(const QString &link);
    void on_cleanButton_clicked();
    void exitApp();  
    void toggleAutoStart(bool checked);
    void toggleThresholdInput(bool checked);
    void validateThresholdInput();
    void run();
    void startMonitoring();
    void checkAndCreateLogFile();


};

#endif 
