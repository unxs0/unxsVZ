/*
FILE
	sms.c
	$Id: sms.c 1466 2010-05-18 16:32:59Z Gary $
PURPOSE
	SMS gateway preprocessor interface
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2010.
	GPLv2 License applies. See LICENSE file.
NOTES
	See README in this dir.
*/

#include "mysqlrad.h"
#include <sys/sysinfo.h>
#include <ctype.h>

MYSQL gMysql;
FILE *gLfp=NULL;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};
char gcProgram[32]={""};
unsigned guStatus=0;//not a valid status
unsigned uDebug=1;

//extern same dir
void TextConnectDb(void);

//this file protos TOC
void Run(void);
void Set(const char *cPhone,const char *cuDigestThreshold,const char *cuReceivePeriod,const char *cuSendPeriod,const char *cuPeriodCount);
void QueueMessage(const char *cPhone,const char *cMessage);
void logfileLine(const char *cFunction,const char *cLogline);
int main(int iArgc, char *cArgv[]);
void Initialize(const char *cPasswd);
void ErrorExit(void);
void NormalExit(void);
void mySQLRootConnect(const char *cPasswd);

#define macro_MySQLQueryBasic \
	mysql_query(&gMysql,gcQuery);\
	if(mysql_errno(&gMysql))\
	{\
		printf("%s\n",mysql_error(&gMysql));\
		ErrorExit();\
	}


void Run(void)
{
}//void Run(void)


void Set(const char *cPhone,const char *cuDigestThreshold,const char *cuReceivePeriod,const char *cuSendPeriod,const char *cuPeriodCount)
{
}//void Set()


void QueueMessage(const char *cPhone,const char *cMessage)
{
}//void QueueMessage()


void logfileLine(const char *cFunction,const char *cLogline)
{
	if(gLfp!=NULL)
	{
		time_t luClock;
		char cTime[32];
		pid_t pidThis;
		const struct tm *tmTime;

		pidThis=getpid();

		time(&luClock);
		tmTime=localtime(&luClock);
		strftime(cTime,31,"%b %d %T",tmTime);

		fprintf(gLfp,"%s %s[%u]: %s.\n",cTime,cFunction,pidThis,cLogline);
		if(uDebug)
			fprintf(stdout,"%s %s[%u]: %s.\n",cTime,cFunction,pidThis,cLogline);
		fflush(gLfp);
	}

}//void logfileLine()


int main(int iArgc, char *cArgv[])
{
	struct sysinfo structSysinfo;

	if((gLfp=fopen("/var/log/unxsSMS.log","a"))==NULL)
	{
		fprintf(stderr,"Could not open /var/log/unxsSMS.log error exit\n");
		exit(1);
	}

	sprintf(gcProgram,"%.31s",cArgv[0]);

	if(sysinfo(&structSysinfo))
	{
		logfileLine("main","sysinfo() failed");
		ErrorExit();
	}
#define LINUX_SYSINFO_LOADS_SCALE 65536
#define JOBQUEUE_MAXLOAD 20 //This is equivalent to uptime 20.00 last 1 min avg load
	if(structSysinfo.loads[0]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		logfileLine("main","structSysinfo.loads[0] larger than JOBQUEUE_MAXLOAD");
		ErrorExit();
	}

	switch(iArgc)
	{
		case 1:
			printf("Usage: %s <cPhone> <cMessage> | initialize <mysql password> | run | debug |\n"
				"\t\t\tset <cPhone> <uDigestThreshold> <uReceivePeriod> <uSendPeriod> <uPeriodCount>\n",cArgv[0]);
		break;

		case 2:
			if(!strncmp(cArgv[1],"run",3))
			{
				uDebug=0;
				Run();
			}
			else if(!strncmp(cArgv[1],"debug",5))
			{
				uDebug=1;
				Run();
			}
			else
			{
				sprintf(gcQuery,"unknown command cArgv[1]=%.32s",cArgv[1]);
				logfileLine("main",gcQuery);
				ErrorExit();
			}
		break;

		case 3:
			if(!strncmp(cArgv[1],"initialize",10))
			{
				Initialize(cArgv[2]);
			}
			else if(isdigit(cArgv[1][0]) && strlen(cArgv[1])<33 && strlen(cArgv[2])<145)
			{
				QueueMessage(cArgv[1],cArgv[2]);
			}
			else
			{
				sprintf(gcQuery,"unknown command cArgv[1]=%.32s cArgv[2]=%.140s",cArgv[1],cArgv[2]);
				logfileLine("main",gcQuery);
				ErrorExit();
			}
		break;

		case 7:
			if(!strncmp(cArgv[1],"set",10))
			{
				Set(cArgv[2],cArgv[3],cArgv[4],cArgv[5],cArgv[6]);
			}
			else
			{
				sprintf(gcQuery,"unknown command cArgv[1]=%.32s cArgv[2]=%.32s cArgv[3]=%.32s cArgv[4]=%.32s cArgv[5]=%.32s cArgv[6]=%.32s",
					cArgv[1],cArgv[2],cArgv[3],cArgv[4],cArgv[5],cArgv[6]);
				logfileLine("main",gcQuery);
				ErrorExit();
			}
		break;

		default:
			sprintf(gcQuery,"unknown command iArgc=%d",iArgc);
			logfileLine("main",gcQuery);
			ErrorExit();
	}

	NormalExit();
	return(0);

}//main()


void Initialize(const char *cPasswd)
{
	if(uDebug)
		printf("Initialize()\n");

	if(getuid()!=0)
	{
		logfileLine("Initialize","Must be root to run Initialize");
		ErrorExit();
	}

	mySQLRootConnect(cPasswd);

	sprintf(gcQuery,"DROP DATABASE IF EXISTS unxssms");
	macro_MySQLQueryBasic;

	sprintf(gcQuery,"CREATE DATABASE unxssms");
	macro_MySQLQueryBasic;

	sprintf(gcQuery,"USE unxssms");
	macro_MySQLQueryBasic;

	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tPhone ("
			" uPhone INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			" cNumber VARCHAR(32) NOT NULL DEFAULT '',"
			" uDigestThreshold INT UNSIGNED NOT NULL DEFAULT 0,"
			" uReceivePeriod INT UNSIGNED NOT NULL DEFAULT 0,"
			" uSendPeriod INT UNSIGNED NOT NULL DEFAULT 0,"
			" uPeriodCount INT UNSIGNED NOT NULL DEFAULT 0"
			" )");
	macro_MySQLQueryBasic;

	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tQueue ("
			" uQueue INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			" cMessage VARCHAR(140) NOT NULL DEFAULT '',"
			" uPhone INT UNSIGNED NOT NULL DEFAULT 0,"
			" uDateCreated INT UNSIGNED NOT NULL DEFAULT 0"
			" )");
	macro_MySQLQueryBasic;

}//void Initialize()


void ErrorExit(void)
{
	fclose(gLfp);
	exit(1);
}//void ErrorExit(void)


void NormalExit(void)
{
	fclose(gLfp);
	exit(0);
}//void NormalExit(void)


void mySQLRootConnect(const char *cPasswd)
{
        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,NULL,"root",cPasswd,"mysql",0,NULL,0))
        {
		logfileLine("mySQLRootConnect","MySQL server not available");
		ErrorExit();
        }

}//void mySQLRootConnect(void)
