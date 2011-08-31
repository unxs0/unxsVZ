/*
FILE
	unxsvz-checkconf.c
	$Id$
PURPOSE
	Check unxsVZ managed OpenVZ server configuration.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2011-2012.
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include "../../mysqlrad.h"
#include <sys/sysinfo.h>

MYSQL gMysql;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};
char gcProgram[32]={""};
unsigned guNodeOwner=1;
unsigned guContainerOwner=1;
unsigned guStatus=0;//not a valid status

//local protos
void logfileLine(const char *cFunction,const char *cLogline,const unsigned uContainer);
void CheckConf(void);

//external
void TextConnectDb(void);

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

		fprintf(gLfp,"%s unxsvz-checkconf.%s[%u]: %s. uContainer=%u\n",cTime,cFunction,pidThis,cLogline,uContainer);
		fflush(gLfp);
	}

}//void logfileLine()

#define LINUX_SYSINFO_LOADS_SCALE 65536
#define JOBQUEUE_MAXLOAD 10 //This is equivalent to uptime 10.00 last 5 min avg load
struct sysinfo structSysinfo;

int main(int iArgc, char *cArgv[])
{
	sprintf(gcProgram,"%.31s",cArgv[0]);
	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
		exit(1);
	}
		
	if(sysinfo(&structSysinfo))
	{
		logfileLine("main","sysinfo() failed",0);
		exit(1);
	}
	if(structSysinfo.loads[1]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		logfileLine("main","structSysinfo.loads[1] larger than JOBQUEUE_MAXLOAD",0);
		exit(1);
	}
	//Check to see if this program is still running. If it is exit.
	//This may mean losing data gathering data points. But it
	//will avoid runaway du and other unexpected high load
	//situations. See #120.

	struct stat structStat;
	if(!stat("/tmp/unxsvz-checkconf.lock",&structStat))
	{
		logfileLine("main","waiting for rmdir(/tmp/unxsvz-checkconf.lock)",0);
		return(1);
	}
	if(mkdir("/tmp/unxsvz-checkconf.lock",S_IRUSR|S_IWUSR|S_IXUSR))
	{
		logfileLine("main","could not open /tmp/unxsvz-checkconf.lock dir",0);
		return(1);
	}
	CheckConf();
	if(rmdir("/tmp/unxsvz-checkconf.lock"))
	{
		logfileLine("main","could not rmdir(/tmp/unxsvz-checkconf.lock)",0);
		return(1);
	}
	return(0);
}//main()


void CheckConf(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uDatacenter=0;
	unsigned uNode=0;
	unsigned uContainer=0;
	FILE *fp;
	char cCommand[128];
	char cMD5Sum[64];

	if(gethostname(cHostname,99)!=0)
	{
		logfileLine("CheckConf","gethostname() failed",uContainer);
		exit(1);
	}

	//Uses login data from local.h
	TextConnectDb();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT uNode,uDatacenter,uOwner FROM tNode WHERE cLabel='%.99s'",cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("CheckConf",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
		sscanf(field[2],"%u",&guNodeOwner);
	}
	mysql_free_result(res);
	if(!uNode)
	{
		char *cp;

		//FQDN vs short name of 2nd NIC mess
		if((cp=strchr(cHostname,'.')))
			*cp=0;
		sprintf(gcQuery,"SELECT uNode,uDatacenter,uOwner FROM tNode WHERE cLabel='%.99s'",cHostname);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("CheckConf",mysql_error(&gMysql),uContainer);
			mysql_close(&gMysql);
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uNode);
			sscanf(field[1],"%u",&uDatacenter);
			sscanf(field[2],"%u",&guNodeOwner);
		}
		mysql_free_result(res);
	}

	if(!uNode)
	{
		logfileLine("CheckConf","Could not determine uNode",uContainer);
		mysql_close(&gMysql);
		exit(1);
	}

	//debug only
	printf("CheckConf() for %s (uNode=%u,uDatacenter=%u)\n",
			cHostname,uNode,uDatacenter);

	//Main loop. TODO use defines for tStatus.uStatus values.
	sprintf(gcQuery,"SELECT uContainer,uOwner,uStatus FROM tContainer WHERE uNode=%u"
				" AND uDatacenter=%u"
				" AND (uStatus=1"//Active
				" OR uStatus=31) LIMIT 10"//Stopped
						,uNode,uDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("CheckConf",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(sysinfo(&structSysinfo))
		{
			logfileLine("CheckConf","sysinfo() failed",0);
			exit(1);
		}
		if(structSysinfo.loads[1]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
		{
			logfileLine("CheckConf","structSysinfo.loads[1] larger than JOBQUEUE_MAXLOAD",0);
			mysql_free_result(res);
			mysql_close(&gMysql);
			return;
		}

		sscanf(field[0],"%u",&uContainer);
		sscanf(field[1],"%u",&guContainerOwner);
		sscanf(field[2],"%u",&guStatus);
		sprintf(cCommand,"/usr/bin/md5sum /etc/vz/conf/%u.conf",uContainer);
		if((fp=popen(cCommand,"r"))==NULL)
		{
			logfileLine("CheckConf",cCommand,0);
			continue;
		}
		fgets(cMD5Sum,63,fp);
		printf("%s",cMD5Sum);
		pclose(fp);
	}
	mysql_free_result(res);
	mysql_close(&gMysql);

}//void CheckConf(void)
