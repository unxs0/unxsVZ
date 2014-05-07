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
char gcLockfile[64]={"/tmp/snort.lock"};

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
void ForkPostAddScript(const char *cMsg);
void ForkPostDelScript(const char *cMsg);
unsigned CheckIP(const char *cIP,char *cReport);
unsigned ReportIP(const char *cIP);
unsigned BlockIP(const char *cIP);
unsigned DumpBlocked(void);
unsigned RemoveBlocked(const char *cIP);
unsigned UnBlockIP(const char *cIP);
unsigned MarkBlockedVZ(const char *cIP);
unsigned MarkUnBlockedVZ(const char *cIP);
//external protos
void TextConnectDb(void);

#define gcLocalUser	"unxsvz"
#define gcLocalPasswd	"wsxedc"
#define gcLocalDb	"unxsvz"
unsigned TextLocalConnectDb(void);

unsigned guLogLevel=3;
static FILE *gLfp=NULL;
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

	if(gLfp==NULL)
		gLfp=stdout;
	fprintf(gLfp,"%s unxsSnort.%s[%u]: %s.\n",cTime,cFunction,pidThis,cLogline);
	fflush(gLfp);

}//void logfileLine()


#define LOCALDBIP0 "64.71.154.153"
unsigned TextLocalConnectDb(void)
{
	mysql_init(&gMysqlLocal);
	if(!mysql_real_connect(&gMysqlLocal,LOCALDBIP0,gcLocalUser,gcLocalPasswd,gcLocalDb,0,NULL,0))
	{
		logfileLine("TextLocalConnectDb","Could not connect to db\n");
		return(1);
	}
	guMysqlLocal=1;
	return(0);
}//void TextLocalConnectDb(void)


int main(int iArgc, char *cArgv[])
{

	//private main functions
	void PrintUsage(void)
	{
		printf("usage: %s\n"
			"\t[--help]\n"
			"\t[--process]\n"
			"\t[--check-ip <ip dotted quad>]\n"
			"\t[--report-ip <ip dotted quad>]\n"
			"\t[--block-ip <ip dotted quad>]\n"
			"\t[--unblock-ip <ip dotted quad>]\n"
			"\t[--dump-blocked]\n"
			"\t[--remove-from-blocked <ip dotted quad>]\n"
			"\t[--version] [--create-geoip]\n"
			"\t[--create-blockedip]\n",cArgv[0]);
		exit(0);//this does not follow normal design rules
	}//void PrintUsage(void)

	unsigned Process(void)
	{
		if((gLfp=fopen(cSNORTLOGFILE,"a"))==NULL)
		{
			fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
			return(1);
		}
		
		struct stat structStat;
		if(!stat(gcLockfile,&structStat))
		{
			logfileLine("main","waiting for rmdir(gcLockfile)");
			return(1);
		}
		if(mkdir(gcLockfile,S_IRUSR|S_IWUSR|S_IXUSR))
		{
			logfileLine("main","could not open gcLockfile dir");
			return(1);
		}

		//uPriority critically important!
		ProcessBarnyard(1);
		ProcessBarnyard(2);//requires tGeoIP tables
		ProcessBarnyard(3);//requires tGeoIP tables

		if(rmdir(gcLockfile))
		{
			logfileLine("main","could not rmdir(gcLockfile)");
			return(1);
		}
		logfileLine("main","end");
		return(0);
	}//unsigned Process(void)

	sprintf(gcProgram,"%.31s",cArgv[0]);
	if(iArgc>1)
	{
		register int i;
		for(i=1;i<iArgc;i++)
		{
			if(!strcmp(cArgv[i],"--process"))
			{
				return(Process());
			}
			if(!strcmp(cArgv[i],"--help"))
			{
				PrintUsage();
			}
			if(!strcmp(cArgv[i],"--version"))
			{
				printf("version: %s $Id$\n",cArgv[0]);
				return(0);
			}
			if(!strcmp(cArgv[i],"--report-ip") && iArgc==(i+2))
			{
				ReportIP(cArgv[i+1]);
				return(0);
			}
			if(!strcmp(cArgv[i],"--dump-blocked"))
			{
				DumpBlocked();
				return(0);
			}
			if(!strcmp(cArgv[i],"--block-ip") && iArgc==(i+2))
			{
				BlockIP(cArgv[i+1]);
				return(0);
			}
			if(!strcmp(cArgv[i],"--remove-from-blocked") && iArgc==(i+2))
			{
				return(RemoveBlocked(cArgv[i+1]));
			}
			if(!strcmp(cArgv[i],"--unblock-ip") && iArgc==(i+2))
			{
				UnBlockIP(cArgv[i+1]);
				return(0);
			}
			if(!strcmp(cArgv[i],"--create-geoip"))
			{
				CreateGeoIPTable();
				return(0);
			}
			if(!strcmp(cArgv[i],"--create-blockedip"))
			{
				CreateBlockedIPTable();
				return(0);
			}
			if(!strcmp(cArgv[i],"--check-ip") && iArgc==(i+2))
			{
				if(strchr(cArgv[i+1],'.'))
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
					PrintUsage();
			}
		}
	}

	PrintUsage();
	//should never reach here
	return(1);
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
	if(TextLocalConnectDb()) exit(1);

	//Check last 60 seconds event for priorty 1 events.
	//If any get the IP or IPs to block.
	sprintf(gcQuery,"SELECT DISTINCT INET_NTOA(iphdr.ip_src),iphdr.ip_src FROM event,iphdr,signature"
			" WHERE event.cid=iphdr.cid"
			" AND event.signature=signature.sig_id"
			" AND iphdr.ip_src NOT IN (SELECT uBlockedIP FROM tBlockedIP)"
			" AND event.timestamp>(NOW()-61)"
			" AND signature.sig_priority=%u LIMIT 16",uPriority);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("ProcessBarnyard",mysql_error(&gMysqlLocal));
		mysql_close(&gMysqlLocal);
		return;
	}
        resLocal=mysql_store_result(&gMysqlLocal);
	register unsigned uIndex=0;
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
		register unsigned uIndex2=0;
		for(uIndex2=0;uIndex2<uIndex;uIndex2++)
		{
			unsigned uGeoIPCountryCode=0;
			sprintf(gcQuery,"SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=%u LIMIT 1",uIP[uIndex2]);
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
					//logfileLine("ProcessBarnyard-continue",cIP[uIndex2]);
					//logfileLine("ProcessBarnyard-continue",fieldLocal[0]);
					//sprintf(gcQuery,"%u",uIP[uIndex2]);
					//logfileLine("ProcessBarnyard-continue",gcQuery);
					cIP[uIndex2][0]=0;//mark as not to be used for email below.
					continue;
				}
			}

			if(uPriority>2)
				sprintf(gcQuery,"INSERT INTO tJob"
					" SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					",cLabel='TestBlockAccess unxsSnort %u'"
					",cJobName='TestBlockAccess'"
					",uDatacenter=%u,uNode=%u"
					",cJobData='cIPv4=%.15s;\nuGeoIPCountryCode=%u;\nuPriority=%u;'"
					",uJobDate=UNIX_TIMESTAMP(NOW())"
					",uJobStatus=1",
						uIndex2,
						uDatacenter,
						uNode,
						cIP[uIndex2],uGeoIPCountryCode,uPriority);
			else
				sprintf(gcQuery,"INSERT INTO tJob"
					" SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
					",cLabel='BlockAccess unxsSnort %u'"
					",cJobName='BlockAccess'"
					",uDatacenter=%u,uNode=%u"
					",cJobData='cIPv4=%.15s;\nuGeoIPCountryCode=%u;\nuPriority=%u;'"
					",uJobDate=UNIX_TIMESTAMP(NOW())"
					",uJobStatus=1",
						uIndex2,
						uDatacenter,
						uNode,
						cIP[uIndex2],uGeoIPCountryCode,uPriority);
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
			sprintf(gcQuery,"INSERT INTO tBlockedIP SET uBlockedIP=INET_ATON('%s')",cIP[uIndex2]);
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
		if(uPriority>2)
			sprintf(cMsg,"TESTMODE:");
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
	

	if(TextLocalConnectDb()) exit(1);

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
		
	if(TextLocalConnectDb()) exit(1);

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
			"uBlockedIP INT UNSIGNED PRIMARY KEY DEFAULT 0 )");
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


void ForkPostAddScript(const char *cMsg)
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


void ForkPostUndoScript(const char *cMsg)
{
	pid_t pidChild;

	pidChild=fork();
	if(pidChild!=0)
		return;

	struct stat statInfo;
	char gcQuery[1024]={""};
	if(!stat("/usr/sbin/unxsSnortPostUndoJob.sh",&statInfo))
	{
		if(statInfo.st_uid!=0)
		{
			logfileLine("ForkPostUndoScript","script not owned by root");
			exit(0);
		}
		if(statInfo.st_mode & ( S_IWOTH | S_IWGRP | S_IWUSR | S_IXOTH | S_IROTH | S_IXGRP | S_IRGRP ) )
		{
			logfileLine("ForkPostUndoScript","script not chmod 500");
			exit(0);
		}
		sprintf(gcQuery,"/usr/sbin/unxsSnortPostUndoJob.sh \"%.768s\"",cMsg);
		if(!system(gcQuery))
			logfileLine("ForkPostUndoScript","ran ok");
		else
			logfileLine("ForkPostUndoScript","ran with errors");
			
	}

	exit(0);

}//void ForkPostUndoScript(char *cMsg)


unsigned CheckIP(const char *cIP,char *cReport)
{
	unsigned uRetVal=0;
	MYSQL_RES *resLocal;
	MYSQL_ROW fieldLocal;

	if(!guMysqlLocal)
		if(TextLocalConnectDb()) return(1);

	sprintf(gcQuery,"SELECT cCountryCode,cCountryName,uGeoIPCountryCode"
			" FROM tGeoIPCountryCode"
			" WHERE uGeoIPCountryCode="
			"(SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=INET_ATON('%s') LIMIT 1)",cIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("CheckIP","SQL error");
		logfileLine("CheckIP",mysql_error(&gMysqlLocal));
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


unsigned ReportIP(const char *cIP)
{
	unsigned uRetVal=0;
	MYSQL_RES *resLocal;
	MYSQL_ROW fieldLocal;

	if(!guMysqlLocal)
		if(TextLocalConnectDb()) return(1);

	sprintf(gcQuery,"SELECT cCountryCode,cCountryName,uGeoIPCountryCode"
			" FROM tGeoIPCountryCode"
			" WHERE uGeoIPCountryCode="
			"(SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=INET_ATON('%s') LIMIT 1)",cIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
		return(1);
        resLocal=mysql_store_result(&gMysqlLocal);
	if((fieldLocal=mysql_fetch_row(resLocal)))
		printf("%s %.2s(%.10s) %.50s\n",cIP,fieldLocal[0],fieldLocal[2],fieldLocal[1]);
	else
		return(uRetVal++);	

	//list event types
	printf("\n24 hour type of events\n");
	sprintf(gcQuery,"SELECT DISTINCT signature.sig_name,signature.sig_priority"
			" FROM event,iphdr,signature"
			" WHERE event.cid=iphdr.cid AND event.signature=signature.sig_id AND event.timestamp>(NOW()-86400)"
			" AND iphdr.ip_src=INET_ATON('%s') ORDER BY event.timestamp DESC",cIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
		return(1);
        resLocal=mysql_store_result(&gMysqlLocal);
	unsigned uCount=0;
	while((fieldLocal=mysql_fetch_row(resLocal)))
	{
		uCount++;
		printf("%s\t%s\n",fieldLocal[0],fieldLocal[1]);
	}
	printf("uCount=%u\n",uCount);

	//24 hour report of all events for given IP
	printf("\n24 hour report of all events\n");
	sprintf(gcQuery,"SELECT signature.sig_name,event.timestamp"
			" FROM event,iphdr,signature"
			" WHERE event.cid=iphdr.cid AND event.signature=signature.sig_id AND event.timestamp>(NOW()-86400)"
			" AND iphdr.ip_src=INET_ATON('%s') ORDER BY event.timestamp DESC",cIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
		return(1);
        resLocal=mysql_store_result(&gMysqlLocal);
	uCount=0;
	while((fieldLocal=mysql_fetch_row(resLocal)))
	{
		uCount++;
		printf("%s\t%s\n",fieldLocal[0],fieldLocal[1]);
	}
	printf("uCount=%u\n",uCount);

	mysql_free_result(resLocal);
	return(uRetVal);

}//unsigned ReportIP()


unsigned BlockIP(const char *cIP)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	MYSQL_RES *resLocal;
	MYSQL_ROW fieldLocal;

	if(!guMysqlLocal)
		if(TextLocalConnectDb()) return(1);

	sprintf(gcQuery,"SELECT cCountryCode,cCountryName,uGeoIPCountryCode"
			" FROM tGeoIPCountryCode"
			" WHERE uGeoIPCountryCode="
			"(SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=INET_ATON('%s') LIMIT 1)",cIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
		return(1);
        resLocal=mysql_store_result(&gMysqlLocal);
	char cCountryInfo[100]={""};
	if((fieldLocal=mysql_fetch_row(resLocal)))
		sprintf(cCountryInfo,"%s %.2s(%.10s) %.50s",cIP,fieldLocal[0],fieldLocal[2],fieldLocal[1]);
	else
		sprintf(cCountryInfo,"%s No country info available\n",cIP);


	//Uses login data from local.h
	TextConnectDb();
	guLoginClient=1;//Root user

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
		printf("%s",mysql_error(&gMysql));
		return(1);
	}
        res=mysql_store_result(&gMysql);
	unsigned uNode=0;
	unsigned uCount=0;
	unsigned uDatacenter=0;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
		sprintf(gcQuery,"INSERT INTO tJob"
				" SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
				",cLabel='BlockAccess --block-ip'"
				",cJobName='BlockAccess'"
				",uDatacenter=%u,uNode=%u"
				",cJobData='cIPv4=%.15s;\ncCountryInfo=%s;'"
				",uJobDate=UNIX_TIMESTAMP(NOW())"
				",uJobStatus=1",
					uDatacenter,
					uNode,
					cIP,cCountryInfo);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s",mysql_error(&gMysql));
			return(1);
		}

		uCount++;

		sprintf(gcQuery,"INSERT INTO tBlockedIP SET uBlockedIP=INET_ATON('%s')",cIP);
		MarkBlockedVZ(cIP);
		mysql_query(&gMysqlLocal,gcQuery);

	}//while each server

	if((gLfp=fopen(cSNORTLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
		return(1);
	}
	if(uCount && cCountryInfo[0])
	{
		ForkPostAddScript(cCountryInfo);
	}
	logfileLine("BlockIP",cCountryInfo);

	mysql_free_result(res);
	mysql_close(&gMysql);
	mysql_close(&gMysqlLocal);

	return(0);

}//unsigned BlockIP()


unsigned UnBlockIP(const char *cIP)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	//Uses login data from local.h
	TextConnectDb();
	guLoginClient=1;//Root user

	if(TextLocalConnectDb()) return(1);

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
		printf("%s",mysql_error(&gMysql));
		return(1);
	}
        res=mysql_store_result(&gMysql);
	unsigned uNode=0;
	unsigned uCount=0;
	unsigned uDatacenter=0;
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
		sprintf(gcQuery,"INSERT INTO tJob"
				" SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
				",cLabel='UndoBlockAccess --unblock-ip'"
				",cJobName='UndoBlockAccess'"
				",uDatacenter=%u,uNode=%u"
				",cJobData='cIPv4=%.15s;'"
				",uJobDate=UNIX_TIMESTAMP(NOW())"
				",uJobStatus=1",
					uDatacenter,
					uNode,
					cIP);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s",mysql_error(&gMysql));
			return(1);
		}

		uCount++;

		sprintf(gcQuery,"DELETE FROM tBlockedIP WHERE uBlockedIP=INET_ATON('%s')",cIP);
		mysql_query(&gMysqlLocal,gcQuery);
		MarkUnBlockedVZ(cIP);

	}//while each server

	if((gLfp=fopen(cSNORTLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
		return(1);
	}
	logfileLine("UnBlockIP",cIP);
	if(uCount && cIP[0])
		ForkPostUndoScript(cIP);

	mysql_free_result(res);
	mysql_close(&gMysql);
	mysql_close(&gMysqlLocal);

	return(0);
}//unsigned UnBlockIP()


unsigned DumpBlocked(void)
{
	unsigned uRetVal=0;
	MYSQL_RES *resLocal;
	MYSQL_ROW fieldLocal;
	MYSQL_RES *resLocal2;
	MYSQL_ROW fieldLocal2;

	if(!guMysqlLocal)
		if(TextLocalConnectDb()) return(1);

	sprintf(gcQuery,"SELECT INET_NTOA(uBlockedIP),uBlockedIP FROM tBlockedIP");
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
		return(1);
        resLocal=mysql_store_result(&gMysqlLocal);
	unsigned uCount=0;
	while((fieldLocal=mysql_fetch_row(resLocal)))
	{
		sprintf(gcQuery,"SELECT cCountryCode,cCountryName,uGeoIPCountryCode"
			" FROM tGeoIPCountryCode"
			" WHERE uGeoIPCountryCode="
			"(SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=%s LIMIT 1)",fieldLocal[1]);
		mysql_query(&gMysqlLocal,gcQuery);
		if(mysql_errno(&gMysqlLocal))
			return(1);
        	resLocal2=mysql_store_result(&gMysqlLocal);
		if((fieldLocal2=mysql_fetch_row(resLocal2)))
		{
			printf("%s %s(%s) %s\n",fieldLocal[0],fieldLocal2[0],fieldLocal2[2],fieldLocal2[1]);
			uCount++;
		}
		else
		{
			printf("%s Country info not available\n",fieldLocal[0]);
			uCount++;
		}
	}

	printf("uCount=%u\n",uCount);

	mysql_free_result(resLocal2);
	mysql_free_result(resLocal);
	return(uRetVal);

}//unsigned DumpBlocked(void)


unsigned RemoveBlocked(const char *cIP)
{
	unsigned uRetVal=0;

	DumpBlocked();

	if(!guMysqlLocal)
		if(TextLocalConnectDb()) return(1);

	sprintf(gcQuery,"DELETE FROM tBlockedIP WHERE uBlockedIP=INET_ATON('%s')",cIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		printf("%s\n",mysql_error(&gMysqlLocal));
		return(3);
	}

	DumpBlocked();

	return(uRetVal);

}//unsigned RemoveBlocked(chatr *cIP)


unsigned MarkBlockedVZ(const char *cIP)
{
	//gMysql must be ready
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uIP FROM tIP WHERE"
				" cLabel='%s'"
				" AND uDatacenter=41"//CustomerPremise magic number fix ASAP
				" AND cComment='unxsSnort blocked'"
				" AND uAvailable=0 LIMIT 1",cIP);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("MarkBlockedVZ1",mysql_error(&gMysql));
		return(1);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"UPDATE tIP"
				" SET uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE uIP=%s",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("MarkBlockedVZ2",mysql_error(&gMysql));
			mysql_free_result(res);
			return(1);
		}
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tIP"
				" SET cLabel='%s'"
				",uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
				",cComment='unxsSnort blocked'"
				",uDatacenter=41"//CustomerPremise magic number fix ASAP
				",uAvailable=0",
							cIP);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("MarkBlockedVZ3",mysql_error(&gMysql));
			mysql_free_result(res);
			return(1);
		}
	}
	mysql_free_result(res);
	return(0);
}//unsigned MarkBlockedVZ(char *cIP)


unsigned MarkUnBlockedVZ(const char *cIP)
{
	//gMysql must be ready
	sprintf(gcQuery,"DELETE FROM tIP WHERE"
				" cLabel='%s'"
				" AND uDatacenter=41"//CustomerPremise magic number fix ASAP
				" AND cComment='unxsSnort blocked'"
				" AND uAvailable=0 LIMIT 1",cIP);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("MarkUnBlockedVZ",mysql_error(&gMysql));
		return(1);
	}

	return(0);
}//unsigned MarkUnBlockedVZ(char *cIP)
