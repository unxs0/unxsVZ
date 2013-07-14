#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QString>
#include <stdio.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QTimer *timer=new QTimer(this);

    connect(timer,SIGNAL(timeout()),this,SLOT(updateProgressBar()));
    //connect(timer,SIGNAL(timeout()),this,SLOT(quit()));
    timer->start(200);

    uCounter=0;

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateProgressBar()
{


    if(uCounter<=100)
    {
        ui->progressBar->setValue(100-uCounter);
        uCounter++;
    }
    else
    {
        char cLabel[8];
        sprintf(cLabel,"%u",uCounter);
        uCounter=0;
        QString cOTP1Label = cLabel;
        ui->cOTP1->setText(cOTP1Label);
    }

}
