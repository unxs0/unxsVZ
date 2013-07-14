#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;

    //count down to new OTP
    unsigned uCounter;

    //initial dumb 5 fixed OTPs
    char cOTP[5][8];
    char cOTPSecret[5][32];
    char cOTPName[5][32];

    void updateOTP();
    void loadData();

public slots:
    void updateProgressBar();


};

#endif // MAINWINDOW_H
