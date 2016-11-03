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
MYSQL gMysqlExt;
char gcQuery[8192]={""};
char *gcBuildInfo="svn ID removed
static FILE *gLfp=NULL;//log file
char gcHostname[100]={""};

//prototype TOC
int main(int iArgc, char *cArgv[]);
void ProcessJobQueue(void);
void DoSomeJob(unsigned uJob,char const *cJobData);
void logfileLine(const char *cFunction,const char *cLogline);
void LoadPBXsFromOpenSIPS(void);
void GetConfiguration(const char *cName,char *cValue,unsigned ucValueLen,unsigned uServer);
void TextConnectOpenSIPSDb(void);
void LoadDIDsFromOpenSIPS(void);

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
		else if(!strcmp(cArgv[1],"LoadPBXsFromOpenSIPS"))
			LoadPBXsFromOpenSIPS();
		else if(!strcmp(cArgv[1],"LoadDIDsFromOpenSIPS"))
			LoadDIDsFromOpenSIPS();
	}
	else if(1)
	{
		printf("usage: %s ProcessJobQueue LoadPBXsFromOpenSIPS LoadDIDsFromOpenSIPS\n",cArgv[0]);
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


void LoadPBXsFromOpenSIPS(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	/*
	char cOpenSIPS_Server[64]={""};
	char cOpenSIPS_Db[64]={""};
	char cOpenSIPS_Passwd[64]={""};
	GetConfiguration("cOpenSIPS_Server",cOpenSIPS_Server,63,0);
	GetConfiguration("cOpenSIPS_Db",cOpenSIPS_Db,63,0);
	GetConfiguration("cOpenSIPS_Passwd",cOpenSIPS_Db,63,0);
	*/

	logfileLine("LoadPBXsFromOpenSIPS","start");

	TextConnectOpenSIPSDb();

	sprintf(gcQuery,"SELECT address,attrs,description,gwid FROM dr_gateways WHERE type=1");
	mysql_query(&gMysqlExt,gcQuery);
	if(mysql_errno(&gMysqlExt))
	{
		logfileLine("LoadPBXsFromOpenSIPS",mysql_error(&gMysqlExt));
		return;
	}
        res=mysql_store_result(&gMysqlExt);
	while((field=mysql_fetch_row(res)))
	{
		char *cp;
		char cLabel[32]={""};
		unsigned uLines=0;

		printf("%s %s %s\n",field[0],field[1],field[2]);

		sprintf(cLabel,"%.31s",field[0]);
		if((cp=strchr(cLabel,'.'))) *cp=0;

		if((cp=strchr(field[1],'|')))
			sscanf(cp+1,"%u",&uLines);

		sprintf(gcQuery,"INSERT INTO tPBX SET uPBX=%s,cLabel='%s',cHostname='%s',cAttributes='%s',cDescription='%s'"
				",uLines=%u"
				",uCluster=1"
				",uStatus=1"
				",cComment='LoadPBXsFromOpenSIPS()'"
				",uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				field[3],cLabel,field[0],field[1],field[2],
				uLines);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("LoadPBXsFromOpenSIPS",mysql_error(&gMysql));
			return;
		}
	}
	mysql_free_result(res);

	logfileLine("LoadPBXsFromOpenSIPS","end");

}//void LoadPBXsFromOpenSIPS(void)


void GetConfiguration(const char *cName,char *cValue,unsigned ucValueLen,unsigned uServer)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        char cQuery[256];
	char cExtra[64]={""};
	char cFormat[32]={"%.255s"};

        sprintf(cQuery,"SELECT cValue FROM tConfiguration WHERE cLabel='%.32s'",cName);
	if(uServer)
	{
		sprintf(cExtra," AND uServer=%u",uServer);
		strcat(cQuery,cExtra);
	}
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
	{
		logfileLine("GetConfiguration",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if(ucValueLen)
        	sprintf(cFormat,"%%.%us",ucValueLen);
		
        if((field=mysql_fetch_row(res)))
        	sprintf(cValue,cFormat,field[0]);
        mysql_free_result(res);

}//void GetConfiguration()


void TextConnectOpenSIPSDb(void)
{

        mysql_init(&gMysqlExt);
        if (!mysql_real_connect(&gMysqlExt,NULL,"opensips","wsxedc","opensips",0,NULL,0))
	{
		logfileLine("TextConnectOpenSIPSDb","mysql_real_connect()");
		exit(3);
	}

}//TextConnectOpenSIPSDb()


void LoadDIDsFromOpenSIPS(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_ROW field2;
	/*
	char cOpenSIPS_Server[64]={""};
	char cOpenSIPS_Db[64]={""};
	char cOpenSIPS_Passwd[64]={""};
	GetConfiguration("cOpenSIPS_Server",cOpenSIPS_Server,63,0);
	GetConfiguration("cOpenSIPS_Db",cOpenSIPS_Db,63,0);
	GetConfiguration("cOpenSIPS_Passwd",cOpenSIPS_Db,63,0);
	*/

	logfileLine("LoadDIDsFromOpenSIPS","start");

	TextConnectOpenSIPSDb();

/*
+--------+---------+-------------+---------+----------+---------+--------+-------+-------------------------------------+
| ruleid | groupid | prefix      | timerec | priority | routeid | gwlist | attrs | description                         |
+--------+---------+-------------+---------+----------+---------+--------+-------+-------------------------------------+
|   7243 | 1       | 15122982130 |         |        0 |         | 388    | NULL  | Bandwidth |Reseller San Antonio GCS |
|   7244 | 1       | 15122982131 |         |        0 |         | 388    | NULL  | Bandwidth |Reseller San Antonio GCS |
|   7242 | 1       | 12063169001 |         |        0 |         | 263    | NULL  | Americas Locksmith | Bandwidth      |
|     10 | 1       | 5084844730  |         |        0 | 0       | 19     |       | Nine Technologies                   |
|     11 | 1       | 5084844731  |         |        0 | 0       | 19     |       | Nine Technologies                   |
*/


	sprintf(gcQuery,"SELECT prefix,gwlist,description FROM dr_rules"
			" WHERE groupid=1"
			" AND gwlist IS NOT NULL"
			" AND gwlist!=''");
	mysql_query(&gMysqlExt,gcQuery);
	if(mysql_errno(&gMysqlExt))
	{
		logfileLine("LoadDIDsFromOpenSIPS",mysql_error(&gMysqlExt));
		return;
	}
        res=mysql_store_result(&gMysqlExt);
	while((field=mysql_fetch_row(res)))
	{
		unsigned uCarrier=0;

		printf("%s %s %s\n",field[0],field[1],field[2]);

		sprintf(gcQuery,"SELECT uCarrier FROM tCarrier WHERE INSTR('%s',cLabel)>0",field[2]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("LoadDIDsFromOpenSIPS",mysql_error(&gMysql));
			return;
		}
        	res2=mysql_store_result(&gMysql);
		if((field2=mysql_fetch_row(res2)))
			sscanf(field2[0],"%u",&uCarrier);
		sprintf(gcQuery,"INSERT INTO tDID SET cLabel='%s',cDID='%s',uPBX=%s"
				",cComment='%s -ldfo'"
				",uCluster=1"
				",uCarrier=%u"
				",uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				field[0],field[0],field[1],field[2],uCarrier);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s\n",gcQuery);
			logfileLine("LoadDIDsFromOpenSIPS",mysql_error(&gMysql));
		}
	}
	mysql_free_result(res);

	logfileLine("LoadDIDsFromOpenSIPS","end");

}//void LoadDIDsFromOpenSIPS(void)

