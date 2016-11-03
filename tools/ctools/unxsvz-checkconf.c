/*
FILE
	unxsvz-checkconf.c
	svn ID removed
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
void GetNodeHostnameFromContainer(unsigned uContainer,char *cHost);

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

	if(getuid()>1)
		return(0);
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


//We run this on nodes with clone containers.
//It compares the md5sum hash of the clone container
//against the ssh remote gathered md5sum of the clone
//container uSource container.
//Only reporting those that do not match.
void CheckConf(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uDatacenter=0;
	unsigned uNode=0;
	unsigned uContainer=0;
	unsigned uSource=0;
	FILE *fp;
	char cCommand[128];
	char cLocalMD5Sum[64];
	char cRemoteMD5Sum[64];

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
	sprintf(gcQuery,"SELECT uContainer,uSource FROM tContainer WHERE uNode=%u"
				" AND uDatacenter=%u"
				" AND uSource>0"
				" AND (uStatus=1"//Active OR
				" OR uStatus=31)"//Stopped
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
		sscanf(field[1],"%u",&uSource);

		//local first
		sprintf(cCommand,"cat /etc/vz/conf/%u.conf|grep -v IP_ADDRESS|grep -v ONBOOT|grep -v NAME|/usr/bin/md5sum",
					uContainer);
		if((fp=popen(cCommand,"r"))==NULL)
		{
			logfileLine("CheckConf",cCommand,0);
			pclose(fp);
			continue;
		}
		fgets(cLocalMD5Sum,33,fp);
		pclose(fp);

		//remote
		char cHost[100]={""};
		GetNodeHostnameFromContainer(uSource,cHost);
		sprintf(cCommand,"/usr/bin/ssh -c arcfour %s "
				"\"cat /etc/vz/conf/%u.conf|grep -v IP_ADDRESS|grep -v ONBOOT|grep -v NAME|/usr/bin/md5sum\"",
					cHost,uSource);
		if((fp=popen(cCommand,"r"))==NULL)
		{
			logfileLine("CheckConf",cCommand,0);
			pclose(fp);
			continue;
		}
		fgets(cRemoteMD5Sum,33,fp);

		if(strcmp(cLocalMD5Sum,cRemoteMD5Sum))
			printf("/etc/vz/conf/ file mismatch for %u %s %u %s\n",uContainer,cLocalMD5Sum,uSource,cRemoteMD5Sum);
	}
	mysql_free_result(res);
	mysql_close(&gMysql);

}//void CheckConf(void)


void GetNodeHostnameFromContainer(unsigned uContainer,char *cHost)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tNode.cLabel FROM tNode,tContainer WHERE tContainer.uContainer=%u"
				" AND tNode.uNode=tContainer.uNode",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("GetNodeHostnameFromContainer",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sprintf(cHost,"%.99s",field[0]);
	mysql_free_result(res);

}//void GetNodeHostname(unsigned uSource,char *cHost)
