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
void CreateGeoIPTable(void);
void CreateBlockedIPTable(void);
//external protos
void TextConnectDb(void);

#define gcLocalUser	"unxsvz"
#define gcLocalPasswd	"wsxedc"
#define gcLocalDb	"snort"
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
	sprintf(gcProgram,"%.31s",cArgv[0]);
	if(iArgc>1)
	{
		register int i;
		for(i=1;i<iArgc;i++)
		{
			if(!strcmp(cArgv[i],"--help"))
			{
				printf("usage: %s [--help] [--version] [--create-geoip] [--create-blockedip]\n",cArgv[0]);
				exit(0);
			}
			if(!strcmp(cArgv[i],"--version"))
			{
				printf("version: %s $Id$\n",cArgv[0]);
				exit(0);
			}
			if(!strcmp(cArgv[i],"--create-geoip"))
			{
				CreateGeoIPTable();
				exit(0);
			}
			if(!strcmp(cArgv[i],"--create-blockedip"))
			{
				CreateBlockedIPTable();
				exit(0);
			}
		}
	}

	char cLockfile[64]={"/tmp/snort.lock"};

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

	MYSQL gMysqlLocal;
	MYSQL_RES *resLocal;
	MYSQL_ROW fieldLocal;
        mysql_init(&gMysqlLocal);
        if(!mysql_real_connect(&gMysqlLocal,NULL,gcLocalUser,gcLocalPasswd,gcLocalDb,0,NULL,0))
        {
		logfileLine("ProcessBarnyard","Could not connect to local db");
		return;
        }

	//Check last 60 seconds event for priorty 1 events.
	//If any get the IP or IPs to block.
	sprintf(gcQuery,"SELECT DISTINCT INET_NTOA(iphdr.ip_src) FROM event,iphdr,signature"
			" WHERE event.cid=iphdr.cid"
			" AND event.signature=signature.sig_id"
			" AND iphdr.ip_src NOT IN (SELECT uBlockedIP FROM tBlockedIP)"
			" AND event.timestamp>(NOW()-600)"
			//debug only
			//" AND signature.sig_priority<3 LIMIT 16");
			" AND signature.sig_priority<2 LIMIT 16");
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("ProcessBarnyard",mysql_error(&gMysqlLocal));
		mysql_close(&gMysqlLocal);
		return;
	}
        resLocal=mysql_store_result(&gMysqlLocal);
	unsigned uIndex=0;
	char cIP[16][16]={"","","","","","","","","","","","","","","",""};
	while((fieldLocal=mysql_fetch_row(resLocal)))
	{
		if(uIndex>15) break;
		sprintf(cIP[uIndex],"%.15s",fieldLocal[0]);
		logfileLine("ProcessBarnyard",cIP[uIndex]);
		uIndex++;
	}
	mysql_free_result(resLocal);

	//Create a BlockAccess tJob for each active tNode
	sprintf(gcQuery,"SELECT tNode.uNode,tDatacenter.uDatacenter FROM tNode,tDatacenter"
			" WHERE tDatacenter.uStatus=1 AND tNode.uStatus=1"
			" AND tDatacenter.cLabel!='CustomerPremise'"
			" AND tNode.uDatacenter=tDatacenter.uDatacenter");
	//		debug only
	//		" AND tNode.uDatacenter=tDatacenter.uDatacenter LIMIT 1");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessBarnyard",mysql_error(&gMysql));
		mysql_close(&gMysql);
		return;
	}
        res=mysql_store_result(&gMysql);
	unsigned uCount=0;
	unsigned uNode=0;
	unsigned uDatacenter=0;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
		for(uIndex=0;uIndex<16 && cIP[uIndex][0];uIndex++)
		{
			sprintf(gcQuery,"INSERT INTO tJob"
			" SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
			",cLabel='BlockAccess unxsSnort %u'"
			",cJobName='BlockAccess'"
			",uDatacenter=%u,uNode=%u"
			",cJobData='cIPv4=%.15s;'"
			",uJobDate=UNIX_TIMESTAMP(NOW())"
			",uJobStatus=1",
					uIndex,
					uDatacenter,
					uNode,
					cIP[uIndex]);
			//debug only
			//printf("%s\n",gcQuery);
			//continue;
			//return;

			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessBarnyard",mysql_error(&gMysql));
				mysql_close(&gMysql);
				return;
			}
			uCount++;
			sprintf(gcQuery,"INSERT INTO tBlockedIP SET uBlockedIP=INET_ATON('%s')",cIP[uIndex]);
			mysql_query(&gMysqlLocal,gcQuery);
			//debug only
			//if(mysql_errno(&gMysqlLocal))
			//	logfileLine("ProcessBarnyard",mysql_error(&gMysqlLocal));
		}
	}
	if(uCount)
	{
		sprintf(gcQuery,"Created %u tJob entries",uCount);
		logfileLine("ProcessBarnyard",gcQuery);
	}
	mysql_free_result(res);
	mysql_close(&gMysql);
	mysql_close(&gMysqlLocal);
	logfileLine("ProcessBarnyard","end");

}//void ProcessBarnyard(void)


void CreateGeoIPTable(void)
{
	if((gLfp=fopen(cSNORTLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
		exit(1);
	}
		
	MYSQL gMysqlLocal;
        mysql_init(&gMysqlLocal);
        if(!mysql_real_connect(&gMysqlLocal,NULL,gcLocalUser,gcLocalPasswd,gcLocalDb,0,NULL,0))
        {
		logfileLine("CreateGeoIPTable","Could not connect to local db");
		return;
        }

	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tGeoIP( "
			"uGeoIP INT UNSIGNED PRIMARY KEY AUTO_INCREMENT");
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
		logfileLine("CreateGeoIPTable",mysql_error(&gMysqlLocal));
	if(mysql_affected_rows(&gMysqlLocal))
		logfileLine("CreateGeoIPTable","Ok");
	mysql_close(&gMysqlLocal);
}//void CreateGeoIPTable(void)


void CreateBlockedIPTable(void)
{
	if((gLfp=fopen(cSNORTLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
		exit(1);
	}
	logfileLine("CreateBlockedIPTable","start");
		
	MYSQL gMysqlLocal;
        mysql_init(&gMysqlLocal);
        if(!mysql_real_connect(&gMysqlLocal,NULL,gcLocalUser,gcLocalPasswd,gcLocalDb,0,NULL,0))
        {
		logfileLine("CreateBlockedIPTable","Could not connect to local db");
		return;
        }

	sprintf(gcQuery,"DROP TABLE tBlockedIP");
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("CreateBlockedIPTable","error");
		logfileLine("CreateBlockedIPTable",mysql_error(&gMysqlLocal));
		mysql_close(&gMysqlLocal);
		return;
	}

	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tBlockedIP ( "
			"uBlockedIP INT UNSIGNED PRIMARY KEY DEFAULT 0 ) ENGINE=MEMORY");
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("CreateBlockedIPTable","error");
		logfileLine("CreateBlockedIPTable",mysql_error(&gMysqlLocal));
		mysql_close(&gMysqlLocal);
		return;
	}

	logfileLine("CreateBlockedIPTable","Ok");
	mysql_close(&gMysqlLocal);
	logfileLine("CreateBlockedIPTable","end");

}//void CreateBlockedIPTable(void)
