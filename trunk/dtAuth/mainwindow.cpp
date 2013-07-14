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
    timer->start(300);

    uCounter=0;

    //todo load this from file
    sprintf(cOTPSecret[0],"%.16s","ABD5AGDT25HGADHU");
    sprintf(cOTPSecret[1],"%.16s","ABD5SDT25FHGADHU");
    sprintf(cOTPSecret[2],"%.16s","ABDGHDT2F5HGADHU");
    sprintf(cOTPSecret[3],"%.16s","ABD5AGDTHUHGADHU");
    sprintf(cOTPSecret[4],"%.16s","ABD5AGDWJSHGADHU");

    sprintf(cOTPName[0],"%.31s","root");
    sprintf(cOTPName[1],"%.31s","user@isp.net");
    sprintf(cOTPName[2],"%.31s","jdoe@isp.com");
    sprintf(cOTPName[3],"%.31s","sipagent");
    sprintf(cOTPName[4],"%.31s","dbadmin");

    ui->setupUi(this);

    loadData();
    updateOTP();

}//MainWindow::MainWindow()


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
        updateOTP();

    }
}//void MainWindow::updateProgressBar()


void MainWindow::updateOTP()
{
    int rc,i;

    rc=oath_init();
    if(rc!=OATH_OK)
        return;

    for(i=0;i<5;i++)
    {
        time_t timeNow=time(NULL);
        char *secret;
        size_t secretlen;
        rc=oath_base32_decode(cOTPSecret[i],strlen(cOTPSecret[i]),&secret,&secretlen);
        if(rc!=OATH_OK)
            return;
        rc=oath_totp_generate(secret,secretlen,timeNow,30,0,6,cOTP[i]);
        if(rc==OATH_OK)
        {
            switch(i)
            {
            case 0:
                ui->cOTP1->setText(cOTP[i]);
                ui->cOTPName1->setText(cOTPName[i]);
                break;
            case 1:
                ui->cOTP2->setText(cOTP[i]);
                ui->cOTPName2->setText(cOTPName[i]);
                break;
            case 2:
                ui->cOTP3->setText(cOTP[i]);
                ui->cOTPName3->setText(cOTPName[i]);
                break;
            case 3:
                ui->cOTP4->setText(cOTP[i]);
                ui->cOTPName4->setText(cOTPName[i]);
                break;
            case 4:
                ui->cOTP5->setText(cOTP[i]);
                ui->cOTPName5->setText(cOTPName[i]);
                break;
            }
        }

    }

}//void MainWindow::updateOTP()


void MainWindow::loadData()
{
    FILE *fp;
    if((fp=fopen(".dtAuthData","r"))!=NULL)
    {
        char cLine[64];
        int i=0,j=0;

        while(fgets(cLine,63,fp)!=NULL)
        {
            char *cp=NULL,*cp2=NULL;
            if((cp=(strstr(cLine,"cOTPName="))))
            {
                if((cp2=(strchr(cp+strlen("cOTPName="),';'))))
                {
                    *cp2=0;
                    sprintf(cOTPName[i++],"%.31s",cp+strlen("cOTPName="));
                    *cp2=';';
                }
            }
            if((cp=(strstr(cLine,"cOTPSecret="))))
            {
                if((cp2=(strchr(cp+strlen("cOTPSecret="),';'))))
                {
                    *cp2=0;
                    sprintf(cOTPSecret[j++],"%.16s",cp+strlen("cOTPSecret="));
                    *cp2=';';
                }
            }
            if(i>4) i=0;
            if(j>4) j=0;
        }
        fclose(fp);
    }
}//void MainWindow::loadData()
