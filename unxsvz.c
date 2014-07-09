/*
FILE 
	unxsVZ/unxsvz.c
	$Id$
PURPOSE
	Move the jobqueue out of the cgi.
NOTES
AUTHOR/LEGAL
	(C) 2011-2014 Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
FREE HELP
	support @ openisp . net
	supportgrp @ unixservice . com
	Join mailing list: https://lists.openisp.net/mailman/listinfo/unxsvz
*/

#include "mysqlrad.h"
#include <ctype.h>

//Global vars

MYSQL gMysql;
MYSQL gMysqlUBC;
char *gcUBCDBIP0;//must be set to DBIP0 to be used as alternative
char *gcUBCDBIP1;
char gcUBCDBIP0Buffer[32]={""};
char gcUBCDBIP1Buffer[32]={""};
FILE *gLfp=NULL;
char gcCommand[100];
char cLogKey[16]={"Ksdj458jssdUjf79"};
char cStartDate[32]={""};
char cStartTime[32]={""};

int guPermLevel=0;
unsigned guLoginClient=0;
unsigned guReseller=0;
unsigned guCompany=0;
char gcCompany[100]={""};
char gcUser[100]={""};
char gcLogin[100]={""};
char gcHost[100]={""};
char gcHostname[100]={""};

char gcFunction[100]={""};
char gcQuery[8192]={""};

int main(int iArgc, char *cArgv[])
{
	gethostname(gcHostname,98);

	ExtMainShell(iArgc,cArgv);

	return(0);

}//end of main()

#include "local.h"
#include <dirent.h>
#include <openisp/ucidr.h>
void GetDatacenterProp(const unsigned uDatacenter,const char *cName,char *cValue);//tcontainerfunc.h
void SetContainerStatus(unsigned uContainer,unsigned uStatus);
void ChangeGroup(unsigned uContainer, unsigned uGroup);
void GetNodeProp(const unsigned uNode,const char *cName,char *cValue);//jobqueue.c
char *strptime(const char *s, const char *format, struct tm *tm);

//jobqueue.c
void ProcessJobQueue(unsigned uDebug);

static char cRELEASE={"$Id$"};

void ExtMainShell(int argc, char *argv[])
{
	if(getuid())
	{
		printf("Only root can run this command!\n");
		exit(0);
	}

        if(argc==2 && !strcmp(argv[1],"ProcessJobQueue"))
                ProcessJobQueue(0);
        if(argc==2 && !strcmp(argv[1],"ProcessJobQueueDebug"))
                ProcessJobQueue(1);
        else
	{
		printf("\n%s %s Menu\n",argv[0],cRELEASE);
		printf("\tProcessJobQueue\n");
		printf("\tProcessJobQueueDebug\n");
		printf("\n");
	}
	mysql_close(&gMysql);
        exit(0);


}//void ExtMainShell(int argc, char *argv[])

