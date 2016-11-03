/*
FILE
	copyCloneQuota.c
	svn ID removed
PURPOSE
	Fix missing /var/vzquota/quota.VEID files that are
	now correctly copied by jobqueue.c on CloneContainer()
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2012.
	GPLv2 License applies. See LICENSE file.
NOTES
	Must run from dir that has the sipsettings.MYI and .MYD files.
*/

#include "../../mysqlrad.h"
#include <sys/sysinfo.h>

MYSQL gMysql;
char gcQuery[8192]={""};
char gcHostname[100]={""};
char gcProgram[100]={""};
unsigned guNode=0;
unsigned guDatacenter=0;

//dir protos
void TextConnectDb(void);
void TransferQuotaFile(void);

//local protos
void logfileLine(const char *cFunction,const char *cLogline,const unsigned uContainer);

static FILE *gLfp=NULL;
void logfileLine(const char *cFunction,const char *cLogline,const unsigned uContainer)
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

		fprintf(gLfp,"%s copyCloneQuota::%s[%u]: %s. uContainer=%u\n",cTime,cFunction,pidThis,cLogline,uContainer);
		fflush(gLfp);
	}
	else
	{
		fprintf(stderr,"%s: copyCloneQuota::%s. uContainer=%u\n",cFunction,cLogline,uContainer);
	}

}//void logfileLine()


int main(int iArgc, char *cArgv[])
{
	struct sysinfo structSysinfo;

	sprintf(gcProgram,"%.99s",cArgv[0]);

	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		logfileLine("main","fopen logfile failed",0);
		exit(1);
	}

	if(sysinfo(&structSysinfo))
	{
		logfileLine("main","sysinfo() failed",0);
		exit(1);
	}
#define LINUX_SYSINFO_LOADS_SCALE 65536
#define JOBQUEUE_MAXLOAD 20 //This is equivalent to uptime 20.00 last 1 min avg load
	if(structSysinfo.loads[0]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		logfileLine("main","structSysinfo.loads[0] larger than JOBQUEUE_MAXLOAD",0);
		exit(1);
	}
	//Check to see if this program is still running. If it is exit.
	//This may mean losing data gathering data points. But it
	//will avoid runaway du and other unexpected high load
	//situations. See #120.

	//Uses login data from ../../../local.h
	TextConnectDb();

	gethostname(gcHostname,98);
	//short hostname
	char *cp;
	if((cp=strchr(gcHostname,'.')))
		*cp=0;
        MYSQL_RES *res;
        MYSQL_ROW field;
	sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode"
			" WHERE cLabel='%s'",gcHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("main",mysql_error(&gMysql),0);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&guNode);
		sscanf(field[1],"%u",&guDatacenter);
	}
	mysql_free_result(res);
	if(!guNode)
	{
		fprintf(stderr,"could not determine node for %s\n",gcHostname);
		logfileLine("main","could not determine node",0);
		goto CommonExit;
	}

	if(iArgc==2)
	{
		if(!strncmp(cArgv[1],"TransferQuotaFile",25))
		{
			TransferQuotaFile();
			goto CommonExit;
		}
	}

	printf("Usage: %s TransferQuotaFile\n",gcProgram);

CommonExit:
	mysql_close(&gMysql);
	fclose(gLfp);
	return(0);

}//main()


void TransferQuotaFile(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_ROW field2;
	char cRemoteServer[100]={""};

	sprintf(gcQuery,"SELECT uContainer,uSource FROM tContainer"
			" WHERE uNode=%u AND uSource!=0 AND uStatus=31",guNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		fprintf(stderr,gcQuery);
		logfileLine("TransferQuotaFile",mysql_error(&gMysql),0);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	//debug only
	//if((field=mysql_fetch_row(res)))
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"SELECT tProperty.cValue FROM tProperty,tNode,tContainer"
			" WHERE tProperty.uKey=tNode.uNode"
			" AND tContainer.uNode=tNode.uNode"
			" AND tProperty.cName='Name'"
			" AND tContainer.uContainer=%s"
			" AND (tContainer.uStatus=31 OR tContainer.uStatus=1)"
			" AND tProperty.uType=2",field[1]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			strcat(gcQuery,"\n");
			fprintf(stderr,gcQuery);
			logfileLine("TransferQuotaFile",mysql_error(&gMysql),0);
			mysql_close(&gMysql);
			exit(2);
		}
        	res2=mysql_store_result(&gMysql);
		cRemoteServer[0]=0;
		if((field2=mysql_fetch_row(res2)))
			sprintf(cRemoteServer,"%.99s",field2[0]);
		mysql_free_result(res2);

		if(!cRemoteServer[0])
		{
			sprintf(gcQuery,"No cRemoteServer VEIDs Local:%s Remote:%s",field[0],field[1]);
			logfileLine("TransferQuotaFile",gcQuery,0);
			strcat(gcQuery,"\n");
			fprintf(stderr,gcQuery);
			continue;
		}
		sprintf(gcQuery,"scp %s:/var/vzquota/quota.%s /var/vzquota/quota.%s ",cRemoteServer,field[1],field[0]);
		if(system(gcQuery))
		{
			logfileLine("TransferQuotaFile",gcQuery,0);
			strcat(gcQuery,"\n");
			fprintf(stderr,gcQuery);
		}
	}
	mysql_free_result(res);

}//void TransferQuotaFile()
