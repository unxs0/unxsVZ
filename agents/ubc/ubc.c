/*
FILE
	ubc.c
	$Id$
PURPOSE
	Collection of diverse VZ operating parameters and other system vars.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2008-2010.
	GPLv2 License applies. See LICENSE file.
NOTES
	For latest autonomic functions we need to collect one week of data.
	This weekly data wil be limited to only a few UBC values, and disk usage
	quota data. The basic idea is to provide trend information for better
	informed elastic autonomics.

	Starting to work on ticket unxsVZ Trac ticket #120
*/

#include "../../mysqlrad.h"
#include <sys/sysinfo.h>

#define cUBCLOGFILE "/var/log/unxsUBC.log"

MYSQL gMysql;
MYSQL gMysqlUBC;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};
char gcProgram[32]={""};
unsigned guNodeOwner=1;
unsigned guContainerOwner=1;
unsigned guStatus=0;//not a valid status
unsigned guParallel=0;
unsigned guNoUBC=0;
unsigned guUBCOnly=0;
unsigned guRunUBC=1;
unsigned guRunQuota=1;
unsigned guRunStatus=1;
unsigned guRunMemCheck=1;
unsigned guRunCPUCheck=1;
unsigned guRunTraffic=1;

char *gcUBCDBIP0=DBIP0;
char *gcUBCDBIP1=DBIP1;
char gcUBCDBIP0Buffer[32]={""};
char gcUBCDBIP1Buffer[32]={""};

//local protos
void TextConnectDb0(void);
void TextConnectDbUBC(void);
void ProcessSingleUBC(unsigned uContainer, unsigned uNode);
void ProcessUBC(void);
void ProcessNodeUBC(void);
void ProcessSingleHDUsage(unsigned uContainer);
void ProcessSingleQuota(unsigned uContainer);
void ProcessSingleStatus(unsigned uContainer);
void ProcessSingleTraffic(unsigned uContainer);
void ProcessVZMemCheck(unsigned uContainer, unsigned uNode);
void ProcessVZCPUCheck(unsigned uContainer, unsigned uNode);
void UpdateContainerUBCJob(unsigned uContainer, char *cResource);
void ProcessSingleTraffic(unsigned uContainer);
void SendEmail(char *cSubject,char *cMsg);
void UBCConnectToOptionalUBCDb(unsigned uDatacenter);

unsigned guLogLevel=3;
static FILE *gLfp0=NULL;
void logfileLine0(const char *cFunction,const char *cLogline,const unsigned uContainer)
{
	FILE *fp=stdout;

	if(gLfp0!=NULL)
		fp=gLfp0;

	time_t luClock;
	char cTime[32];
	pid_t pidThis;
	const struct tm *tmTime;

	pidThis=getpid();

	time(&luClock);
	tmTime=localtime(&luClock);
	strftime(cTime,31,"%b %d %T",tmTime);

	fprintf(fp,"%s unxsUBC.%s[%u]: %s.",cTime,cFunction,pidThis,cLogline);
	if(uContainer)
		fprintf(fp," %u",uContainer);
	fprintf(fp,"\n");
	fflush(fp);

}//void logfileLine0()

#define LINUX_SYSINFO_LOADS_SCALE 65536
#define JOBQUEUE_MAXLOAD 10 //This is equivalent to uptime 10.00 last 5 min avg load
struct sysinfo structSysinfo;

int main(int iArgc, char *cArgv[])
{


	if(iArgc>1)
	{
		register int i;
		for(i=1;i<iArgc;i++)
		{
			if(!strcmp(cArgv[i],"--parallel"))
				guParallel=1;
			if(!strcmp(cArgv[i],"--noUBC"))
				guNoUBC=1;
			if(!strcmp(cArgv[i],"--UBCOnly"))
				guUBCOnly=1;
			if(!strcmp(cArgv[i],"--help"))
			{
				printf("usage: %s [--parallel] [--noUBC] [--UBCOnly] [--help] [--version] [--initUBCDB]\n",cArgv[0]);
				exit(0);
			}
			if(!strcmp(cArgv[i],"--version"))
			{
				printf("version: %s $Id$\n",cArgv[0]);
				//SendEmail("ubc.c test","ubc.c test");
				exit(0);
			}
			if(!strcmp(cArgv[i],"--initUBCDB"))
			{
        			MYSQL_RES *res;
        			MYSQL_ROW field;
				unsigned uDatacenter=0;
				char *cp;

				TextConnectDb0();
				gethostname(cHostname,99);
				if((cp=strchr(cHostname,'.')))
					*cp=0;
				//printf("Connected to %s %s from %s\n",DBIP0,DBIP1,cHostname);
				printf("Connected from %s\n",cHostname);

				sprintf(gcQuery,"SELECT uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s",mysql_error(&gMysql));
					mysql_close(&gMysql);
					exit(2);
				}
				res=mysql_store_result(&gMysql);
				if((field=mysql_fetch_row(res)))
					sscanf(field[0],"%u",&uDatacenter);
				mysql_free_result(res);

				UBCConnectToOptionalUBCDb(uDatacenter);
				sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tProperty ( "
					"uProperty INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
					"cName VARCHAR(32) NOT NULL DEFAULT '', INDEX (cName),"
					"uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner),"
					"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
					"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
					"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
					"uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
					"cValue TEXT NOT NULL DEFAULT '',"
					"uKey INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uKey),"
					"uType INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uType) )");
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
					printf("%s",mysql_error(&gMysqlUBC));
				else if(mysql_affected_rows(&gMysqlUBC))
					printf("Created tProperty table at %s for uDatacenter=%u\n",gcUBCDBIP0,uDatacenter);

				sprintf(gcQuery,"SELECT COUNT(*) FROM tProperty");
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
					printf("%s",mysql_error(&gMysqlUBC));
				else
				{
			        	res=mysql_store_result(&gMysqlUBC);
					if((field=mysql_fetch_row(res)))
						printf("tProperty rows=%s\n",field[0]);
					mysql_free_result(res);
				}

				mysql_close(&gMysqlUBC);
				mysql_close(&gMysql);
				exit(0);
			}
		}
	}

	char cLockfile[64]={"/tmp/ubc.lock"};
	if(guUBCOnly)
		sprintf(cLockfile,"/tmp/ubc.ubconly.lock");

	sprintf(gcProgram,"%.31s",cArgv[0]);
	if((gLfp0=fopen(cUBCLOGFILE,"a"))==NULL)
	{
		fprintf(stderr,"%s main() fopen logfile error\n",gcProgram);
		exit(1);
	}
		
	if(sysinfo(&structSysinfo))
	{
		logfileLine0("main","sysinfo() failed",0);
		exit(1);
	}
	if(structSysinfo.loads[1]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		logfileLine0("main","structSysinfo.loads[1] larger than JOBQUEUE_MAXLOAD",0);
		exit(1);
	}
	//Check to see if this program is still running. If it is exit.
	//This may mean losing data gathering data points. But it
	//will avoid runaway du and other unexpected high load
	//situations. See #120.

	struct stat structStat;
	if(!stat(cLockfile,&structStat))
	{
		logfileLine0("main","waiting for rmdir(cLockfile)",0);
		return(1);
	}
	if(mkdir(cLockfile,S_IRUSR|S_IWUSR|S_IXUSR))
	{
		logfileLine0("main","could not open cLockfile dir",0);
		return(1);
	}

	if(guParallel)
	{
		unsigned uPID=0;

		switch((uPID=fork()))
		{

			default:
				//parent
			break;

			case -1:
				//failure
				exit(1);

			case 0:
			{
				char cLockfile[64]={"/tmp/ubc.lock.fork1"};
				if(!stat(cLockfile,&structStat))
				{
					logfileLine0("main","waiting for rmdir(cLockfile) fork 1",0);
					return(1);
				}
				if(mkdir(cLockfile,S_IRUSR|S_IWUSR|S_IXUSR))
				{
					logfileLine0("main","could not open cLockfile dir fork 1",0);
					return(1);
				}
				//child
				logfileLine0("main","fork 1 guRunTraffic",uPID);
				guRunUBC=0;
				guRunQuota=0;
				guRunStatus=0;
				guRunMemCheck=0;
				guRunCPUCheck=0;
				guRunTraffic=1;
				ProcessUBC();
				if(rmdir(cLockfile))
				{
					logfileLine0("main","could not rmdir(cLockfile) fork 1",0);
					return(1);
				}
				logfileLine0("main","end fork 1",uPID);
				return(0);
			}
			break;
		}
		switch((uPID=fork()))
		{

			default:
				//parent
			break;

			case -1:
				//failure
				exit(1);

			case 0:
			{
				char cLockfile[64]={"/tmp/ubc.lock.fork2"};
				if(!stat(cLockfile,&structStat))
				{
					logfileLine0("main","waiting for rmdir(cLockfile) fork 2",0);
					return(1);
				}
				if(mkdir(cLockfile,S_IRUSR|S_IWUSR|S_IXUSR))
				{
					logfileLine0("main","could not open cLockfile dir fork 2",0);
					return(1);
				}
				//child
				logfileLine0("main","fork 2 guRunCPUCheck",uPID);
				guRunUBC=0;
				guRunQuota=0;
				guRunStatus=0;
				guRunMemCheck=0;
				guRunCPUCheck=1;
				guRunTraffic=0;
				ProcessUBC();
				if(rmdir(cLockfile))
				{
					logfileLine0("main","could not rmdir(cLockfile) fork 2",0);
					return(1);
				}
				logfileLine0("main","end fork 2",uPID);
				return(0);
			}
			break;
		}
		switch((uPID=fork()))
		{

			default:
				//parent
			break;

			case -1:
				//failure
				exit(1);

			case 0:
			{
				char cLockfile[64]={"/tmp/ubc.lock.fork3"};
				if(!stat(cLockfile,&structStat))
				{
					logfileLine0("main","waiting for rmdir(cLockfile) fork 3",0);
					return(1);
				}
				if(mkdir(cLockfile,S_IRUSR|S_IWUSR|S_IXUSR))
				{
					logfileLine0("main","could not open cLockfile dir fork 3",0);
					return(1);
				}
				//child
				logfileLine0("main","fork 3 guRunMemCheck",uPID);
				guRunUBC=0;
				guRunQuota=0;
				guRunStatus=0;
				guRunMemCheck=1;
				guRunCPUCheck=0;
				guRunTraffic=0;
				ProcessUBC();
				if(rmdir(cLockfile))
				{
					logfileLine0("main","could not rmdir(cLockfile) fork 3",0);
					return(1);
				}
				logfileLine0("main","end fork 3",uPID);
				return(0);
			}
			break;
		}
		switch((uPID=fork()))
		{

			default:
				//parent
			break;

			case -1:
				//failure
				exit(1);

			case 0:
			{
				char cLockfile[64]={"/tmp/ubc.lock.fork4"};
				if(!stat(cLockfile,&structStat))
				{
					logfileLine0("main","waiting for rmdir(cLockfile) fork 4",0);
					return(1);
				}
				if(mkdir(cLockfile,S_IRUSR|S_IWUSR|S_IXUSR))
				{
					logfileLine0("main","could not open cLockfile dir fork 4",0);
					return(1);
				}
				//child
				logfileLine0("main","fork 4 guRunStatus",uPID);
				guRunUBC=0;
				guRunQuota=0;
				guRunStatus=1;
				guRunMemCheck=0;
				guRunCPUCheck=0;
				guRunTraffic=0;
				ProcessUBC();
				if(rmdir(cLockfile))
				{
					logfileLine0("main","could not rmdir(cLockfile) fork 4",0);
					return(1);
				}
				logfileLine0("main","end fork 4",uPID);
				return(0);
			}
			break;
		}
		switch((uPID=fork()))
		{

			default:
				//parent
			break;

			case -1:
				//failure
				exit(1);

			case 0:
			{
				char cLockfile[64]={"/tmp/ubc.lock.fork5"};
				if(!stat(cLockfile,&structStat))
				{
					logfileLine0("main","waiting for rmdir(cLockfile) fork 5",0);
					return(1);
				}
				if(mkdir(cLockfile,S_IRUSR|S_IWUSR|S_IXUSR))
				{
					logfileLine0("main","could not open cLockfile dir fork 5",0);
					return(1);
				}
				//child
				logfileLine0("main","fork 5 guRunQuota",uPID);
				guRunUBC=0;
				guRunQuota=1;
				guRunStatus=0;
				guRunMemCheck=0;
				guRunCPUCheck=0;
				guRunTraffic=0;
				ProcessUBC();
				if(rmdir(cLockfile))
				{
					logfileLine0("main","could not rmdir(cLockfile) fork 5",0);
					return(1);
				}
				logfileLine0("main","end fork 5",uPID);
				return(0);
			}
			break;
		}
		switch((uPID=fork()))
		{

			default:
				//parent
			break;

			case -1:
				//failure
				exit(1);

			case 0:
			{
				char cLockfile[64]={"/tmp/ubc.lock.fork6"};
				if(!stat(cLockfile,&structStat))
				{
					logfileLine0("main","waiting for rmdir(cLockfile) fork 6",0);
					return(1);
				}
				if(mkdir(cLockfile,S_IRUSR|S_IWUSR|S_IXUSR))
				{
					logfileLine0("main","could not open cLockfile dir fork 6",0);
					return(1);
				}
				//child
				logfileLine0("main","fork 6 guRunUBC",uPID);
				guRunUBC=1;
				guRunQuota=0;
				guRunStatus=0;
				guRunMemCheck=0;
				guRunCPUCheck=0;
				guRunTraffic=0;
				ProcessUBC();
				if(rmdir(cLockfile))
				{
					logfileLine0("main","could not rmdir(cLockfile) fork 6",0);
					return(1);
				}
				logfileLine0("main","end fork 6",uPID);
				return(0);
			}
			break;
		}
	}
	else
	{
		logfileLine0("main","start",0);
		ProcessUBC();//For all node containers
	}

	ProcessNodeUBC();

	if(rmdir(cLockfile))
	{
		logfileLine0("main","could not rmdir(cLockfile)",0);
		return(1);
	}
	logfileLine0("main","end",0);
	return(0);
}//main()


void ProcessSingleUBC(unsigned uContainer, unsigned uNode)
{
	if(!guRunUBC) return;
	if(guLogLevel>3)
		logfileLine0("ProcessSingleUBC","start",uContainer);

	FILE *fp;
	char cLine[256];
	char cContainerTag[64];
	register unsigned uStart=0;
	unsigned long luHeld,luMaxheld,luBarrier,luLimit,luFailcnt;
	char cResource[64];
	unsigned uType=3;//tContainer type

	sprintf(cContainerTag,"%u:  kmemsize",uContainer);
	if(uContainer)
	{
		//debug only
		//printf("\tProcessSingleUBC(Container=%u)\n",uContainer);
		;
	}
	else if(uNode)
	{
		//debug only
		//printf("\tProcessSingleUBC(Node=%u)\n",uNode);
		uContainer=uNode;
		guContainerOwner=guNodeOwner;
		uType=2;
	}
	else if(1)
	{
		logfileLine0("ProcessSingleUBC","No container or node specified",uContainer);
		exit(1);
	}
		

	if((fp=fopen("/proc/user_beancounters","r")))
	{
		while(fgets(cLine,1024,fp)!=NULL)
		{
			cResource[0]=0;
			luHeld=0;
			luMaxheld=0;
			luBarrier=0;
			luLimit=0;
			luFailcnt=0;

			if(!uStart)
			{
				if(strstr(cLine,cContainerTag))
					uStart=1;
			}
			else
			{
				if(strchr(cLine,':')) break;
			}

			if(uStart==1)
			{
				sprintf(cResource,"kmemsize");
				sscanf(cLine,"%*u:  kmemsize %lu %lu %lu %lu %lu",
					&luHeld,&luMaxheld,&luBarrier,&luLimit,&luFailcnt);
				uStart++;
			}
			else if(uStart)
			{
				sscanf(cLine,"%s %lu %lu %lu %lu %lu",
					cResource,
					&luHeld,&luMaxheld,&luBarrier,&luLimit,&luFailcnt);
				if(!strcmp("dummy",cResource))
					continue;
				uStart++;
			}

			if(uStart)
			{
        			MYSQL_RES *res;
        			MYSQL_ROW field;
				register int i;
				char cKnownUBCVals[8][32]={"luHeld","luMaxheld","luBarrier",
					"luLimit","luFailcnt","luFailDelta","",""};
				long unsigned luKnownUBCVals[8];

				//debug only
				//printf("\t%s luHeld=%lu luMaxheld=%lu luBarrier=%lu luLimit=%lu luFailcnt=%lu\n",
				//	cResource,
				//	luHeld,luMaxheld,luBarrier,luLimit,luFailcnt);
				//update or insert into tProperty for each cResource.lu<value label>
				luKnownUBCVals[0]=luHeld;
				luKnownUBCVals[1]=luMaxheld;
				luKnownUBCVals[2]=luBarrier;
				luKnownUBCVals[3]=luLimit;
				luKnownUBCVals[4]=luFailcnt;
				luKnownUBCVals[5]=0;

				for(i=0;i<6;i++)
				{
					sprintf(gcQuery,"SELECT uProperty,cValue FROM tProperty WHERE cName='%.63s.%.32s'"
								" AND uKey=%u AND uType=%u",
						cResource,cKnownUBCVals[i],uContainer,uType);
					mysql_query(&gMysqlUBC,gcQuery);
					if(mysql_errno(&gMysqlUBC))
					{
						logfileLine0("ProcessSingleUBC",mysql_error(&gMysqlUBC),uContainer);
						exit(2);
					}
			        	res=mysql_store_result(&gMysqlUBC);
					if((field=mysql_fetch_row(res)))
					{
						//luFailDelta calculation
						//If fails appear at least once per sample time
						//luFailDelta will accumulate. Else will reset.
						//Event is logged. An email or other notice
						//could also be done here.
						if(i==4)
						{	
							unsigned long luPrevFailcnt=0;

							sscanf(field[1],"%lu",&luPrevFailcnt);

							if(luFailcnt>luPrevFailcnt)
							{
								if(guLogLevel>2)
									logfileLine0("ProcessSingleUBC","insert luFailcnt",uContainer);
								luKnownUBCVals[5]+=luFailcnt;
								//Notify via log system message
								sprintf(gcQuery,"INSERT INTO tLog SET"
								" cLabel='UBC Agent: luFailcnt++',"
								"uLogType=4,uLoginClient=1,"
								"uTablePK=%u,cTableName='tContainer',"
								"cLogin='UBC Agent',cMessage=\"%s %lu>%lu %u:%u\","
								"cServer='%s',uOwner=%u,uCreatedBy=1,"
								"uCreatedDate=UNIX_TIMESTAMP(NOW())",
									uContainer,
									cResource,luFailcnt,luPrevFailcnt,
									uNode,uContainer,cHostname,guContainerOwner);
								mysql_query(&gMysql,gcQuery);
								if(mysql_errno(&gMysql))
								{
									logfileLine0("ProcessSingleUBC",
										mysql_error(&gMysql),uContainer);
									exit(2);
								}
								//First autonomic foray
								UpdateContainerUBCJob(uContainer,cResource);
							}
							else
							{
								luKnownUBCVals[5]=0;
							}
						}
						if(guLogLevel>4)
							logfileLine0("ProcessSingleUBC","update",uContainer);
						sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
								"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
								" cName='%.63s.%.32s' AND uProperty=%s"
									,luKnownUBCVals[i]
									,guContainerOwner
									,cResource
									,cKnownUBCVals[i]
									,field[0]);
								mysql_query(&gMysqlUBC,gcQuery);
								if(mysql_errno(&gMysqlUBC))
								{
									logfileLine0("ProcessSingleUBC",
										mysql_error(&gMysqlUBC),uContainer);
									exit(2);
								}
					}
					else
					{
						if(guLogLevel>4)
							logfileLine0("ProcessSingleUBC","insert",uContainer);
						sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
								",cName='%.63s.%.32s'"
								",uType=%u"
								",uKey=%u"
								",uOwner=%u"
								",uCreatedBy=1"
								",uCreatedDate=UNIX_TIMESTAMP(NOW())"
									,luKnownUBCVals[i]
									,cResource
									,cKnownUBCVals[i]
									,uType
									,uContainer,guContainerOwner);
						mysql_query(&gMysqlUBC,gcQuery);
						if(mysql_errno(&gMysqlUBC))
						{
							logfileLine0("ProcessSingleUBC",mysql_error(&gMysqlUBC),
									uContainer);
							exit(2);
						}
					}
					mysql_free_result(res);
				}
			}
		}
		fclose(fp);
	}
	else
	{
		logfileLine0("ProcessSingleUBC","fopen() failed",uContainer);
	}

}//void ProcessSingleUBC(unsigned uContainer, unsigned uNode)


void ProcessUBC(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uDatacenter=0;
	unsigned uNode=0;
	unsigned uContainer=0;

	if(gethostname(cHostname,99)!=0)
	{
		logfileLine0("ProcessUBC","gethostname() failed",uContainer);
		exit(1);
	}

	char cRandom[8]={""};
	int fd=open("/dev/urandom",O_RDONLY);
	if(fd>0)
	{
		if(!read(fd,cRandom,sizeof(cRandom))!=sizeof(cRandom))
			(void)srand((unsigned)cRandom[0]);
	}

	if(!cRandom[0])
	{
		logfileLine0("ProcessJobQueue","/dev/urandom error",uContainer);
		(void)srand((int)time((time_t *)NULL));
	}

	unsigned uDelay=0;
    	uDelay=rand() % 60;
	sprintf(gcQuery,"random delay of %us added",uDelay);
	logfileLine0("ProcessJobQueue",gcQuery,uContainer);
	sleep(uDelay);


	//Uses login data from local.h
	TextConnectDb0();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT uNode,uDatacenter,uOwner FROM tNode WHERE cLabel='%.99s'",cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine0("ProcessUBC",mysql_error(&gMysql),uContainer);
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
			logfileLine0("ProcessUBC",mysql_error(&gMysql),uContainer);
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
		logfileLine0("ProcessUBC","Could not determine uNode",uDatacenter);
		mysql_close(&gMysql);
		exit(1);
	}


	UBCConnectToOptionalUBCDb(uDatacenter);

	//debug only
	//printf("ProcessUBC() for %s (uNode=%u,uDatacenter=%u)\n",
	//		cHostname,uNode,uDatacenter);

	//Main loop. TODO use defines for tStatus.uStatus values.
	sprintf(gcQuery,"SELECT uContainer,uOwner,uStatus FROM tContainer WHERE uNode=%u"
				" AND uDatacenter=%u"
				" AND (uStatus=1"//Active
				" OR uStatus=31)"//Stopped
						,uNode,uDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine0("ProcessUBC",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		mysql_close(&gMysqlUBC);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(sysinfo(&structSysinfo))
		{
			logfileLine0("ProcessUBC","sysinfo() failed",0);
			exit(1);
		}
		if(structSysinfo.loads[1]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
		{
			logfileLine0("ProcessUBC","structSysinfo.loads[1] larger than JOBQUEUE_MAXLOAD",0);
			mysql_free_result(res);
			mysql_close(&gMysql);
			mysql_close(&gMysqlUBC);
			return;
		}

		sscanf(field[0],"%u",&uContainer);
		sscanf(field[1],"%u",&guContainerOwner);
		sscanf(field[2],"%u",&guStatus);
		//Job dispatcher based on cJobName
		if(guStatus==uSTOPPED)
		{
			ProcessSingleStatus(uContainer);
			//ProcessSingleHDUsage(uContainer);
		}
		else
		{

			if(!guNoUBC)
				ProcessSingleUBC(uContainer,0);

			if(!guUBCOnly)
			{
				ProcessSingleQuota(uContainer);
				ProcessSingleStatus(uContainer);
				ProcessVZMemCheck(uContainer,0);
				ProcessVZCPUCheck(uContainer,0);
				ProcessSingleTraffic(uContainer);
			}

			//This function needs to be much more efficient
			//ProcessSingleHDUsage(uContainer);
		}

	}
	mysql_free_result(res);
	mysql_close(&gMysql);
	mysql_close(&gMysqlUBC);

}//void ProcessUBC(void)


void ProcessNodeUBC(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uDatacenter=0;
	unsigned uNode=0;

	if(gethostname(cHostname,99)!=0)
	{
		logfileLine0("ProcessNodeUBC","gethostname() failed",0);
		exit(1);
	}

	//Uses login data from local.h
	TextConnectDb0();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT uNode,uDatacenter,uOwner FROM tNode WHERE cLabel='%.99s'",cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine0("ProcessNodeUBC",mysql_error(&gMysql),0);
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
			logfileLine0("ProcessNodeUBC",mysql_error(&gMysql),0);
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
		logfileLine0("ProcessNodeUBC","Could not determine uNode",0);
		mysql_close(&gMysql);
		exit(1);
	}

	UBCConnectToOptionalUBCDb(uDatacenter);

	//Process  node
	if(guLogLevel>2)
		logfileLine0("ProcessSingleUBC","node start",uNode);
	ProcessSingleUBC(0,uNode);

	if(guLogLevel>2)
		logfileLine0("ProcessVZMemCheck","node start",uNode);
	ProcessVZMemCheck(0,uNode);

	if(guLogLevel>2)
		logfileLine0("ProcessVZCPUCheck","node start",uNode);
	ProcessVZCPUCheck(0,uNode);

	if(guLogLevel>2)
		logfileLine0("ProcessNodeUBC","end",uNode);

	mysql_close(&gMysql);
	mysql_close(&gMysqlUBC);

}//void ProcessNodeUBC(void)


void ProcessSingleHDUsage(unsigned uContainer)
{
	char cCommand[64];
	char cLine[256];
	unsigned long luUsage=0;
	FILE *fp;
	struct stat structStat;

	if(uContainer)
	{
		//debug only
		//printf("\tProcessSingleHDQuota(Container=%u)\n",uContainer);
		;
	}
	else
	{
		logfileLine0("ProcessSingleHDUsage","No container specified",uContainer);
		exit(1);
	}

	//We will use a semaphore file to only run one du at a time.
	if(!stat("/tmp/ubchd",&structStat))
	{
		logfileLine0("ProcessSingleHDUsage","waiting for unlink(/tmp/ubchd)",uContainer);
		return;
	}
	if((fp=fopen("/tmp/ubchd","w"))==NULL)
	{
		logfileLine0("ProcessSingleHDUsage","could not open /tmp/ubchd",uContainer);
		return;
	}

	//It seems like we are stuck using du. But we can limit it's use, or not
	//run it if another one is already running. This will help with #120
	//resolution. This probably should be done globally for this whole
	//data collection agent. See main() for more on this.

	sprintf(cCommand,"nice /usr/bin/du -ks /vz/private/%u/ 2> /dev/null",uContainer);

	if((fp=popen(cCommand,"r")))
	{
        	MYSQL_RES *res;
        	MYSQL_ROW field;

		if(fgets(cLine,255,fp)!=NULL)
		{
			sscanf(cLine,"%lu ",&luUsage);
		}
		else
		{
			logfileLine0("ProcessSingleHDUsage","No lines from popen",uContainer);
			unlink("/tmp/ubchd");
			return;
		}

		if(luUsage==0)
		{
			logfileLine0("ProcessSingleHDUsage","Unexpected luUsage==0",uContainer);
			unlink("/tmp/ubchd");
			exit(1);
		}

		sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='1k-hdblocks.luUsage'"
							" AND uKey=%u AND uType=3",uContainer);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
		{
			logfileLine0("ProcessSingleHDUsage",mysql_error(&gMysqlUBC),uContainer);
			unlink("/tmp/ubchd");
			exit(2);
		}
	       	res=mysql_store_result(&gMysqlUBC);
		if((field=mysql_fetch_row(res)))
		{
			sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
					"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
					" uProperty=%s"
							,luUsage
							,guContainerOwner
							,field[0]);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleHDUsage",mysql_error(&gMysqlUBC),uContainer);
				unlink("/tmp/ubchd");
				exit(2);
			}
		}
		else
		{
			sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
					",cName='1k-hdblocks.luUsage'"
					",uType=3"
					",uKey=%u"
					",uOwner=%u"
					",uCreatedBy=1"
					",uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,luUsage
						,uContainer
						,guContainerOwner);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleHDUsage",mysql_error(&gMysqlUBC),uContainer);
				unlink("/tmp/ubchd");
				exit(2);
			}
		}
		mysql_free_result(res);
		pclose(fp);
	}
	else
	{
		logfileLine0("ProcessSingleHDUsage","popen() failed",uContainer);
	}
	unlink("/tmp/ubchd");

}//void ProcessSingleHDUsage(unsigned uContainer)


void ProcessSingleQuota(unsigned uContainer)
{
	if(!guRunQuota) return;

	if(guLogLevel>3)
		logfileLine0("ProcessSingleQuota","start",uContainer);

	FILE *fp;
	char cLine[1024];
	char cContainerTag[64];
	register unsigned uStart=0;
	unsigned long luUsage,luSoftlimit,luHardlimit,luTime,luExpire;
	char cResource[64];
	unsigned uType=3;//tContainter type

	//qid: path            usage      softlimit      hardlimit       time     expire
	//111: /vz/private/111
	//  1k-blocks         876364        1048576        1153024          0          0
	//     inodes          29792         200000         220000          0          0


	sprintf(cContainerTag,"%u: /vz/private/%u",uContainer,uContainer);
	if(uContainer)
	{
		;
	}
	else
	{
		logfileLine0("ProcessSingleQuota","No container specified",uContainer);
		exit(1);
	}
		

	if((fp=fopen("/proc/vz/vzquota","r")))
	{
		while(fgets(cLine,1024,fp)!=NULL)
		{
			cResource[0]=0;
			luUsage=0;
			luSoftlimit=0;
			luHardlimit=0;
			luTime=0;
			luExpire=0;

			if(!uStart)
			{
				if(strstr(cLine,cContainerTag))
				{
					uStart=1;
					continue;
				}
				continue;
			}
			else
			{
				if(strchr(cLine,':')) break;
			}

			if(uStart==1)
				sprintf(cResource,"1k-blocks");
			else
				sprintf(cResource,"inodes");

				
			sscanf(cLine,"%s %lu %lu %lu %lu %lu",
				cResource,
				&luUsage,&luSoftlimit,&luHardlimit,&luTime,&luExpire);

			//debug only
			//printf("%s %lu %lu %lu %lu %lu\n",
			//	cResource,
			//	luUsage,luSoftlimit,luHardlimit,luTime,luExpire);

        		MYSQL_RES *res;
        		MYSQL_ROW field;
			register int i;
			char cKnownQuotaVals[8][32]={"luUsage","luSoftlimit","luHardlimit","luTime","luExpire"};
			long unsigned luKnownQuotaVals[8];

			luKnownQuotaVals[0]=luUsage;
			luKnownQuotaVals[1]=luSoftlimit;
			luKnownQuotaVals[2]=luHardlimit;
			luKnownQuotaVals[3]=luTime;
			luKnownQuotaVals[4]=luExpire;

			for(i=0;i<5;i++)
			{
				sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='%.63s.%.32s'"
							" AND uKey=%u AND uType=%u",
					cResource,cKnownQuotaVals[i],uContainer,uType);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessSingleQuota",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
			       	res=mysql_store_result(&gMysqlUBC);
				if((field=mysql_fetch_row(res)))
				{
					sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
							"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
							" uProperty=%s"
								,luKnownQuotaVals[i]
								,guContainerOwner
								,field[0]);
					mysql_query(&gMysqlUBC,gcQuery);
					if(mysql_errno(&gMysqlUBC))
					{
						logfileLine0("ProcessSingleQuota",mysql_error(&gMysqlUBC),uContainer);
						exit(2);
					}
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
							",cName='%.63s.%.32s'"
							",uType=%u"
							",uKey=%u"
							",uOwner=%u"
							",uCreatedBy=1"
							",uCreatedDate=UNIX_TIMESTAMP(NOW())"
								,luKnownQuotaVals[i]
								,cResource
								,cKnownQuotaVals[i]
								,uType
								,uContainer
								,guContainerOwner);
					mysql_query(&gMysqlUBC,gcQuery);
					if(mysql_errno(&gMysqlUBC))
					{
						logfileLine0("ProcessSingleQuota",mysql_error(&gMysqlUBC),uContainer);
						exit(2);
					}
				}
				mysql_free_result(res);
			}

			//Keep one week of usage data for trend analysis
			for(i=0;i<1;i++)
			{
				sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE"
						" cName=CONCAT('%.63s.%.32s.',DAYOFWEEK(NOW()))"
							" AND uKey=%u AND uType=%u",
					cResource,cKnownQuotaVals[i],uContainer,uType);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessSingleQuota",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
			       	res=mysql_store_result(&gMysqlUBC);
				if((field=mysql_fetch_row(res)))
				{
					//Average
					sprintf(gcQuery,"UPDATE tProperty SET cValue=CONVERT((%lu+cValue)/2,UNSIGNED)"
							",uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
							" uProperty=%s"
								,luKnownQuotaVals[i]
								,guContainerOwner
								,field[0]);
					mysql_query(&gMysqlUBC,gcQuery);
					if(mysql_errno(&gMysqlUBC))
					{
						logfileLine0("ProcessSingleQuota",mysql_error(&gMysqlUBC),uContainer);
						exit(2);
					}
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
							",cName=CONCAT('%.63s.%.32s.',DAYOFWEEK(NOW()))"
							",uType=%u"
							",uKey=%u"
							",uOwner=%u"
							",uCreatedBy=1"
							",uCreatedDate=UNIX_TIMESTAMP(NOW())"
								,luKnownQuotaVals[i]
								,cResource
								,cKnownQuotaVals[i]
								,uType
								,uContainer
								,guContainerOwner);
					mysql_query(&gMysqlUBC,gcQuery);
					if(mysql_errno(&gMysqlUBC))
					{
						logfileLine0("ProcessSingleQuota",mysql_error(&gMysqlUBC),uContainer);
						exit(2);
					}
				}
				mysql_free_result(res);
			}
		}
		fclose(fp);
	}
	else
	{
		logfileLine0("ProcessSingleQuota","fopen() failed",uContainer);
	}

}//void ProcessSingleQuota(unsigned uContainer)


/*
Output values in %
veid		LowMem          RAM     MemSwap		Alloc
		util  commit    util    util  commit    util  commit   limit
321		0.03    1.74    0.05    0.02    2.41    0.02    2.41    6.48
291		0.06    1.71    0.10    0.03    2.41    0.05    2.41    6.48
	-------------------------------------------------------------------------
Summary:	0.09    3.46    0.15    0.05    4.83    0.07    4.83   12.96
*/
void ProcessVZMemCheck(unsigned uContainer, unsigned uNode)
{
	if(!guRunMemCheck) return;
	if(guLogLevel>3)
		logfileLine0("ProcessVZMemCheck","start",uContainer);

	FILE *fp;
	char cLine[256];
	char cContainerTag[64];
	float fLowMemUtil,fLowMemCommit,fRAMUtil,fMemSwapUtil;
	float fMemSwapCommit,fAllocUtil,fAllocCommit,fAllocLimit;
	char cResource[64]={"vzmemcheck"};
	unsigned uType=0;//tProperty type
	unsigned uKey=0;//tProperty key

	if(uContainer)
	{
		//debug only
		//printf("\tProcessVZMemCheck(Container=%u)\n",uContainer);
		sprintf(cContainerTag,"%u",uContainer);
		uType=3;
		uKey=uContainer;
	}
	else if(uNode)
	{
		//debug only
		//printf("\tProcessVZMemCheck(uNode=%u)\n",uNode);
		sprintf(cContainerTag,"Summary:");
		uType=2;
		uKey=uNode;
		guContainerOwner=guNodeOwner;
	}
	else
	{
		logfileLine0("ProcessVZMemCheck","No container or node specified",uContainer);
		exit(1);
	}
		

	if((fp=popen("/usr/sbin/vzmemcheck -v","r")))
	{
		while(fgets(cLine,255,fp)!=NULL)
		{
			fLowMemUtil=0.0;
			fLowMemCommit=0.0;
			fRAMUtil=0.0;
			fMemSwapUtil=0.0;
			fMemSwapCommit=0.0;
			fAllocUtil=0.0;
			fAllocCommit=0.0;
			fAllocLimit=0.0;

			if(strncmp(cLine,cContainerTag,strlen(cContainerTag)))
					continue;

			sscanf(cLine,"%*s %f %f %f %f %f %f %f %f",
				&fLowMemUtil,&fLowMemCommit,&fRAMUtil,&fMemSwapUtil,
				&fMemSwapCommit,&fAllocUtil,&fAllocCommit,&fAllocLimit);

        		MYSQL_RES *res;
        		MYSQL_ROW field;
			register int i;
			char cKnownVZVals[8][32]={"fLowMemUtil","fLowMemCommit","fRAMUtil","fMemSwapUtil",
							"fMemSwapCommit","fAllocUtil","fAllocCommit","fAllocLimit"};
			float fKnownVZVals[8];

			fKnownVZVals[0]=fLowMemUtil;
			fKnownVZVals[1]=fLowMemCommit;
			fKnownVZVals[2]=fRAMUtil;
			fKnownVZVals[3]=fMemSwapUtil;
			fKnownVZVals[4]=fMemSwapCommit;
			fKnownVZVals[5]=fAllocUtil;
			fKnownVZVals[6]=fAllocCommit;
			fKnownVZVals[7]=fAllocLimit;

			for(i=0;i<8;i++)
			{
				sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='%.63s.%.32s'"
							" AND uKey=%u AND uType=%u",
					cResource,cKnownVZVals[i],uKey,uType);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessVZMemCheck",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
			       	res=mysql_store_result(&gMysqlUBC);
				if((field=mysql_fetch_row(res)))
				{
					sprintf(gcQuery,"UPDATE tProperty SET cValue=%2.2f,"
							"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
							" cName='%.63s.%.32s' AND uProperty=%s"
								,fKnownVZVals[i]
								,guContainerOwner
								,cResource
								,cKnownVZVals[i]
								,field[0]);
					mysql_query(&gMysqlUBC,gcQuery);
					if(mysql_errno(&gMysqlUBC))
					{
						logfileLine0("ProcessVZMemCheck",mysql_error(&gMysqlUBC),uContainer);
						exit(2);
					}
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%2.2f"
							",cName='%.63s.%.32s'"
							",uType=%u"
							",uKey=%u"
							",uOwner=%u"
							",uCreatedBy=1"
							",uCreatedDate=UNIX_TIMESTAMP(NOW())"
								,fKnownVZVals[i]
								,cResource
								,cKnownVZVals[i]
								,uType
								,uKey
								,guContainerOwner);
					mysql_query(&gMysqlUBC,gcQuery);
					if(mysql_errno(&gMysqlUBC))
					{
						logfileLine0("ProcessVZMemCheck",mysql_error(&gMysqlUBC),uContainer);
						exit(2);
					}
				}
				mysql_free_result(res);
			}
		}
		fclose(fp);
	}
	else
	{
		logfileLine0("ProcessVZMemCheck","popen() failed",uContainer);
	}

}//void ProcessVZMemCheck(unsigned uContainer, unsigned uNode)


/*
VEID            CPUUNITS
-------------------------
0               1000
311             1000
281             1000
Current CPU utilization: 3000
Power of the node: 184838
*/
void ProcessVZCPUCheck(unsigned uContainer, unsigned uNode)
{
	if(!guRunCPUCheck) return;
	if(guLogLevel>3)
		logfileLine0("ProcessVZCPUCheck","start",uContainer);

	FILE *fp;
	char cLine[256];
	char cContainerTag[64];
	float fCPUUnits;
	char cResource[64]={"vzcpucheck"};
	unsigned uType=0;//tProperty type
	unsigned uKey=0;//tProperty key

	if(uContainer)
	{
		//debug only
		//printf("\tProcessVZCPUCheck(Container=%u)\n",uContainer);
		sprintf(cContainerTag,"%u",uContainer);
		uType=3;
		uKey=uContainer;
	}
	else if(uNode)
	{
		//debug only
		//printf("\tProcessVZCPUCheck(uNode=%u)\n",uNode);
		sprintf(cContainerTag,"0");
		uType=2;
		uKey=uNode;
		guContainerOwner=guNodeOwner;
	}
	else
	{
		logfileLine0("ProcessVZCPUCheck","No container or node specified",uContainer);
		exit(1);
	}
		

	if((fp=popen("/usr/sbin/vzcpucheck -v","r")))
	{
		unsigned uPower=0;
		while(fgets(cLine,255,fp)!=NULL)
		{
			fCPUUnits=0.0;

			if(strncmp(cLine,cContainerTag,strlen(cContainerTag)))
					continue;
			if(uPower)
				sscanf(cLine,"Power of the node: %f",&fCPUUnits);
			else
				sscanf(cLine,"%*s %f",&fCPUUnits);

        		MYSQL_RES *res;
        		MYSQL_ROW field;
			register int i;
			char cKnownVZVals[8][32]={"fCPUUnits","","","","","","",""};
			float fKnownVZVals[8];

			fKnownVZVals[0]=fCPUUnits;

			for(i=0;i<1;i++)
			{
				sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='%.63s.%.32s'"
							" AND uKey=%u AND uType=%u",
					cResource,cKnownVZVals[i],uKey,uType);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessVZCPUCheck",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
			       	res=mysql_store_result(&gMysqlUBC);
				if((field=mysql_fetch_row(res)))
				{
					sprintf(gcQuery,"UPDATE tProperty SET cValue=%2.2f,"
							"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
							" cName='%.63s.%.32s' AND uProperty=%s"
								,fKnownVZVals[i]
								,guContainerOwner
								,cResource
								,cKnownVZVals[i]
								,field[0]);
					mysql_query(&gMysqlUBC,gcQuery);
					if(mysql_errno(&gMysqlUBC))
					{
						logfileLine0("ProcessVZCPUCheck",mysql_error(&gMysqlUBC),uContainer);
						exit(2);
					}
				}
				else
				{
					sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%2.2f"
							",cName='%.63s.%.32s'"
							",uType=%u"
							",uKey=%u"
							",uOwner=%u"
							",uCreatedBy=1"
							",uCreatedDate=UNIX_TIMESTAMP(NOW())"
								,fKnownVZVals[i]
								,cResource
								,cKnownVZVals[i]
								,uType
								,uKey
								,guContainerOwner);
					mysql_query(&gMysqlUBC,gcQuery);
					if(mysql_errno(&gMysqlUBC))
					{
						logfileLine0("ProcessVZCPUCheck",mysql_error(&gMysqlUBC),uContainer);
						exit(2);
					}
				}
				mysql_free_result(res);
			}
			//Special dual line for node
			if(uNode)
			{
				uPower=1;
				sprintf(cContainerTag,"Power of the node:");
				sprintf(cResource,"vzcpucheck-nodepwr");
			}
		}
		fclose(fp);
	}
	else
	{
		logfileLine0("ProcessVZCPUCheck","popen() failed",uContainer);
	}
}//void ProcessVZCPUCheck(unsigned uContainer, unsigned uNode)


void UpdateContainerUBCJob(unsigned uContainer, char *cResource)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uDatacenter=0;
	unsigned uNode=0;

	if(!uContainer)
		return;

	sprintf(gcQuery,"SELECT uDatacenter,uNode FROM tContainer WHERE uContainer=%u",uContainer);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine0("UpdateContainerUBCJob",mysql_error(&gMysql),uContainer);
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uDatacenter);
		sscanf(field[1],"%u",&uNode);
	}
	mysql_free_result(res);

	if(!uDatacenter || !uNode)
	{
		logfileLine0("UpdateContainerUBCJob","!uDatacenter || !uNode",uContainer);
		return;
	}

	//Only one waiting job per resource please
	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uContainer=%u"
				" AND uNode=%u AND uDatacenter=%u"
				" AND uJobStatus=1"
				" AND cLabel='UpdateContainerUBCJob()'"
				" AND cJobData='cResource=%s;'"
					,uContainer,uNode,uDatacenter,cResource);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine0("UpdateContainerUBCJob",mysql_error(&gMysql),uContainer);
		exit(2);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		mysql_free_result(res);
		return;
	}
	mysql_free_result(res);
	
	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='UpdateContainerUBCJob()',cJobName='UpdateContainerUBCJob'"
			",uDatacenter=%u,uNode=%u,uContainer=%u"
			",uJobDate=UNIX_TIMESTAMP(NOW())+10"
			",uJobStatus=1"
			",cJobData='cResource=%s;'"
			",uOwner=%u,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uDatacenter,uNode,uContainer,cResource,guContainerOwner);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine0("UpdateContainerUBCJob",mysql_error(&gMysql),uContainer);
		exit(2);
	}

	sprintf(gcQuery,"uDatacenter=%u,uNode=%u,uContainer=%u\ncResource=%s",uDatacenter,uNode,uContainer,cResource);
	SendEmail("UpdateContainerUBCJob",gcQuery);

}//void UpdateContainerUBCJob(...)


//Not uVETH=1 compatible function
void ProcessSingleTraffic(unsigned uContainer)
{
	if(!guRunTraffic) return;
	if(guLogLevel>3)
		logfileLine0("ProcessSingleTraffic","start",uContainer);

	char cCommand[128];
	char cLine[512];
	unsigned long luIn=0;
	unsigned long luOut=0;
	FILE *fp;

	if(!uContainer)
	{
		logfileLine0("ProcessSingleTraffic","No container specified",uContainer);
		return;
	}
		
	sprintf(cCommand,"/usr/sbin/vzctl exec %u \"grep venet0 /proc/net/dev\" 2> /dev/null",uContainer);
	if((fp=popen(cCommand,"r")))
	{
        	MYSQL_RES *res;
        	MYSQL_ROW field;

		if(fgets(cLine,512,fp)!=NULL)
		{
//Current format
//        1          2       3    4    5     6          7         8  9          10      11   12   13    14      15         16
//venet0: 1053860    5317    0    0    0     0          0         0  9780415    7987    0    0    0     0       0          0
			if(sscanf(cLine,
				"venet0: %lu %*u %*u %*u %*u %*u %*u %*u %lu %*u %*u %*u %*u %*u %*u %*u",
					&luIn,&luOut)!=2)
			{
				logfileLine0("ProcessSingleTraffic","sscanf failed",uContainer);
				//debug only
				printf("sscanf failed\n");
				return;
			}

		}
		else
		{
			logfileLine0("ProcessSingleTraffic","No lines from popen",uContainer);
			return;
		}


		//1-. IN Bytes
		sprintf(gcQuery,"SELECT uProperty,uModDate,UNIX_TIMESTAMP(NOW()) FROM tProperty WHERE cName='Venet0.luInDelta'"
							" AND uKey=%u AND uType=3",uContainer);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
		{
			logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
			exit(2);
		}
	       	res=mysql_store_result(&gMysqlUBC);
		if((field=mysql_fetch_row(res)))
		{
			long unsigned luNewInDelta=0;
			long unsigned luPrevIn=0;
			long unsigned luDeltaModDate=0;
			long unsigned luNowDate= -1;
			unsigned uProperty=0;
        		MYSQL_RES *res2;
        		MYSQL_ROW field2;

			//New delta is based on current traffic counter and previous Venet0.luOut counter
			sprintf(gcQuery,"SELECT cValue,uProperty FROM tProperty WHERE cName='Venet0.luIn'"
							" AND uKey=%u AND uType=3",uContainer);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}
			res2=mysql_store_result(&gMysqlUBC);
			if((field2=mysql_fetch_row(res2)))
			{
				sscanf(field2[0],"%lu",&luPrevIn);
				sscanf(field2[1],"%u",&uProperty);
			}
			mysql_free_result(res2);

			if(luIn>luPrevIn)
				luNewInDelta=luIn-luPrevIn;
			else
				luNewInDelta=0;

			//debug only
			//printf("luNewInDelta=%lu = luIn=%lu - luPrevIn=%lu\n",luNewInDelta,luIn,luPrevIn);

			//Update delta
			sscanf(field[1],"%lu",&luDeltaModDate);
			sscanf(field[2],"%lu",&luNowDate);
			//Calculate traffic per second
			luNewInDelta=(luNewInDelta/(luNowDate-luDeltaModDate));
			sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
					"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
					" uProperty=%s"
							,luNewInDelta
							,guContainerOwner
							,field[0]);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}
			//Update traffic counter
			sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
					"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
					" uProperty=%u"
							,luIn
							,guContainerOwner
							,uProperty);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}

			//
			//New Maximums section
			//
			long unsigned luMaxDailyInDelta=0;
			unsigned uMaxProperty=0;
			long unsigned luMaxCreatedDate= -1;

			//Daily
			//
			sprintf(gcQuery,"SELECT cValue,uProperty,uCreatedDate"
					" FROM tProperty WHERE cName='Venet0.luMaxDailyInDelta'"
							" AND uKey=%u AND uType=3",uContainer);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}
			res2=mysql_store_result(&gMysqlUBC);
			if((field2=mysql_fetch_row(res2)))
			{
				sscanf(field2[0],"%lu",&luMaxDailyInDelta);
				sscanf(field2[1],"%u",&uMaxProperty);
				sscanf(field2[2],"%lu",&luMaxCreatedDate);
			}
			else
			{

				//First sample daily max delta is 0
				//DATE(NOW()) for time stamp at 0 hours
				sprintf(gcQuery,"INSERT INTO tProperty SET cValue=0"
						",cName='Venet0.luMaxDailyInDelta'"
						",uType=3"
						",uKey=%u"
						",uOwner=%u"
						",uCreatedBy=1"
						",uCreatedDate=UNIX_TIMESTAMP(DATE(NOW()))"
							,uContainer
							,guContainerOwner);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
				uMaxProperty=mysql_insert_id(&gMysqlUBC);
			}
			mysql_free_result(res2);

			//Day is always from today 0 hours
			//So after 00:00:00 we lose the max for "yesterday."
			//This limits the usefulness, but soon we will have MaxWeekly 
			//and MaxMonthly that will help make this a minor issue.
			if((luNowDate-(24*3600))>luMaxCreatedDate)
			{
				//DATE(NOW()) for time stamp at 0 hours
				sprintf(gcQuery,"UPDATE tProperty SET cValue=0,"
					"uCreatedDate=UNIX_TIMESTAMP(DATE(NOW())),uModBy=1,uOwner=%u WHERE"
					" uProperty=%u"
							,guContainerOwner
							,uMaxProperty);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
				luMaxDailyInDelta=0;
			}

			if(luMaxDailyInDelta<luNewInDelta)
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
					"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
					" uProperty=%u"
							,luNewInDelta
							,guContainerOwner
							,uMaxProperty);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
			}
		}
		else
		{
			//First sample delta is 0
			sprintf(gcQuery,"INSERT INTO tProperty SET cValue=0"
					",cName='Venet0.luInDelta'"
					",uType=3"
					",uKey=%u"
					",uOwner=%u"
					",uCreatedBy=1"
					",uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,uContainer
						,guContainerOwner);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}

			//First sample
			sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
					",cName='Venet0.luIn'"
					",uType=3"
					",uKey=%u"
					",uOwner=%u"
					",uCreatedBy=1"
					",uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,luIn
						,uContainer
						,guContainerOwner);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}
		}
		mysql_free_result(res);

		//2-. OUT Bytes
		sprintf(gcQuery,"SELECT uProperty,uModDate,UNIX_TIMESTAMP(NOW()) FROM tProperty WHERE cName='Venet0.luOutDelta'"
							" AND uKey=%u AND uType=3",uContainer);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
		{
			logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
			exit(2);
		}
	       	res=mysql_store_result(&gMysqlUBC);
		if((field=mysql_fetch_row(res)))
		{
			long unsigned luNewOutDelta=0;
			long unsigned luPrevOut=0;
			long unsigned luNowDate= -1;
			long unsigned luDeltaModDate=0;
			unsigned uProperty=0;
        		MYSQL_RES *res2;
        		MYSQL_ROW field2;

			//New delta is based on current traffic counter and previous Venet0.luOut counter
			sprintf(gcQuery,"SELECT cValue,uProperty FROM tProperty WHERE cName='Venet0.luOut'"
							" AND uKey=%u AND uType=3",uContainer);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}
			res2=mysql_store_result(&gMysqlUBC);
			if((field2=mysql_fetch_row(res2)))
			{
				sscanf(field2[0],"%lu",&luPrevOut);
				sscanf(field2[1],"%u",&uProperty);
			}
			mysql_free_result(res2);

			if(luOut>luPrevOut)
				luNewOutDelta=luOut-luPrevOut;
			else
				luNewOutDelta=0;

			//debug only
			//printf("luNewOutDelta=%lu = luOut=%lu  - luPrevOut=%lu\n",luNewOutDelta,luOut,luPrevOut);

			//Update delta
			sscanf(field[1],"%lu",&luDeltaModDate);
			sscanf(field[2],"%lu",&luNowDate);
			luNewOutDelta=(luNewOutDelta/(luNowDate-luDeltaModDate));
			//Calculate traffic per second
			sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
					"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
					" uProperty=%s"
							,luNewOutDelta
							,guContainerOwner
							,field[0]);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}
			//Update traffic counter
			sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
					"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
					" uProperty=%u"
							,luOut
							,guContainerOwner
							,uProperty);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}

			//
			//New Maximums section
			//
			long unsigned luMaxDailyOutDelta=0;
			unsigned uMaxProperty=0;
			long unsigned luMaxCreatedDate= -1;

			//Daily
			//
			sprintf(gcQuery,"SELECT cValue,uProperty,uCreatedDate"
					" FROM tProperty WHERE cName='Venet0.luMaxDailyOutDelta'"
							" AND uKey=%u AND uType=3",uContainer);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}
			res2=mysql_store_result(&gMysqlUBC);
			if((field2=mysql_fetch_row(res2)))
			{
				sscanf(field2[0],"%lu",&luMaxDailyOutDelta);
				sscanf(field2[1],"%u",&uMaxProperty);
				sscanf(field2[2],"%lu",&luMaxCreatedDate);
			}
			else
			{

				//First sample daily max delta is 0
				sprintf(gcQuery,"INSERT INTO tProperty SET cValue=0"
						",cName='Venet0.luMaxDailyOutDelta'"
						",uType=3"
						",uKey=%u"
						",uOwner=%u"
						",uCreatedBy=1"
						",uCreatedDate=UNIX_TIMESTAMP(DATE(NOW()))"
							,uContainer
							,guContainerOwner);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
				uMaxProperty=mysql_insert_id(&gMysqlUBC);
			}
			mysql_free_result(res2);

			if((luNowDate-(24*3600))>luMaxCreatedDate)
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue=0,"
					"uCreatedDate=UNIX_TIMESTAMP(DATE(NOW())),uModBy=1,uOwner=%u WHERE"
					" uProperty=%u"
							,guContainerOwner
							,uMaxProperty);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
				luMaxDailyOutDelta=0;
			}

			if(luMaxDailyOutDelta<luNewOutDelta)
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue=%lu,"
					"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
					" uProperty=%u"
							,luNewOutDelta
							,guContainerOwner
							,uMaxProperty);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
			}
		}
		else
		{
			//First sample delta is 0
			sprintf(gcQuery,"INSERT INTO tProperty SET cValue=0"
					",cName='Venet0.luOutDelta'"
					",uType=3"
					",uKey=%u"
					",uOwner=%u"
					",uCreatedBy=1"
					",uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,uContainer
						,guContainerOwner);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}

			//First sample
			sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%lu"
					",cName='Venet0.luOut'"
					",uType=3"
					",uKey=%u"
					",uOwner=%u"
					",uCreatedBy=1"
					",uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,luOut
						,uContainer
						,guContainerOwner);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleTraffic",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}
		}
		mysql_free_result(res);

		pclose(fp);
	}
	else
	{
		logfileLine0("ProcessSingleTraffic","popen() failed",uContainer);
	}

	//debug only
	//logfileLine0("ProcessSingleTraffic","End",uContainer);

}//void ProcessSingleTraffic(unsigned uContainer)


void ProcessSingleStatus(unsigned uContainer)
{
	if(!guRunStatus) return;
	if(guLogLevel>3)
		logfileLine0("ProcessSingleStatus","start",uContainer);

	FILE *fp;

	if(uContainer)
	{
		;
	}
	else
	{
		logfileLine0("ProcessSingleStatus","No container specified",uContainer);
		exit(1);
	}
		

	//[root@node2vm ~]# cat /proc/vz/veinfo
	// uVEID uNotKnown uProcesses cIP
	//	391     0     6   192.168.22.22
	//	0     0    66

	if((fp=fopen("/proc/vz/veinfo","r")))
	{
        	MYSQL_RES *res;
        	MYSQL_ROW field;
		char cLine[1024];
		char cIP[32];
		unsigned uProcesses;
		unsigned uVEID;

		//0-. Zero process vals
		sprintf(gcQuery,"UPDATE tProperty SET cValue='0' WHERE cName='veinfo.uProcesses'"
							" AND uKey=%u AND uType=3",uContainer);
		mysql_query(&gMysqlUBC,gcQuery);
		if(mysql_errno(&gMysqlUBC))
		{
			logfileLine0("ProcessSingleStatus",mysql_error(&gMysqlUBC),uContainer);
			exit(2);
		}

		while(fgets(cLine,1024,fp)!=NULL)
		{
			cIP[0]=0;
			uProcesses=0;
			uVEID=0;

			sscanf(cLine,"%u %*u %u %s",&uVEID,&uProcesses,cIP);

			if(uVEID!=uContainer)
				continue;

			//debug only
			//printf("uContainer=%u uVEID=%u uProcesses=%u cIP=%s\n",
			//		uContainer,uVEID,uProcesses,cIP);

			//1-. veinfo.uProcesses
			sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='veinfo.uProcesses'"
							" AND uKey=%u AND uType=3",uContainer);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleStatus",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}
			res=mysql_store_result(&gMysqlUBC);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue=%u,"
						"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
						" uProperty=%s"
							,uProcesses
							,guContainerOwner
							,field[0]);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessSingleStatus",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cValue=%u"
						",cName='veinfo.uProcesses'"
						",uType=3"
						",uKey=%u"
						",uOwner=%u"
						",uCreatedBy=1"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,uProcesses
							,uContainer
							,guContainerOwner);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessSingleStatus",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
			}
			mysql_free_result(res);

	
			//2-. veinfo.cIP
			sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE cName='veinfo.cIP'"
							" AND uKey=%u AND uType=3",uContainer);
			mysql_query(&gMysqlUBC,gcQuery);
			if(mysql_errno(&gMysqlUBC))
			{
				logfileLine0("ProcessSingleStatus",mysql_error(&gMysqlUBC),uContainer);
				exit(2);
			}
			res=mysql_store_result(&gMysqlUBC);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gcQuery,"UPDATE tProperty SET cValue='%s',"
						"uModDate=UNIX_TIMESTAMP(NOW()),uModBy=1,uOwner=%u WHERE"
						" uProperty=%s"
							,cIP
							,guContainerOwner
							,field[0]);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessSingleStatus",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
			}
			else
			{
				sprintf(gcQuery,"INSERT INTO tProperty SET cValue='%s'"
						",cName='veinfo.cIP'"
						",uType=3"
						",uKey=%u"
						",uOwner=%u"
						",uCreatedBy=1"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,cIP
							,uContainer
							,guContainerOwner);
				mysql_query(&gMysqlUBC,gcQuery);
				if(mysql_errno(&gMysqlUBC))
				{
					logfileLine0("ProcessSingleStatus",mysql_error(&gMysqlUBC),uContainer);
					exit(2);
				}
			}
			mysql_free_result(res);
		}
	}
	else
	{
		logfileLine0("ProcessSingleStatus","fopen() failed",uContainer);
	}

}//void ProcessSingleStatus(unsigned uContainer)


void SendEmail(char *cSubject,char *cMsg)
{
	FILE *fp;
	char cFrom[100]={"ubc-agent"};
	char cEmail[100]={"supportgrp@unixservice.com"};

	if((fp=popen("/usr/lib/sendmail -t > /dev/null","w")))
	{
		fprintf(fp,"To: %s\n",cEmail);
		fprintf(fp,"From: %s\n",cFrom);
		fprintf(fp,"Subject: %s\n\n",cSubject);
		fprintf(fp,"%s\n",cMsg);
	}
	pclose(fp);

}//void SendEmail()


void UBCConnectToOptionalUBCDb(unsigned uDatacenter)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	//UBC MySQL server per datacenter option. Get db IPs
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u"
				" AND uType=1"
				" AND cName='gcUBCDBIP0'"
						,uDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine0("UBCConnectToOptionalUBCDb",mysql_error(&gMysql),uDatacenter);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		unsigned uA=0,uB=0,uC=0,uD=0;
		if(sscanf(field[0],"%u.%u.%u.%u",&uA,&uB,&uC,&uD)==4)
		{
			sprintf(gcUBCDBIP0Buffer,"%u.%u.%u.%u",uA,uB,uC,uD);
			gcUBCDBIP0=gcUBCDBIP0Buffer;
			logfileLine0("UBCConnectToOptionalUBCDb",gcUBCDBIP0Buffer,uDatacenter);
		}
	}
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u"
				" AND uType=1"
				" AND cName='gcUBCDBIP1'"
						,uDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine0("UBCConnectToOptionalUBCDb",mysql_error(&gMysql),uDatacenter);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		unsigned uA=0,uB=0,uC=0,uD=0;
		if(sscanf(field[0],"%u.%u.%u.%u",&uA,&uB,&uC,&uD)==4)
		{
			sprintf(gcUBCDBIP1Buffer,"%u.%u.%u.%u",uA,uB,uC,uD);
			gcUBCDBIP1=gcUBCDBIP1Buffer;
			logfileLine0("UBCConnectToOptionalUBCDb",gcUBCDBIP1Buffer,uDatacenter);
		}
	}
	//If gcUBCDBIP1 or gcUBCDBIP1 exist then we will use another MySQL db for UBC tProperty
	//	data
	TextConnectDbUBC();

}//void UBCConnectToOptionalUBCDb()

