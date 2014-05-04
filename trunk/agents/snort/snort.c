/*
FILE
	snort.c
	$Id$
PURPOSE
	Agent to run on Snort IDS servers usually via barnyard2.
	E.g. Create DenyAccess tJob for certain events for all active nodes.

AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2008-2014.
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include "snort.h"
#include <sys/sysinfo.h>

#define cSNORTLOGFILE "/var/log/unxsSnortLog"

MYSQL gMysql;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};
char gcProgram[32]={""};
unsigned guNodeOwner=0;

//local protos
void logfileLine(const char *cFunction,const char *cLogline);
void ProcessBarnyard(void);
//external protos
void TextConnectDb(void);
void TextLocalConnectDb(void);

unsigned guLogLevel=3;
static FILE *gLfp=NULL;
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

		fprintf(gLfp,"%s unxsSnort.%s[%u]: %s.\n",cTime,cFunction,pidThis,cLogline);
		fflush(gLfp);
	}

}//void logfileLine()

int main(int iArgc, char *cArgv[])
{
	if(iArgc>1)
	{
		register int i;
		for(i=1;i<iArgc;i++)
		{
			if(!strcmp(cArgv[i],"--help"))
			{
				printf("usage: %s [--help] [--version]\n",cArgv[0]);
				exit(0);
			}
			if(!strcmp(cArgv[i],"--version"))
			{
				printf("version: %s $Id$\n",cArgv[0]);
				exit(0);
			}
		}
	}

	char cLockfile[64]={"/tmp/snort.lock"};

	sprintf(gcProgram,"%.31s",cArgv[0]);
	if((gLfp=fopen(cSNORTLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
		exit(1);
	}
		
	struct stat structStat;
	if(!stat(cLockfile,&structStat))
	{
		logfileLine("main","waiting for rmdir(cLockfile)");
		return(1);
	}
	if(mkdir(cLockfile,S_IRUSR|S_IWUSR|S_IXUSR))
	{
		logfileLine("main","could not open cLockfile dir");
		return(1);
	}

	ProcessBarnyard();

	if(rmdir(cLockfile))
	{
		logfileLine("main","could not rmdir(cLockfile)");
		return(1);
	}
	logfileLine("main","end");
	return(0);
}//main()


void ProcessBarnyard(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	logfileLine("ProcessBarnyard","start");
	if(gethostname(cHostname,99)!=0)
	{
		logfileLine("ProcessNodeOVZ","gethostname() failed");
		exit(1);
	}

	//Uses login data from local.h
	TextConnectDb();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT tNode.uNode,tDatacenter.uDatacenter FROM tNode,tDatacenter"
			" WHERE tDatacenter.uStatus=1 AND tNode.uStatus=1"
			" AND tNode.uDatacenter=tDatacenter.uDatacenter");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessBarnyard",mysql_error(&gMysql));
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	unsigned uNode=0;
	unsigned uDatacenter=0;
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
	}
	mysql_free_result(res);

	mysql_close(&gMysql);
	logfileLine("ProcessBarnyard","end");

}//void ProcessBarnyard(void)


