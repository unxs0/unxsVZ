/*
FILE
	openvz.c
	$Id: openvz.c 2074 2012-09-02 02:38:13Z Colin $
PURPOSE
	Collection of OpenVZ container information.
	Initially to reconcile unxsVZ container data with actual deployed
	OpenVZ VEs.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2008-2010.
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include "openvz.h"
#include <sys/sysinfo.h>

#define cOVZLOGFILE "/var/log/unxsOVZ.log"

MYSQL gMysql;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};
char gcProgram[32]={""};
unsigned guNodeOwner=0;

//local protos
void logfileLine(const char *cFunction,const char *cLogline,const unsigned uContainer);
void ProcessNodeOVZ(void);
void ProcessIPCheck(unsigned uNode);
//external protos
void TextConnectDb(void);

unsigned guLogLevel=3;
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

		fprintf(gLfp,"%s unxsOVZ.%s[%u]: %s. uContainer=%u\n",cTime,cFunction,pidThis,cLogline,uContainer);
		fflush(gLfp);
	}

}//void logfileLine()

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
			if(!strcmp(cArgv[i],"--help"))
			{
				printf("usage: %s [--help] [--version]\n",cArgv[0]);
				exit(0);
			}
			if(!strcmp(cArgv[i],"--version"))
			{
				printf("version: %s $Id: openvz.c 2074 2012-09-02 02:38:13Z Colin $\n",cArgv[0]);
				exit(0);
			}
		}
	}

	char cLockfile[64]={"/tmp/openvz.lock"};

	sprintf(gcProgram,"%.31s",cArgv[0]);
	if((gLfp=fopen(cOVZLOGFILE,"a"))==NULL)
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
	if(!stat(cLockfile,&structStat))
	{
		logfileLine("main","waiting for rmdir(cLockfile)",0);
		return(1);
	}
	if(mkdir(cLockfile,S_IRUSR|S_IWUSR|S_IXUSR))
	{
		logfileLine("main","could not open cLockfile dir",0);
		return(1);
	}

	ProcessNodeOVZ();

	if(rmdir(cLockfile))
	{
		logfileLine("main","could not rmdir(cLockfile)",0);
		return(1);
	}
	logfileLine("main","end",0);
	return(0);
}//main()


void ProcessNodeOVZ(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uDatacenter=0;
	unsigned uNode=0;

	if(gethostname(cHostname,99)!=0)
	{
		logfileLine("ProcessNodeOVZ","gethostname() failed",0);
		exit(1);
	}

	//Uses login data from local.h
	TextConnectDb();
	guLoginClient=1;//Root user

	sprintf(gcQuery,"SELECT uNode,uDatacenter,uOwner FROM tNode WHERE cLabel='%.99s'",cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessNodeOVZ",mysql_error(&gMysql),0);
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
			logfileLine("ProcessNodeOVZ",mysql_error(&gMysql),0);
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
		logfileLine("ProcessNodeOVZ","Could not determine uNode",0);
		mysql_close(&gMysql);
		exit(1);
	}


	//Process  node
	if(guLogLevel>2)
		logfileLine("ProcessIPCheck","node start",uNode);
	ProcessIPCheck(uNode);

	if(guLogLevel>2)
		logfileLine("ProcessNodeOVZ","end",uNode);

	mysql_close(&gMysql);

}//void ProcessNodeOVZ(void)


void ProcessIPCheck(unsigned uNode)
{
	char cCommand[64];
	char cLine[256];
	FILE *fp;

	sprintf(cCommand,"/usr/sbin/vzlist -H -a -o veid,ip,hostname 2> /dev/null");
	if((fp=popen(cCommand,"r")))
	{
		while(fgets(cLine,255,fp)!=NULL)
		{
        		MYSQL_RES *res;
        		MYSQL_ROW field;
			char cHostname[100]={""};

			unsigned uVEID=0,uA=0,uB=0,uC=0,uD=0;

			if(sscanf(cLine,"%u %u.%u.%u.%u %s",&uVEID,&uA,&uB,&uC,&uD,cHostname)!=6)
			{
				logfileLine("ProcessIPCheck","sscanf item count error",uVEID);
			}

			char cIP[32]={""};
			sprintf(cIP,"%u.%u.%u.%u",uA,uB,uC,uD);
			//debug only
			//printf("%u %s\n",uVEID,cIP);
	
			//Compare against unxsVZ data
			//If VEID exists
			//report if different into tProperty cOrg_Warning_IPNumber <ip>
			//if VEID does not exist report in log file only.
			sprintf(gcQuery,"SELECT tIP.cLabel FROM tContainer,tIP WHERE uContainer=%u AND tContainer.uIPv4=tIP.uIP",uVEID);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				//debug only
				printf("%s\n",mysql_error(&gMysql));
				logfileLine("ProcessIPCheck",mysql_error(&gMysql),uVEID);
				mysql_close(&gMysql);
				exit(2);
			}
		        res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)>0)
			{
				while((field=mysql_fetch_row(res)))
				{
					if(strcmp(field[0],cIP))
					{
						//debug only
						printf("%u:%s:%s diff\n",uVEID,field[0],cHostname);
						logfileLine("ProcessIPCheck diff",field[0],uVEID);
					}
				}
			}
			else
			{
				//debug only
				printf("%u:%s:%s nf\n",uVEID,cIP,cHostname);
				logfileLine("ProcessIPCheck nf",cIP,uVEID);

        			MYSQL_RES *res;
        			MYSQL_ROW field;
				sprintf(gcQuery,"SELECT tContainer.cLabel,tIP.uIP,tIP.uAvailable"
						" FROM tContainer,tIP WHERE tIP.cLabel='%s' AND tContainer.uIPv4=tIP.uIP",cIP);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					//debug only
					printf("%s\n",mysql_error(&gMysql));
					logfileLine("ProcessIPCheck",mysql_error(&gMysql),uVEID);
					mysql_close(&gMysql);
					exit(2);
				}
			        res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)>0)
				{

					while((field=mysql_fetch_row(res)))
					{
						//debug only
						printf("%u:%s:%s:%s:uAvailable=%s found in tIP\n",uVEID,field[0],cHostname,field[1],field[2]);
					}
				}
				else
				{
					mysql_free_result(res);
					sprintf(gcQuery,"SELECT tIP.uIP,tIP.uAvailable FROM tIP WHERE cLabel='%s'",cIP);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
					{
						//debug only
						printf("%s\n",mysql_error(&gMysql));
						logfileLine("ProcessIPCheck",mysql_error(&gMysql),uVEID);
						mysql_close(&gMysql);
						exit(2);
					}
			        	res=mysql_store_result(&gMysql);
					while((field=mysql_fetch_row(res)))
					{
						//debug only
						printf("%u:%s:%s:%s:uAvailable=%s found in tIP no container\n",uVEID,cIP,cHostname,field[0],field[1]);
					}
				}
				mysql_free_result(res);
			}
			mysql_free_result(res);
		}//while lines from vzlist
		pclose(fp);
	}
	else
	{
		logfileLine("ProcessIPCheck","popen() failed",0);
	}

}//void ProcessIPCheck(unsigned uNode)

