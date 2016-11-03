/*
FILE 
	unxsrad.c
	svn ID removed
PURPOSE
	unxsSPS job queue CLI standalone program	
LEGAL
	(C) Gary Wallis 2012 for Unixservice, LLC. All Rights Reserved.
	LICENSE file should be included in distribution.
OTHER
HELP

*/

#include "mysqlrad.h"
#include "local.h"
#include <ctype.h>
#include <openisp/template.h>

#define cLOGFILE "/var/log/unxsSPS.log"

//Global vars
MYSQL gMysql;
char gcQuery[8192]={""};
char *gcBuildInfo="svn ID removed
static FILE *gLfp=NULL;//log file
char gcHostname[100]={""};

//prototype TOC
int main(int iArgc, char *cArgv[]);
void ProcessJobQueue(void);
void DoSomeJob(unsigned uJob,char const *cJobData);
void logfileLine(const char *cFunction,const char *cLogline);

//external prototypes
unsigned TextConnectDb(void); //mysqlconnect.c
void unxsSPS(const char *cResult);
void unxsSPS(const char *cMessage)
{
}//void unxsSPS(const char *cMessage)

int main(int iArgc, char *cArgv[])
{
	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"Could not open logfile: %s\n",cLOGFILE);
		exit(300);
       	}

	if(gethostname(gcHostname,99)!=0)
	{
		logfileLine("ProcessJobQueue","gethostname() failed");
		exit(1);
	}
	logfileLine("main",gcHostname);

	//Uses login data from local.h
	if(TextConnectDb())
		exit(1);

	if(iArgc==2)
	{
		if(!strcmp(cArgv[1],"ProcessJobQueue"))
			ProcessJobQueue();
	}
	else if(1)
	{
		printf("usage: %s ProcessJobQueue\n",cArgv[0]);
	}
	fclose(gLfp);
	mysql_close(&gMysql);
	return(0);
}//main()


void ProcessJobQueue(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uJob=0;

	sprintf(gcQuery,"SELECT uJob,cJobName,cJobData FROM tJob,tServer"
			" WHERE tJob.uServer=tServer.uServer AND tServer.cLabel='%s'"
			" AND tJob.uJobStatus=1",
					gcHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessJobQueue",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uJob);
		if(!strcmp(field[1],"DoSomeJOb"))
			DoSomeJob(uJob,field[2]);
	}
	mysql_free_result(res);

}//void ProcessJobQueue(void)


void DoSomeJob(unsigned uJob,char const *cJobData)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uProject=0;
	unsigned uOnce=0;
	unsigned uTemplateSet=0;
	unsigned uTable=0;

	logfileLine("DoSomeJob","start");

	sscanf(cJobData,"uProject=%u;",&uProject);

	sprintf(gcQuery,"SELECT tProject.cLabel,tTable.cLabel,tTable.uTable,tProject.uTemplateSet FROM tProject,tTable"
			" WHERE tTable.uProject=tProject.uProject AND tProject.uProject=%u",
					uProject);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("DoSomeJob",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(uOnce)
		{
			sscanf(field[3],"%u",&uTemplateSet);
			printf("%s uTemplateSet:%u\n",field[0],uTemplateSet);
			uOnce=0;
		}
		sscanf(field[2],"%u",&uTable);
		printf("%s uTable:%u\n",field[1],uTable);
	}
	mysql_free_result(res);

	logfileLine("DoSomeJob","end");

}//void DoSomeJob(unsigned uJob,char const *cJobData)


void logfileLine(const char *cFunction,const char *cLogline)
{
	time_t luClock;
	char cTime[32];
	pid_t pidThis;
	const struct tm *tmTime;

	pidThis=getpid();

	time(&luClock);
	tmTime=localtime(&luClock);
	strftime(cTime,31,"%b %d %T",tmTime);

        fprintf(gLfp,"%s unxsSPS[%u].%s: %s\n",cTime,pidThis,cFunction,cLogline);
	fflush(gLfp);

}//void logfileLine(char *cLogline)
