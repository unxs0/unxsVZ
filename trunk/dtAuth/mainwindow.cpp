#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QString>
#include <stdio.h>
#include <liboath/oath.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QTimer *timer=new QTimer(this);

    connect(timer,SIGNAL(timeout()),this,SLOT(updateProgressBar()));
    //connect(timer,SIGNAL(timeout()),this,SLOT(quit()));
    timer->start(200);

    uCounter=0;
    sprintf(cOTPSecret[0],"%.16s","ABD5AGDT25HGADHU");
    sprintf(cOTPSecret[1],"%.16s","ABD5SDT25FHGADHU");
    sprintf(cOTPSecret[2],"%.16s","ABDGHDT2F5HGADHU");
    sprintf(cOTPSecret[3],"%.16s","ABD5AGDTHUHGADHU");
    sprintf(cOTPSecret[4],"%.16s","ABD5AGDWJSHGADHU");

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
        uCounter=0;

        int rc,i;
        time_t timeNow=time(NULL);

        rc=oath_init();
        if(rc!=OATH_OK)
            return;

        for(i=0;i<5;i++)
        {

            rc=oath_totp_generate(cOTPSecret[i],sizeof(cOTPSecret[i]),timeNow,30,0,6,cOTP[i]);
            if(rc==OATH_OK)
            {
                QString cOTPLabel = cOTP[i];
                switch(i)
                {
                case 0:
                   ui->cOTP1->setText(cOTPLabel);
                    break;
                case 1:
                    ui->cOTP2->setText(cOTPLabel);
                    break;
                case 2:
                    ui->cOTP3->setText(cOTPLabel);
                    break;
                case 3:
                    ui->cOTP4->setText(cOTPLabel);
                    break;
                case 4:
                    ui->cOTP5->setText(cOTPLabel);
                    break;
                }
            }
        }
    }
}//void MainWindow::updateProgressBar()
