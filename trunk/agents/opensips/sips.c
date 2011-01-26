/*
FILE
	sips.c
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
char gcProgram[32]={""};
unsigned guNodeOwner=1;
unsigned guContainerOwner=1;
unsigned guStatus=0;//not a valid status

//dir protos
void TextConnectDb(void);

//local protos
void ProcessDR(void);
void TextConnectOpenSIPSDb(void);

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

		fprintf(gLfp,"%s %s[%u]: %s. uContainer=%u\n",cTime,cFunction,pidThis,cLogline,uContainer);
		fflush(gLfp);
	}
	else
	{
		fprintf(stderr,"%s: %s. uContainer=%u\n",cFunction,cLogline,uContainer);
	}

}//void logfileLine()


int main(int iArgc, char *cArgv[])
{
	struct sysinfo structSysinfo;

	sprintf(gcProgram,"%.31s",cArgv[0]);

	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		logfileLine("ProcessDR","fopen logfile failed",0);
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

	ProcessDR();

	return(0);
}//main()


void ProcessDR(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_ROW field2;
	unsigned uContainer=0;

	//Uses login data from local.h
	TextConnectDb();
	TextConnectOpenSIPSDb();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT cHostname,uContainer FROM tContainer WHERE uSource=0");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessDR",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[1],"%u",&uContainer);

		//debug only
		printf("%s %u\n",field[0],uContainer);

		//Wish the OPenSIPS guys would use modern SQL var naming conventions
		//like we do ;)
		sprintf(gcQuery,"SELECT gwid,attrs FROM dr_gateways WHERE type=1 AND address='%s'",field[0]);
		mysql_query(&gMysqlExt,gcQuery);
		if(mysql_errno(&gMysqlExt))
		{
			logfileLine("ProcessDR",mysql_error(&gMysqlExt),uContainer);
			mysql_close(&gMysql);
			mysql_close(&gMysqlExt);
			exit(2);
		}
		res2=mysql_store_result(&gMysqlExt);
		if((field2=mysql_fetch_row(res2)))
		{
			//debug only
			printf("gwid=%s attrs=%s\n",field2[0],field2[1]);
		}
		else
		{
			logfileLine("ProcessDR",field[0],uContainer);
		}
		mysql_free_result(res2);
		
	}
	mysql_free_result(res);

	mysql_close(&gMysql);
	mysql_close(&gMysqlExt);
	exit(0);

}//void ProcessDR(void)


void TextConnectOpenSIPSDb(void)
{

#include "local.h"

        mysql_init(&gMysqlExt);
        if (!mysql_real_connect(&gMysqlExt,NULL,"opensips",OPENSIPSPWD,"opensips",0,NULL,0))
	{
		logfileLine("TextConnectOpenSIPSDb","mysql_real_connect()",0);
		exit(3);
	}

}//TextConnectOpenSIPSDb()

