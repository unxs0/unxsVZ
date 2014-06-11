/*
FILE
	snort.c
	$Id$
PURPOSE
	Agent to run on Snort IDS servers usually via barnyard2.
	E.g. Create tJob entries for certain events for all active nodes.

AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2008-2014.
	GPLv2 License applies. See LICENSE file.
NOTES
	This system post processes barnyard2 snort data then deletes it.
	So you can't really use it for webconsole stuff.

	It uses the unxsVZ backend to keep track of IP number status.

	It uses the unxsVZ backend jobqueue to create jobs for hardware node iptables.

	Little by little I am fleshing out a system that allows for sharing reputation
	data for attacker IPs across multiple datacenters that unxsVZ runs on.

	The most interesting post or meta processing of snort alerts is to increase the
	prioroty level based on multiple destination attacks from the same IP over time.

	It seems to be very safe at this time to block IPs that violate snort rules
	on multiple containers and more so if on multiple datacenters.

	If a customer has somehting poorly configured that exhibits this behavior
	then it is probably ok to block him and thus force the issue to be resolved.
*/

#include "snort.h"
#include "local.h"
#include <sys/sysinfo.h>

//unxsVZ tIP FW codes
#define uFWWAITINGBLOCK 1
#define uFWWAITINGACCESS 2
#define uFWBLOCKED 3
#define uFWACCESS 4
#define uFWREMOVED 5
#define uFWWAITINGREMOVAL 6
#define uFWWHITELISTED 7


#define cSNORTLOGFILE "/var/log/unxsSnortLog"

MYSQL gMysql;
MYSQL gMysqlLocal;
unsigned guMysqlLocal=0;
char gcQuery[8192]={""};
char cHostname[100]={""};
char gcProgram[32]={""};
unsigned guNodeOwner=0;

//local protos
void logfileLine(const char *cFunction,const char *cLogline);
void CreateGeoIPTable(void);
void CreateIPHistoryTable(void);
void ForkPostAddScript(const char *cMsg,const char *cIP);
void ForkPostDelScript(const char *cMsg);
unsigned CheckIP(const char *cIP,char *cReport,unsigned uIP);
unsigned ReportIP(const char *cIP, FILE *fp);
unsigned BlockIP(const char *cIP);
unsigned UnBlockIP(const char *cIP);
unsigned UpdateVZIP(const char *cIP,unsigned uIPNum,unsigned uFWStatus,
		unsigned uFWRule,unsigned uCountryCode,unsigned uDstIPCount);
unsigned uGetLastSignatureID(unsigned uIP);
void ProcessBarnyard2(unsigned uPriority);
unsigned uGetCountryCode(const char *cIP,char *cCountryCode);
void Cleanup(void);
unsigned UpdateVZIPProperty(unsigned uIP,unsigned uIPNum,unsigned uFWStatus,unsigned uFWRule,unsigned uDstIPCount);
//external protos
void TextConnectDb(void);

#define gcLocalUser	"unxsvz"
#define gcLocalPasswd	"wsxedc"
#define gcLocalDb	"snort"
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


#define LOCALDBIP0 NULL
unsigned TextLocalConnectDb(void)
{
	mysql_init(&gMysqlLocal);
	if(!mysql_real_connect(&gMysqlLocal,LOCALDBIP0,gcLocalUser,gcLocalPasswd,gcLocalDb,0,NULL,0))
	{
		logfileLine("TextLocalConnectDb","Could not connect to db");
		return(1);
	}
	guMysqlLocal=1;
	return(0);
}//void TextLocalConnectDb(void)


void TruncateSnort(void)
{
		//truncate tables
		//we need to save reputation info in our own table
		sprintf(gcQuery,"TRUNCATE event");
		mysql_query(&gMysqlLocal,gcQuery);
		if(mysql_errno(&gMysqlLocal))
			logfileLine("ProcessBarnyard2-t0",mysql_error(&gMysqlLocal));
		sprintf(gcQuery,"TRUNCATE data");
		mysql_query(&gMysqlLocal,gcQuery);
		if(mysql_errno(&gMysqlLocal))
			logfileLine("ProcessBarnyard2-t1",mysql_error(&gMysqlLocal));
		sprintf(gcQuery,"TRUNCATE iphdr");
		mysql_query(&gMysqlLocal,gcQuery);
		if(mysql_errno(&gMysqlLocal))
			logfileLine("ProcessBarnyard2-t2",mysql_error(&gMysqlLocal));
		sprintf(gcQuery,"TRUNCATE udphdr");
		mysql_query(&gMysqlLocal,gcQuery);
		if(mysql_errno(&gMysqlLocal))
			logfileLine("ProcessBarnyard2-t3",mysql_error(&gMysqlLocal));
}//void TruncateSnort(void)


int main(int iArgc, char *cArgv[])
{

	//private main functions
	char gcLockfile[64]={cLOCKDIR};
	void AcquireLock()
	{	
		struct stat structStat;
		if(!stat(gcLockfile,&structStat))
		{
			logfileLine("main","waiting for rmdir(gcLockfile)");
			exit(1);
		}
		if(mkdir(gcLockfile,S_IRUSR|S_IWUSR|S_IXUSR))
		{
			logfileLine("main","could not open gcLockfile dir");
			exit(1);
		}
	}//void AcquireLock()

	void ReleaseLock()
	{
		if(rmdir(gcLockfile))
		{
			logfileLine("main","could not rmdir(gcLockfile)");
			exit(1);
		}
	}//void ReleaseLock()

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
			"\t[--cleanup]\n"
			"\t[--truncate-snort]\n"
			"\t[--remove-from-blocked <ip dotted quad>]\n"
			"\t[--version] [--create-geoip]\n"
			"\t[--create-iphistory]\n",cArgv[0]);
		exit(0);//this does not follow normal design rules
	}//void PrintUsage(void)

	unsigned Process(void)
	{
		if((gLfp=fopen(cSNORTLOGFILE,"a"))==NULL)
		{
			fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
			return(1);
		}
	

		AcquireLock();

		if(gethostname(cHostname,99)!=0)
		{
			logfileLine("ProcessBarnyard2","gethostname() failed");
			exit(1);
		}
		TextConnectDb();
		if(TextLocalConnectDb()) exit(1);
		//uPriority critically important!
		ProcessBarnyard2(1);
		ProcessBarnyard2(2);
		ProcessBarnyard2(3);

		TruncateSnort();


		mysql_close(&gMysql);
		mysql_close(&gMysqlLocal);

		ReleaseLock();

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
			if(!strcmp(cArgv[i],"--truncate-snort"))
			{
				if(TextLocalConnectDb()) exit(1);
				TruncateSnort();
				mysql_close(&gMysqlLocal);
				return(0);
			}
			if(!strcmp(cArgv[i],"--cleanup"))
			{
				AcquireLock();
				Cleanup();
				ReleaseLock();
				return(0);
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
				ReportIP(cArgv[i+1],stdout);
				return(0);
			}
			if(!strcmp(cArgv[i],"--block-ip") && iArgc==(i+2))
			{
				BlockIP(cArgv[i+1]);
				return(0);
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
			if(!strcmp(cArgv[i],"--create-iphistory"))
			{
				CreateIPHistoryTable();
				return(0);
			}
			if(!strcmp(cArgv[i],"--check-ip") && iArgc==(i+2))
			{
				if(strchr(cArgv[i+1],'.'))
				{
					char cReport[256]={""};
					unsigned uRetVal=CheckIP(cArgv[i+1],cReport,0);
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


void ForkPostAddScript(const char *cMsg,const char *cIP)
{
	pid_t pidChild;

	pidChild=fork();
	if(pidChild!=0)
		return;

	char cFile[100]={""};
	FILE *fp;
	sprintf(cFile,"/tmp/%.15s.unxsSnort.report",cIP);
	if((fp=fopen(cFile,"w"))!=NULL)
	{
		guMysqlLocal=0;
		if(ReportIP(cIP,fp))
			logfileLine("ForkPostAddScript","ReportIP() error");
		fclose(fp);
	}
	else
	{
		logfileLine("ForkPostAddScript","could not open file");
		logfileLine("ForkPostAddScript",cFile);
	}

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
		sprintf(gcQuery,"/usr/sbin/unxsSnortPostAddJob.sh \"%.768s\" %s",cMsg,cFile);
		if(!system(gcQuery))
			logfileLine("ForkPostAddScript","ran ok");
		else
			logfileLine("ForkPostAddScript","ran with errors");
			
	}

	exit(0);

}//void ForkPostAddScript()


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


unsigned CheckIP(const char *cIP,char *cReport,unsigned uIP)
{
	unsigned uRetVal=0;
	MYSQL_RES *resLocal;
	MYSQL_ROW fieldLocal;

	if(!guMysqlLocal)
		if(TextLocalConnectDb()) return(1);

	if(!uIP && cIP[0])
		sprintf(gcQuery,"SELECT cCountryCode,cCountryName,uGeoIPCountryCode"
			" FROM tGeoIPCountryCode"
			" WHERE uGeoIPCountryCode="
			"(SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=INET_ATON('%s') LIMIT 1)",cIP);
	else
		sprintf(gcQuery,"SELECT cCountryCode,cCountryName,uGeoIPCountryCode"
			" FROM tGeoIPCountryCode"
			" WHERE uGeoIPCountryCode="
			"(SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=%u LIMIT 1)",uIP);
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


unsigned ReportIP(const char *cIP, FILE *fp)
{
	unsigned uRetVal=0;
	MYSQL_RES *resLocal;
	MYSQL_ROW fieldLocal;

	if(!guMysqlLocal)
		if(TextLocalConnectDb()) return(1);

	//country info
	sprintf(gcQuery,"SELECT cCountryCode,cCountryName,uGeoIPCountryCode"
			" FROM tGeoIPCountryCode"
			" WHERE uGeoIPCountryCode="
			"(SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=INET_ATON('%s') LIMIT 1)",cIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("ReportIP",mysql_error(&gMysqlLocal));
		return(1);
	}
        resLocal=mysql_store_result(&gMysqlLocal);
	if((fieldLocal=mysql_fetch_row(resLocal)))
		fprintf(fp,"%s %s(%s) %s\n\n",cIP,fieldLocal[0],fieldLocal[2],fieldLocal[1]);

#define EXPERIMENTAL "yes"
#ifdef EXPERIMENTAL
	//list event types
	fprintf(fp,"24hr: event name, priority, count\n");
	sprintf(gcQuery,"SELECT signature.sig_name,signature.sig_priority,COUNT(signature.sig_name)"
			" FROM tIPHistory,signature"
			" WHERE tIPHistory.uSignature=signature.sig_id AND  tIPHistory.uCreatedDate>(UNIX_TIMESTAMP(NOW())-86400)"
			" AND tIPHistory.uIPSrc=INET_ATON('%s') GROUP BY signature.sig_name ORDER BY tIPHistory.uCreatedDate DESC",cIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("ReportIP",mysql_error(&gMysqlLocal));
		return(1);
	}
        resLocal=mysql_store_result(&gMysqlLocal);
	unsigned uCount=0;
	while((fieldLocal=mysql_fetch_row(resLocal)))
	{
		uCount++;
		fprintf(fp,"%s,%s,%s\n",fieldLocal[0],fieldLocal[1],fieldLocal[2]);
	}
	fprintf(fp,"uCount=%u\n",uCount);

	//24 hour report of all events by dst ip 
	fprintf(fp,"\n24hr: event dst IP, count\n");
	sprintf(gcQuery,"SELECT INET_NTOA(tIPHistory.uIPDst),COUNT(tIPHistory.uIPDst)"
			" FROM tIPHistory"
			" WHERE tIPHistory.uCreatedDate>(UNIX_TIMESTAMP(NOW())-86400)"
			" AND tIPHistory.uIPSrc=INET_ATON('%s') GROUP BY tIPHistory.uIPDst ORDER BY tIPHistory.uCreatedDate DESC",cIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("ReportIP",mysql_error(&gMysqlLocal));
		return(1);
	}
        resLocal=mysql_store_result(&gMysqlLocal);
	uCount=0;
	while((fieldLocal=mysql_fetch_row(resLocal)))
	{
		uCount++;
		fprintf(fp,"%s,%s\n",fieldLocal[0],fieldLocal[1]);
	}
	fprintf(fp,"uCount=%u\n",uCount);

	//24 hour report of all events for given IP
	fprintf(fp,"\n24hr: timestamp, event, dst IP\n");
	sprintf(gcQuery,"SELECT FROM_UNIXTIME(tIPHistory.uCreatedDate),signature.sig_name,INET_NTOA(tIPHistory.uIPDst)"
			" FROM tIPHistory,signature"
			" WHERE tIPHistory.uSignature=signature.sig_id AND tIPHistory.uCreatedDate>(UNIX_TIMESTAMP(NOW())-86400)"
			" AND tIPHistory.uIPSrc=INET_ATON('%s') ORDER BY tIPHistory.uCreatedDate DESC",cIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("ReportIP",mysql_error(&gMysqlLocal));
		return(1);
	}
        resLocal=mysql_store_result(&gMysqlLocal);
	uCount=0;
	while((fieldLocal=mysql_fetch_row(resLocal)))
	{
		uCount++;
		fprintf(fp,"%s,%s,%s\n",fieldLocal[0],fieldLocal[1],fieldLocal[2]);
	}
	fprintf(fp,"uCount=%u\n",uCount);
#endif
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
	char cCountryCode[3]={"--"};
	unsigned uCountryCode=0;
	if((fieldLocal=mysql_fetch_row(resLocal)))
	{
		sprintf(cCountryInfo,"%s %.2s(%.10s) %.50s",cIP,fieldLocal[0],fieldLocal[2],fieldLocal[1]);
		sprintf(cCountryCode,"%.2s",fieldLocal[0]);
		sscanf(fieldLocal[2],"%u",&uCountryCode);
	}
	else
	{
		sprintf(cCountryInfo,"%s No country info available\n",cIP);
	}


	//Uses login data from local.h
	TextConnectDb();

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

	}//while each server
	if(uCount)
		UpdateVZIP(cIP,0,uFWWAITINGBLOCK,0,uCountryCode,0);

	if((gLfp=fopen(cSNORTLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
		return(1);
	}
	if(uCount && cCountryInfo[0])
	{
		ForkPostAddScript(cCountryInfo,cIP);
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

	}//while each server

	if(uCount)
	{
		char cCountryCode[3]={"--"};
		unsigned uCountryCode=0;
		uCountryCode=uGetCountryCode(cIP,cCountryCode);
		UpdateVZIP(cIP,0,uFWWAITINGACCESS,0,uCountryCode,0);
	}


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


unsigned UpdateVZIP(const char *cIP,unsigned uIPNum,unsigned uFWStatus,
			unsigned uFWRule,unsigned uCountryCode,unsigned uDstIPCount)
{
	//gMysql must be ready
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uIP=0;

	if(uIPNum)
		sprintf(gcQuery,"SELECT uIP FROM tIP WHERE"
				" uIPNum=%u"
				" AND uDatacenter=41"//CustomerPremise magic number fix ASAP
				" AND uAvailable=0 LIMIT 1",uIPNum);
	else if(cIP[0])
		sprintf(gcQuery,"SELECT uIP FROM tIP WHERE"
				" uIPNum=INET_ATON('%s')"
				" AND uDatacenter=41"//CustomerPremise magic number fix ASAP
				" AND uAvailable=0 LIMIT 1",cIP);
	else
		return(2);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("UpdateVZIP-1",mysql_error(&gMysql));
		return(1);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uIP);
		sprintf(gcQuery,"UPDATE tIP"
				" SET uModBy=2,uModDate=UNIX_TIMESTAMP(NOW()),"
				" uFWStatus=%u,uFWRule=%u,uCountryCode=%u"
				" WHERE uIP=%u",uFWStatus,uFWRule,uCountryCode,uIP);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("UpdateVZIP-2",mysql_error(&gMysql));
			mysql_free_result(res);
			return(1);
		}
		logfileLine("UpdateVZIP","UPDATE");
	}
	else
	{
		if(uIPNum)
			sprintf(gcQuery,"INSERT INTO tIP"
				" SET uIPNum=%u,cLabel=INET_NTOA(%u)"
				",uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
				",uModBy=2,uModDate=UNIX_TIMESTAMP(NOW())"
				",uFWStatus=%u,uFWRule=%u,uCountryCode=%u"
				",uDatacenter=41"//CustomerPremise magic number fix ASAP
				",uAvailable=0",uIPNum,uIPNum,uFWStatus,uFWRule,uCountryCode);
		else if(cIP[0])
			sprintf(gcQuery,"INSERT INTO tIP"
				" SET uIPNum=INET_ATON('%s'),cLabel='%s'"
				",uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
				",uModBy=2,uModDate=UNIX_TIMESTAMP(NOW())"
				",uFWStatus=%u,uFWRule=%u,uCountryCode=%u"
				",uDatacenter=41"//CustomerPremise magic number fix ASAP
				",uAvailable=0",cIP,cIP,uFWStatus,uFWRule,uCountryCode);
		else
			return(3);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("UpdateVZIP-3",mysql_error(&gMysql));
			mysql_free_result(res);
			return(1);
		}
		uIP=mysql_insert_id(&gMysql);
	}
	mysql_free_result(res);

	if(uIP)
		UpdateVZIPProperty(uIP,0,uFWStatus,uFWRule,uDstIPCount);

	return(0);
}//unsigned UpdateVZIP()


unsigned uGetLastSignatureID(unsigned uIP)
{
	//snort connection
	MYSQL_RES *resLocal=NULL;
	MYSQL_ROW fieldLocal;

	sprintf(gcQuery,"SELECT signature.sig_sid"
			" FROM tIPHistory,signature"
			" WHERE tIPHistory.uSignature=signature.sig_id"
			" AND tIPHistory.uIPSrc=%u ORDER BY tIPHistory.uCreatedDate DESC LIMIT 1",uIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("uGetLastSignatureID",mysql_error(&gMysqlLocal));
		return(0);
	}
        resLocal=mysql_store_result(&gMysqlLocal);
	unsigned uRetVal=0;
	if((fieldLocal=mysql_fetch_row(resLocal)))
		sscanf(fieldLocal[0],"%u",&uRetVal);
	mysql_free_result(resLocal);
	return(uRetVal);

}//unsigned uGetLastSignatureID(unsigned uIP)


void ProcessBarnyard2(unsigned uPriority)
{
	logfileLine("ProcessBarnyard2","start");

	//Uses login data from local.h
	//unxsVZ connection
        MYSQL_RES *res=NULL;
        MYSQL_ROW field;

	//snort connection
	MYSQL_RES *resLocal=NULL;
	MYSQL_ROW fieldLocal;

	//Check last 60 seconds event for priorty 1 events.
	//If any get the IP or IPs to block.
	sprintf(gcQuery,"SELECT DISTINCT INET_NTOA(iphdr.ip_src),iphdr.ip_src FROM event,iphdr,signature"
			" WHERE event.cid=iphdr.cid"
			" AND event.signature=signature.sig_id"
			" AND event.timestamp>(NOW()-61)"
			" AND signature.sig_priority=%u LIMIT 17",uPriority);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("ProcessBarnyard2-s0",mysql_error(&gMysqlLocal));
		goto ProcessBarnyard2_exit0;
		
	}
        resLocal=mysql_store_result(&gMysqlLocal);
	unsigned uEventCount=0;
	while((fieldLocal=mysql_fetch_row(resLocal)))
	{
		unsigned uTmpPriority=0;
		unsigned uDstIPCount=0;

		uEventCount++;
		if(uEventCount>16)
		{
			logfileLine("ProcessBarnyard2","Some events ignored: Went over 16");
			break;
		}

		MYSQL_RES *resLocal2=NULL;
		unsigned uIP=0;//src ip number
		char cIP[16]={""};
		sscanf(fieldLocal[1],"%u",&uIP);
		sprintf(cIP,"%.15s",fieldLocal[0]);
		//logfileLine("ProcessBarnyard2",cIP);

		if(!uIP)
		{
			logfileLine("ProcessBarnyard2","error uIP==0");
			continue;
		}

		//Get country info
		char cGeoIPCountryInfo[64]={"no country info available"};
		CheckIP("",cGeoIPCountryInfo,uIP);

		//keep track of IP history/reputation in our own table
		//note sig_id and sig_sid diff
		unsigned uFWRule=0;	
		sprintf(gcQuery,"SELECT iphdr.ip_dst,signature.sig_id,signature.sig_sid FROM iphdr,signature,event"
			" WHERE event.cid=iphdr.cid"
			" AND event.signature=signature.sig_id"
			" AND event.timestamp>(NOW()-61)"
			" AND iphdr.ip_src=%u"
			" AND signature.sig_priority=%u",uIP,uPriority);
		mysql_query(&gMysqlLocal,gcQuery);
		if(mysql_errno(&gMysqlLocal))
		{
			logfileLine("ProcessBarnyard2-s9",mysql_error(&gMysqlLocal));
		}
		else
		{
			MYSQL_ROW fieldLocal2;
			unsigned uIPDst=0;
			unsigned uSignature=0;	
        		resLocal2=mysql_store_result(&gMysqlLocal);
			while((fieldLocal2=mysql_fetch_row(resLocal2)))
			{
				sscanf(fieldLocal2[0],"%u",&uIPDst);
				sscanf(fieldLocal2[1],"%u",&uSignature);
				sscanf(fieldLocal2[2],"%u",&uFWRule);
				sprintf(gcQuery,"INSERT INTO tIPHistory SET"
					" uIPSrc=%u,"
					" uIPDst=%u,"
					" uSignature=%u,"
					" uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,uIP,uIPDst,uSignature);
				mysql_query(&gMysqlLocal,gcQuery);
				if(mysql_errno(&gMysqlLocal))
				{
					logfileLine("ProcessBarnyard2-i9",mysql_error(&gMysqlLocal));
					break;
				}
			}
		}

		//Check to see if we should escalate priority
		//escalate based on alerts from same IP from more than one target dst IP in last 24 hours.
		//but calculate uDstIPCount for all events
		sprintf(gcQuery,"SELECT DISTINCT tIPHistory.uIPDst"
			" FROM tIPHistory,signature"
			" WHERE tIPHistory.uSignature=signature.sig_id AND tIPHistory.uCreatedDate>(UNIX_TIMESTAMP(NOW())-86400)"
			" AND tIPHistory.uIPSrc=%u",uIP);
		mysql_query(&gMysqlLocal,gcQuery);
		if(mysql_errno(&gMysqlLocal))
			logfileLine("ProcessBarnyard2-s3",mysql_error(&gMysqlLocal));
		
        	resLocal2=mysql_store_result(&gMysqlLocal);
		uDstIPCount=mysql_num_rows(resLocal2);
		if(resLocal2!=NULL)
			mysql_free_result(resLocal2);
		if(uPriority>1)
		{

			if(uDstIPCount>3)
			{
				uTmpPriority=1;
				char cMsg[64]={""};
				sprintf(cMsg,"priority escalation %u",uDstIPCount);
				logfileLine("ProcessBarnyard2",cMsg);
			}
		}

		

		//Lets check backend for tIP.uFWStatus
		sprintf(gcQuery,"SELECT uFWStatus FROM tIP WHERE"
			" uIPNum=%u"
			" AND uDatacenter=41"//CustomerPremise magic number fix ASAP
			" AND uAvailable=0 LIMIT 1",uIP);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessBarnyard2-s1a",mysql_error(&gMysql));
			goto ProcessBarnyard2_exit1;
		}
		res=mysql_store_result(&gMysql);
		unsigned uFWStatus=0;//first job is master job
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uFWStatus);

		sprintf(gcQuery,"cIP=%s uFWStatus=%u uTmpPriority=%u uPriority=%u",cIP,uFWStatus,uTmpPriority,uPriority);
		logfileLine("ProcessBarnyard2",gcQuery);

		//Based on uFWStatus and uPriority/uTmpPriority
		//We create drop/allow or no job at all.
		//If no IP status we contine as is.
		//If we are going to DROP but status is waiting or blocked. we can skip
		//If status is uFWWHITELISTED do nothing
		unsigned uCount=0;
		if( ((uFWStatus==uFWWAITINGBLOCK || uFWStatus==uFWBLOCKED) && (uPriority==1 || uTmpPriority))
			||
			((uFWStatus==uFWWAITINGACCESS || uFWStatus==uFWACCESS) && (uPriority>2 || !uTmpPriority))
			||
			((uFWStatus==uFWWHITELISTED)) )
		{
			logfileLine("ProcessBarnyard2","already done or whitelisted");
			//we should add unique activity record for datacenter and signature
			//uSignature is only the last of possible multiple
			UpdateVZIPProperty(0,uIP,uFWStatus,uFWRule,uDstIPCount);
		}
		else
		{
		
			//Create a tJob entry for each active tNode
			sprintf(gcQuery,"SELECT tNode.uNode,tDatacenter.uDatacenter FROM tNode,tDatacenter"
					" WHERE tDatacenter.uStatus=1 AND tNode.uStatus=1"
					" AND tDatacenter.cLabel!='CustomerPremise'"
					" AND tNode.uDatacenter=tDatacenter.uDatacenter");
			//		debug only
			//		" AND tNode.uDatacenter=tDatacenter.uDatacenter LIMIT 1");
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessBarnyard2-s1",mysql_error(&gMysql));
				goto ProcessBarnyard2_exit2;
			}
				res=mysql_store_result(&gMysql);
			unsigned uNode=0;
			unsigned uDatacenter=0;
			unsigned uMasterJob=0;//first job is master job
			while((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&uNode);
				sscanf(field[1],"%u",&uDatacenter);
	
				if(uPriority>2 && !uTmpPriority)
					sprintf(gcQuery,"INSERT INTO tJob"
						" SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cLabel='TestBlockAccess unxsSnort %s'"
						",cJobName='AllowAllAccess'"
						//",cJobName='TestBlockAccess'"
						",uDatacenter=%u,uNode=%u"
						",uMasterJob=%u"
						",cJobData='cIPv4=%.15s;\nuGeoIPCountryInfo=%s;\nuPriority=%u/%u;'"
						",uJobDate=UNIX_TIMESTAMP(NOW())"
						",uJobStatus=1",
							cIDS,
							uDatacenter,
							uNode,
							uMasterJob,
							cIP,cGeoIPCountryInfo,uPriority,uTmpPriority);
				else
					sprintf(gcQuery,"INSERT INTO tJob"
						" SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						",cLabel='BlockAccess unxsSnort %s'"
						",cJobName='BlockAccess'"
						//",cJobName='TestBlockAccess'"
						",uDatacenter=%u,uNode=%u"
						",uMasterJob=%u"
						",cJobData='cIPv4=%.15s;\nuGeoIPCountryInfo=%s;\nuPriority=%u/%u;'"
						",uJobDate=UNIX_TIMESTAMP(NOW())"
						",uJobStatus=1",
							cIDS,
							uDatacenter,
							uNode,
							uMasterJob,
							cIP,cGeoIPCountryInfo,uPriority,uTmpPriority);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					logfileLine("ProcessBarnyard2-s2",mysql_error(&gMysql));
					goto ProcessBarnyard2_exit2;
				}
				uCount++;
				if(!uMasterJob)
					uMasterJob=mysql_insert_id(&gMysql);
			}//while for each server job
		}//remote status condition

		if(uCount)
		{
			unsigned uSignatureID=0;

			uSignatureID=uGetLastSignatureID(uIP);

			sprintf(gcQuery,"%u jobs created uSignatureID=%u;",uCount,uSignatureID);
			logfileLine("ProcessBarnyard2",gcQuery);

			char cCountryCode[3]={"--"};
			unsigned uCountryCode=0;
			uCountryCode=uGetCountryCode(cIP,cCountryCode);
			if(uPriority<3 || uTmpPriority)
			{
				UpdateVZIP("",uIP,uFWWAITINGBLOCK,uSignatureID,uCountryCode,uDstIPCount);
				logfileLine("ProcessBarnyard2","priority 1");
			}
			else
			{
				UpdateVZIP("",uIP,uFWWAITINGACCESS,uSignatureID,uCountryCode,uDstIPCount);
			}

			//Send email etc. one for each IP
			char cMsg[100]={""};
			sprintf(cMsg,"%.15s %.64s",cIP,cGeoIPCountryInfo);
			ForkPostAddScript(cMsg,cIP);

			//debug only
			logfileLine("ProcessBarnyard2",cIP);
			logfileLine("ProcessBarnyard2",cGeoIPCountryInfo);
		}
	}//while each event

ProcessBarnyard2_exit2:
	if(res!=NULL)
		mysql_free_result(res);
ProcessBarnyard2_exit1:
	if(resLocal!=NULL)
		mysql_free_result(resLocal);
ProcessBarnyard2_exit0:
	logfileLine("ProcessBarnyard2","end");

}//void ProcessBarnyard2(unsigned uPriority)


unsigned uGetCountryCode(const char *cIP,char *cCountryCode)
{
	MYSQL_RES *resLocal;
	MYSQL_ROW fieldLocal;
	unsigned uRetVal=0;

	sprintf(gcQuery,"SELECT cCountryCode,uGeoIPCountryCode"
			" FROM tGeoIPCountryCode"
			" WHERE uGeoIPCountryCode="
			"(SELECT uGeoIPCountryCode FROM tGeoIP WHERE uEndIP>=INET_ATON('%s') LIMIT 1)",cIP);
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("uGetCountryCode",mysql_error(&gMysqlLocal));
		return(0);
	}
        resLocal=mysql_store_result(&gMysqlLocal);
	if((fieldLocal=mysql_fetch_row(resLocal)))
	{
		sprintf(cCountryCode,"%.2s",fieldLocal[0]);
		sscanf(fieldLocal[1],"%u",&uRetVal);
	}
	mysql_free_result(resLocal);

	return(uRetVal);

}//unsigned uGetCountryCode(const char *cIP,char *cCountryCode)


void Cleanup(void)
{
	if(TextLocalConnectDb())
	{
		return;
	}

	logfileLine("Cleanup()","start");
	sprintf(gcQuery,"DELETE FROM tIPHistory WHERE uCreatedDate<UNIX_TIMESTAMP(NOW() - INTERVAL 1 DAY)");
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("Cleanup()",mysql_error(&gMysqlLocal));
		return;
	}
	mysql_close(&gMysqlLocal);
	logfileLine("Cleanup()","end");
}//void Cleanup(void)


void CreateIPHistoryTable(void)
{
	if((gLfp=fopen(cSNORTLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
		exit(1);
	}
	logfileLine("CreateIPHistoryTable","start");
		
	if(TextLocalConnectDb()) exit(1);

	sprintf(gcQuery,"DROP TABLE IF EXISTS tIPHistory");
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("CreateIPHistoryTable","error");
		logfileLine("CreateIPHistoryTable",mysql_error(&gMysqlLocal));
		mysql_close(&gMysqlLocal);
		return;
	}

	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tIPHistory ( "
			"uIPSrc INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uIPSrc),"
			"uIPDst INT UNSIGNED NOT NULL DEFAULT 0,"
			" uSignature INT UNSIGNED NOT NULL DEFAULT 0,"
			" uCreatedDate BIGINT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysqlLocal,gcQuery);
	if(mysql_errno(&gMysqlLocal))
	{
		logfileLine("CreateIPHistoryTable","error");
		logfileLine("CreateIPHistoryTable",mysql_error(&gMysqlLocal));
		mysql_close(&gMysqlLocal);
		return;
	}

	logfileLine("CreateIPHistoryTable","Ok");
	mysql_close(&gMysqlLocal);
	logfileLine("CreateIPHistoryTable","end");

}//void CreateIPHistoryTable(void)


unsigned UpdateVZIPProperty(unsigned uIP,unsigned uIPNum,unsigned uFWStatus,unsigned uFWRule,unsigned uDstIPCount)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	if(!uIP)
	{
		sprintf(gcQuery,"SELECT uIP FROM tIP WHERE"
			" uIPNum=%u"
			" AND uDatacenter=41"//CustomerPremise magic number fix ASAP
			" AND uAvailable=0 LIMIT 1",uIPNum);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("UpdateVZIPProperty",mysql_error(&gMysql));
			return(1);
		}
        	res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uIP);
		else
			return(1);
	}

	sprintf(gcQuery,"SELECT uProperty,cValue FROM tProperty"
			" WHERE cName='FWA %s %u'"
			" AND uKey=%u"
			" AND uType=31 LIMIT 1",
				cIDS,uFWRule,
					uIP);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("UpdateVZIPProperty",mysql_error(&gMysql));
		return(1);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		unsigned uMaxDstIPCount=0;
		unsigned uCount=0;
		if(2==sscanf(field[1],"uLastFWStatus=%*u; uMaxDstIPCount=%u; uCount=%u;",&uMaxDstIPCount,&uCount))
		{
			if(uDstIPCount>uMaxDstIPCount) uMaxDstIPCount=uDstIPCount;
			uCount++;
		}
		else
		{
			logfileLine("UpdateVZIPProperty","sscanf() error");
		}
		sprintf(gcQuery,"UPDATE tProperty"
			" SET cValue=CONCAT('uLastFWStatus=%u; uMaxDstIPCount=%u; uCount=%u;')"
			",uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
			" WHERE uProperty=%s",
				uFWStatus,uMaxDstIPCount,uCount,
					field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("UpdateVZIPProperty",mysql_error(&gMysql));
			return(2);
		}
	}
	else
	{
		sprintf(gcQuery,"INSERT INTO tProperty"
			" SET cName='FWA %s %u'"
			",cValue=CONCAT('uLastFWStatus=%u; uMaxDstIPCount=%u; uCount=1;')"
			",uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())"
			",uType=31"
			",uKey=%u",cIDS,uFWRule,uFWStatus,uDstIPCount,uIP);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("UpdateVZIPProperty",mysql_error(&gMysql));
			return(3);
		}
	}
	mysql_free_result(res);

	//debug only
	//logfileLine("UpdateVZIPProperty",gcQuery);

	return(0);
}//unsigned UpdateVZIPProperty()
