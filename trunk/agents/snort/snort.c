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
MYSQL gMysqlLocal;
unsigned guMysqlLocal=0;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};
char gcProgram[32]={""};
unsigned guNodeOwner=0;

//local protos
void logfileLine(const char *cFunction,const char *cLogline);
void ProcessBarnyard(unsigned uPriority);
void CreateGeoIPTable(void);
void CreateBlockedIPTable(void);
void ForkPostAddScript(char *cMsg);
unsigned CheckIP(const char *cIP,char *cReport);
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
				printf("usage: %s [--help] [--check-ip <ip dotted quad>]"
					" [--version] [--create-geoip] [--create-blockedip]\n",cArgv[0]);
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
			if(!strcmp(cArgv[i],"--check-ip"))
			{
				if(iArgc==i+2 && strchr(cArgv[i+1],'.'))
				{
					char cReport[256]={""};
					unsigned uRetVal=CheckIP(cArgv[i+1],cReport);
					if(!uRetVal)
						printf("%s %s\n",cArgv[i+1],cReport);
					else
						printf("%s no match\n",cArgv[i+1]);
					mysql_close(&gMysqlLocal);
					exit(uRetVal);
				}
				else
					printf("usage: %s [--help] [--check-ip <ip dotted quad>]"
						" [--version] [--create-geoip] [--create-blockedip]\n",cArgv[0]);
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

	ProcessBarnyard(1);
	ProcessBarnyard(2);//requires tGeoIP tables

	if(rmdir(cLockfile))
	{
		logfileLine("main","could not rmdir(cLockfile)");
		return(1);
	}
	logfileLine("main","end");
	return(0);
}//main()


void ProcessBarnyard(unsigned uPriority)
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

	MYSQL_RES *resLocal;
	MYSQL_ROW fieldLocal;
        mysql_init(&gMysqlLocal);
        if(!mysql_real_connect(&gMysqlLocal,NULL,gcLocalUser,gcLocalPasswd,gcLocalDb,0,NULL,0))
        {
		logfileLine("ProcessBarnyard","Could not connect to local db");
		return;
        }
	guMysqlLocal=1;

	//Check last 60 seconds event for priorty 1 events.
	//If any get the IP or IPs to block.
	if(uPriority==1)
		sprintf(gcQuery,"SELECT DISTINCT INET_NTOA(iphdr.ip_src),iphdr.ip_src FROM event,iphdr,signature"
			" WHERE event.cid=iphdr.cid"
			" AND event.signature=signature.sig_id"
			" AND iphdr.ip_src NOT IN (SELECT uBlockedIP FROM tBlockedIP)"
			" AND event.timestamp>(NOW()-61)"
			" AND signature.sig_priority=1 LIMIT 16");
	else
		sprintf(gcQuery,"SELECT DISTINCT INET_NTOA(iphdr.ip_src),iphdr.ip_src FROM event,iphdr,signature"
			" WHERE event.cid=iphdr.cid"
			" AND event.signature=signature.sig_id"
			" AND iphdr.ip_src NOT IN (SELECT uBlockedIP FROM tBlockedIP)"
			" AND event.timestamp>(NOW()-61)"
			" AND signature.sig_priority>1 LIMIT 16");
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
	unsigned uIP[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	while((fieldLocal=mysql_fetch_row(resLocal)))
	{
		if(uIndex>15) break;
		sprintf(cIP[uIndex],"%.15s",fieldLocal[0]);
		sscanf(fieldLocal[1],"%u",uIP+uIndex);
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
			unsigned uGeoIPCountryCode=0;
			sprintf(gcQuery,"SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=%u LIMIT 1",uIP[uIndex]);
			mysql_query(&gMysqlLocal,gcQuery);
			if(mysql_errno(&gMysqlLocal))
			{
				logfileLine("ProcessBarnyard-s1",mysql_error(&gMysqlLocal));
				mysql_free_result(res);
				mysql_close(&gMysqlLocal);
				mysql_close(&gMysql);
				return;
			}
        		resLocal=mysql_store_result(&gMysqlLocal);
			if((fieldLocal=mysql_fetch_row(resLocal)))
			{
				sscanf(fieldLocal[0],"%u",&uGeoIPCountryCode);
				//low priority events that are fom US IPs are ignored at this time
				if(uPriority>1 && uGeoIPCountryCode==33)
				{
					//logfileLine("ProcessBarnyard-continue",cIP[uIndex]);
					//logfileLine("ProcessBarnyard-continue",fieldLocal[0]);
					//sprintf(gcQuery,"%u",uIP[uIndex]);
					//logfileLine("ProcessBarnyard-continue",gcQuery);
					cIP[uIndex][0]=0;//mark as not to be counted
					continue;
				}
			}

			if(uPriority>1)
				sprintf(gcQuery,"INSERT INTO tJob"
					" SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					",cLabel='TestBlockAccess unxsSnort %u'"
					",cJobName='TestBlockAccess'"
					",uDatacenter=%u,uNode=%u"
					",cJobData='cIPv4=%.15s;\nuGeoIPCountryCode=%u;'"
					",uJobDate=UNIX_TIMESTAMP(NOW())"
					",uJobStatus=1",
						uIndex,
						uDatacenter,
						uNode,
						cIP[uIndex],uGeoIPCountryCode);
			else
				sprintf(gcQuery,"INSERT INTO tJob"
					" SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					",cLabel='BlockAccess unxsSnort %u'"
					",cJobName='BlockAccess'"
					",uDatacenter=%u,uNode=%u"
					",cJobData='cIPv4=%.15s;\nuGeoIPCountryCode=%u;'"
					",uJobDate=UNIX_TIMESTAMP(NOW())"
					",uJobStatus=1",
						uIndex,
						uDatacenter,
						uNode,
						cIP[uIndex],uGeoIPCountryCode);
			//debug only
			//printf("%s\n",gcQuery);
			//continue;
			//return;

			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessBarnyard-s2",mysql_error(&gMysql));
				mysql_close(&gMysql);
				return;
			}
			uCount++;
			sprintf(gcQuery,"INSERT INTO tBlockedIP SET uBlockedIP=INET_ATON('%s')",cIP[uIndex]);
			mysql_query(&gMysqlLocal,gcQuery);
			//debug only
			//if(mysql_errno(&gMysqlLocal))
			//	logfileLine("ProcessBarnyard",mysql_error(&gMysqlLocal));
		}//for each IP
	}//while for each server
	if(uCount)
	{
		sprintf(gcQuery,"Created %u tJob entries",uCount);
		logfileLine("ProcessBarnyard",gcQuery);
		char cMsg[1024]={""};
		unsigned uFirst=1;
		register unsigned uIndex2;
		//sprintf(cMsg,"%s %s %s %s %s %s %s %u",cIP[0],cIP[1],cIP[2],cIP[3],cIP[4],cIP[5],cIP[6],uIndex);
		//logfileLine("ProcessBarnyard-dbg1",cMsg);
		cMsg[0]=0;
		for(uIndex2=0;uIndex2<uIndex;uIndex2++)
		{
			if(!cIP[uIndex2][0]) continue;
			if(!uFirst)
				strncat(cMsg,",",1);
			strncat(cMsg,cIP[uIndex2],15);

			char cReport[64]={"-No country match"};
			if(!CheckIP(cIP[uIndex2],cReport))
				strncat(cMsg,"-",1);
			strncat(cMsg,cReport,64);

			//logfileLine("ProcessBarnyard-dbg2",cMsg);
			uFirst=0;
		}
		if(cMsg[strlen(cMsg)-1]==',') cMsg[strlen(cMsg)-1]=0;
		logfileLine("ProcessBarnyard",cMsg);
		ForkPostAddScript(cMsg);
	}
	//debug only
	//ForkPostAddScript("testing,123,testing,123");
	mysql_free_result(res);
	mysql_close(&gMysql);
	mysql_close(&gMysqlLocal);
	logfileLine("ProcessBarnyard","end");

}//void ProcessBarnyard(unsigned uPriority)


void CreateGeoIPTable(void)
{
	if((gLfp=fopen(cSNORTLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
		exit(1);
	}
		
        mysql_init(&gMysqlLocal);
        if(!mysql_real_connect(&gMysqlLocal,NULL,gcLocalUser,gcLocalPasswd,gcLocalDb,0,NULL,0))
        {
		logfileLine("CreateGeoIPTable","Could not connect to local db");
		return;
        }

	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tGeoIPCountryCode ("
			" uGeoIPCountryCode TINYINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,"
			" cCountryCode CHAR(2) NOT NULL,"
			" cCountryName VARCHAR(50) NOT NULL )");
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
		logfileLine("CreateGeoIPTable",mysql_error(&gMysqlLocal));

	logfileLine("CreateGeoIPTable","tGeoIPCountryCode Ok");

	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tGeoIP ("
			" uStartIP INT UNSIGNED NOT NULL,"
			" uEndIP INT UNSIGNED NOT NULL, UNIQUE (uEndIP),"
			" uGeoIPCountryCode TINYINT UNSIGNED NOT NULL )");
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
		logfileLine("CreateGeoIPTable",mysql_error(&gMysqlLocal));

	logfileLine("CreateGeoIPTable","tGeoIP Ok");

	mysql_close(&gMysqlLocal);

	printf("tGeoIPCountryCode and tGeoIP tables exist or have been created\n");

}//void CreateGeoIPTable(void)


void CreateBlockedIPTable(void)
{
	if((gLfp=fopen(cSNORTLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
		exit(1);
	}
	logfileLine("CreateBlockedIPTable","start");
		
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


void ForkPostAddScript(char *cMsg)
{
	pid_t pidChild;

	pidChild=fork();
	if(pidChild!=0)
		return;

	struct stat statInfo;
	char gcQuery[1024]={""};
	if(!stat("/usr/sbin/unxsSnortPostAddJob.sh",&statInfo))
	{
		if(statInfo.st_uid!=0)
		{
			logfileLine("ForkPostAddScript","script not owned by root");
			exit(0);
		}
		if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
		{
			logfileLine("ForkPostAddScript","script not chmod 500");
			exit(0);
		}
		sprintf(gcQuery,"/usr/sbin/unxsSnortPostAddJob.sh \"%.768s\"",cMsg);
		if(!system(gcQuery))
			logfileLine("ForkPostAddScript","ran ok");
		else
			logfileLine("ForkPostAddScript","ran with errors");
			
	}

	exit(0);

}//void ForkPostAddScript(char *cMsg)


unsigned CheckIP(const char *cIP,char *cReport)
{
	unsigned uRetVal=0;
	MYSQL_RES *resLocal;
	MYSQL_ROW fieldLocal;

	if(!guMysqlLocal)
	{
        	mysql_init(&gMysqlLocal);
        	if(!mysql_real_connect(&gMysqlLocal,NULL,gcLocalUser,gcLocalPasswd,gcLocalDb,0,NULL,0))
        	{
			return(1);
        	}
		guMysqlLocal=1;
	}

	sprintf(gcQuery,"SELECT cCountryCode,cCountryName,uGeoIPCountryCode"
			" FROM tGeoIPCountryCode"
			" WHERE uGeoIPCountryCode="
			"(SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=INET_ATON('%s') LIMIT 1)",cIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		return(1);
	}
        resLocal=mysql_store_result(&gMysqlLocal);
	if((fieldLocal=mysql_fetch_row(resLocal)))
		sprintf(cReport,"%.2s(%.10s) %.50s",fieldLocal[0],fieldLocal[2],fieldLocal[1]);
	else
		uRetVal=1;	
	mysql_free_result(resLocal);

	return(uRetVal);

}//unsigned CheckIP()
