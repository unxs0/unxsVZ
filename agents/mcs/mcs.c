/*
FILE
	mcs.c
	$Id$
PURPOSE
	Collection of diverse VZ operating parameters and other system vars.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2011.
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include "../../mysqlrad.h"
#include <sys/sysinfo.h>

MYSQL gMysql;
MYSQL gMysqlExt;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};
char gcProgram[100]={""};
unsigned guNodeOwner=1;
unsigned guContainerOwner=1;
unsigned guStatus=0;//not a valid status

//dir protos
void TextConnectDb(void);

//local protos
void Process(void);
void TextConnectOpenMCSDb(void);

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

		fprintf(gLfp,"%s unxsMCS::%s[%u]: %s. uContainer=%u\n",cTime,cFunction,pidThis,cLogline,uContainer);
		fflush(gLfp);
	}
	else
	{
		fprintf(stderr,"%s: unxsMCS::%s. uContainer=%u\n",cFunction,cLogline,uContainer);
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
#define JOBQUEUE_MAXLOAD 10 //This is equivalent to uptime 20.00 last 1 min avg load
	if(structSysinfo.loads[0]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		logfileLine("main","structSysinfo.loads[0] larger than JOBQUEUE_MAXLOAD",0);
		exit(1);
	}
	//Check to see if this program is still running. If it is exit.
	//This may mean losing data gathering data points. But it
	//will avoid runaway du and other unexpected high load
	//situations. See #120.

	if(iArgc==2)
	{
		if(!strncmp(cArgv[1],"Process",9))
		{
			Process();
			goto CommonExit;
		}
	}

	printf("Usage: %s Process\n",gcProgram);

CommonExit:
	fclose(gLfp);
	return(0);

}//main()


void Process(void)
{
        MYSQL_RES *res2;
        MYSQL_ROW field2;
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContainer=0;
	unsigned uOwner=0;

	//Uses login data from local.h
	TextConnectDb();
	TextConnectOpenMCSDb();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT tContainer.cHostname,tContainer.uContainer,tContainer.uOwner FROM tContainer,tGroupGlue,tGroup"
			" WHERE tGroupGlue.uContainer=tContainer.uContainer"
			" AND tGroup.uGroup=tGroupGlue.uGroup"
			" AND tGroup.cLabel LIKE '%%PBX%%'"
			" AND tContainer.uSource=0");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("Process",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	//debug only
	//if((field=mysql_fetch_row(res)))
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uContainer);
		sscanf(field[2],"%u",&uOwner);

		sprintf(gcQuery,"SELECT MAX(dUpJitter),MAX(dDownJitter),MIN(dMOS) FROM tVoipTest WHERE cSID LIKE '%%/%s'",field[0]);
		mysql_query(&gMysqlExt,gcQuery);
		if(mysql_errno(&gMysqlExt))
		{
			logfileLine("Process",mysql_error(&gMysqlExt),uContainer);
			mysql_close(&gMysql);
			mysql_close(&gMysqlExt);
			exit(2);
		}
		res2=mysql_store_result(&gMysqlExt);
		if((field2=mysql_fetch_row(res2)))
		{
			//Clean out
			sprintf(gcQuery,"DELETE FROM tProperty WHERE cName='cOrg_MCS_QOS' "
					" AND uType=3 AND uKey=%u",uContainer);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("Process",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}

			sprintf(gcQuery,"INSERT INTO tProperty SET cName='cOrg_MCS_QOS',cValue='dUpJitter=%s dDownJitter=%s dMOS=%s'"
					",uType=3,uKey=%u,uOwner=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uCreatedBy=1",
						field2[0],field2[1],field2[2],uContainer,uOwner);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("Process",mysql_error(&gMysql),uContainer);
				mysql_close(&gMysql);
				mysql_close(&gMysqlExt);
				exit(2);
			}
		}
		mysql_free_result(res2);
		
	}
	mysql_free_result(res);

	//Everything is cool we can clean up 
	sprintf(gcQuery,"TRUNCATE tVoipTest");
	mysql_query(&gMysqlExt,gcQuery);
	if(mysql_errno(&gMysqlExt))
	{
		logfileLine("Process",mysql_error(&gMysqlExt),uContainer);
		mysql_close(&gMysql);
		mysql_close(&gMysqlExt);
		exit(2);
	}

	mysql_close(&gMysql);
	mysql_close(&gMysqlExt);

	exit(0);

}//void Process(void)


void TextConnectOpenMCSDb(void)
{

#include "local.h"

        mysql_init(&gMysqlExt);
        if (!mysql_real_connect(&gMysqlExt,NULL,"mcs",MCSPWD,"mcs",0,NULL,0))
	{
		logfileLine("TextConnectOpenMCSDb","mysql_real_connect()",0);
		exit(3);
	}

}//TextConnectOpenMCSDb()

