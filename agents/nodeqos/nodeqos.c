/*
FILE
	nodeqos.c
	$Id$
PURPOSE
	Collection of Asterisk PBX QOS data from containers running on a hardware node.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2012-2013.
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include "../../mysqlrad.h"
#include <sys/sysinfo.h>
#include <ctype.h>
#include <math.h>

#define cPbxQOSLOGFILE "/var/log/PbxQOS.log"

MYSQL gMysql;
char gcQuery[8192]={""};
char gcHostname[100]={""};
char gcProgram[100]={""};
unsigned guDebug=0;
unsigned guRRDTool=0;
unsigned guVEID=0;
unsigned guNumCalls=0;

//report data
float fLossRecvMax=0.0;
float fLossSendMax=0.0;
float fJitterRecvMax=0.0;
float fJitterSendMax=0.0;
float fLossRecvMin=0.0;
float fLossSendMin=0.0;
float fJitterRecvMin=0.0;
float fJitterSendMin=0.0;
float fLossRecvAvg=0.0;
float fLossSendAvg=0.0;
float fJitterRecvAvg=0.0;
float fJitterSendAvg=0.0;
float fLossRecvAvgX2=0.0;
float fLossSendAvgX2=0.0;
float fJitterRecvAvgX2=0.0;
float fJitterSendAvgX2=0.0;
float fLossRecvStd=0.0;
float fLossSendStd=0.0;
float fJitterRecvStd=0.0;
float fJitterSendStd=0.0;

//local protos
void TextConnectDb(void);
void ProcessQOS(void);
void ProcessSingleQOS(unsigned uContainer);

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

		fprintf(gLfp,"%s PbxQOS.%s[%u]: %s. uContainer=%u\n",cTime,cFunction,pidThis,cLogline,uContainer);
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
			if(!strcmp(cArgv[i],"--rrdtool"))
				guRRDTool=1;
			if(!strcmp(cArgv[i],"--debug"))
				guDebug=1;
			if(!strncmp(cArgv[i],"--veid",6))
				sscanf(cArgv[i],"--veid=%u",&guVEID);
			if(!strcmp(cArgv[i],"--help"))
			{
				printf("usage: %s [--help] [--version] [--veid=VEID] [--rrdtool] [--debug]\n",cArgv[0]);
				exit(0);
			}
			if(!strcmp(cArgv[i],"--version"))
			{
				printf("version: %s $Id$\n",cArgv[0]);
				exit(0);
			}
		}
	}

	char cLockfile[64]={"/tmp/PbxQOS.lock"};

	sprintf(gcProgram,"%.31s",cArgv[0]);
	if((gLfp=fopen(cPbxQOSLOGFILE,"a"))==NULL)
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

	logfileLine("main","start",0);
	if(guVEID)
		ProcessSingleQOS(guVEID);
	else
		ProcessQOS();//For all node containers

	//report section
	if(guNumCalls>0)
	{
		fLossRecvAvg=fLossRecvAvg/guNumCalls;
		fJitterRecvAvg=fJitterRecvAvg/guNumCalls;
		fLossSendAvg=fLossSendAvg/guNumCalls;
		fJitterSendAvg=fJitterSendAvg/guNumCalls;
	}

	if(guRRDTool)
	{
		if(guDebug)
			printf("fLossSendAvg fLossRecvAvg fJitterSendAvg*1000 fJitterRecvAvg*1000 uNumCalls\n");
		printf("%.0f %.0f %.0f %.0f %u\n",
				fLossSendAvg*100,fLossRecvAvg*100,fJitterSendAvg*100,fJitterRecvAvg*100,guNumCalls);
	}
	else
	{
	
		printf("uNumCalls=%u\n",guNumCalls);
		if(guNumCalls>0)
		{
			printf("fLossRecvMin=%f fJitterRecvMin=%f fLossSendMin=%f fJitterSendMin=%f\n",
					fLossRecvMin,fJitterRecvMin,fLossSendMin,fJitterSendMin);
			printf("fLossRecvMax=%f fJitterRecvMax=%f fLossSendMax=%f fJitterSendMax=%f\n",
				fLossRecvMax,fJitterRecvMax,fLossSendMax,fJitterSendMax);
			printf("fLossRecvAvg=%f fJitterRecvAvg=%f fLossSendAvg=%f fJitterSendAvg=%f\n",
				fLossRecvAvg,fJitterRecvAvg,fLossSendAvg,fJitterSendAvg);
			printf("fLossRecvStd=%f fJitterRecvStd=%f fLossSendStd=%f fJitterSendStd=%f\n",
				fLossRecvStd,fJitterRecvStd,fLossSendStd,fJitterSendStd);
		}
	}
	//end report section

	if(rmdir(cLockfile))
	{
		logfileLine("main","could not rmdir(cLockfile)",0);
		return(1);
	}
	logfileLine("main","end",0);
	return(0);
}//main()


void ProcessQOS(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uNode=0;
	unsigned uContainer=0;

	if(gethostname(gcHostname,99)!=0)
	{
		logfileLine("ProcessQOS","gethostname() failed",uContainer);
		exit(1);
	}

	//Uses login data from local.h
	TextConnectDb();

	sprintf(gcQuery,"SELECT uNode FROM tNode WHERE cLabel='%.99s'",gcHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessQOS",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
	}
	mysql_free_result(res);
	if(!uNode)
	{
		char *cp;

		//FQDN vs short name of 2nd NIC mess
		if((cp=strchr(gcHostname,'.')))
			*cp=0;
		sprintf(gcQuery,"SELECT uNode FROM tNode WHERE cLabel='%.99s'",gcHostname);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessQOS",mysql_error(&gMysql),uContainer);
			mysql_close(&gMysql);
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uNode);
		}
		mysql_free_result(res);
	}

	if(!uNode)
	{
		logfileLine("ProcessQOS","Could not determine uNode",uContainer);
		mysql_close(&gMysql);
		exit(1);
	}

	//Main loop. TODO use defines for tStatus.uStatus values.
	sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uNode=%u AND uStatus=1",uNode);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("ProcessQOS",mysql_error(&gMysql),uContainer);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		if(sysinfo(&structSysinfo))
		{
			logfileLine("ProcessQOS","sysinfo() failed",0);
			exit(1);
		}
		if(structSysinfo.loads[1]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
		{
			logfileLine("ProcessQOS","structSysinfo.loads[1] larger than JOBQUEUE_MAXLOAD",0);
			mysql_free_result(res);
			mysql_close(&gMysql);
			return;
		}

		sscanf(field[0],"%u",&uContainer);
		ProcessSingleQOS(uContainer);

	}
	mysql_free_result(res);
	mysql_close(&gMysql);

}//void ProcessUBC(void)


void ProcessSingleQOS(unsigned uContainer)
{
	char cCommand[256];
	char cLine[256];
	FILE *fp;
	unsigned uFirst=1;

	logfileLine("ProcessSingleQOS","start",uContainer);

	sprintf(cCommand,"/usr/sbin/vzctl exec2 %u \"/usr/sbin/asterisk -rx 'sip show channelstats'\"",uContainer);
	if((fp=popen(cCommand,"r")))
	{
		while(fgets(cLine,255,fp)!=NULL)
                {
			float fLossRecv;
			float fLossSend;
			float fJitterRecv;
			float fJitterSend;
			unsigned uFields=0;
			unsigned uHrs=0;
			unsigned uMins=0;
			unsigned uSecs=0;

			if(guDebug)
				printf("%s",cLine);

			if(strstr(cLine,"Peer"))
				continue;
			if(strstr(cLine,"active SIP channels"))
				continue;
			if(strstr(cLine,"No RTP active"))
				continue;

			if(isdigit(cLine[0])&&(isdigit(cLine[1])||cLine[1]=='.'))
			{
/*
Peer             Call ID      Duration Recv: Pack  Lost       (     %) Jitter Send: Pack  Lost       (     %) Jitter
174.121.136.137  1846308249_  00:10:36 0000029748  0000000000 ( 0.00%) 0.0000 0000029756  0000000000 ( 0.00%) 0.0004
fLossRecv=0.000000 fJitterRecv=0.000000 fLossSend=0.000000 fJitterSend=0.000400
*/
				uFields=sscanf(cLine,"%*[a-zA-Z@0-9--_.: \\t]( %f%%) %f %*[a-zA-Z@0-9--_.: \\t]( %f%%) %f",
										&fLossRecv,&fJitterRecv,&fLossSend,&fJitterSend);
				char *cp;
				if((cp=strchr(cLine,':')))
					sscanf(cp-2,"%u:%u:%u",&uHrs,&uMins,&uSecs);
				if(guDebug)
					printf("uHrs=%u uMins=%u uSecs=%u fLossRecv=%f fJitterRecv=%f fLossSend=%f fJitterSend=%f\n",
						uHrs,uMins,uSecs,fLossRecv,fJitterRecv,fLossSend,fJitterSend);
					
				unsigned uCallDuration=uSecs+uMins*60+uHrs*3600;
				if(guDebug && uCallDuration<60)
					printf("Short called ignored\n");
				if(uFields==4 && uCallDuration>59)
				{
					if(uFirst)
					{
						fLossRecvMin=fLossRecv;
						fLossSendMin=fLossSend;
						fJitterRecvMin=fJitterRecv;
						fJitterSendMin=fJitterSend;
						uFirst=0;
					}
					if(fLossRecv>fLossRecvMax) fLossRecvMax=fLossRecv;
					if(fLossRecv<fLossRecvMin) fLossRecvMin=fLossRecv;
					if(fJitterRecv>fJitterRecvMax) fJitterRecvMax=fJitterRecv;
					if(fJitterRecv<fJitterRecvMin) fJitterRecvMin=fJitterRecv;
					if(fLossSend>fLossSendMax) fLossSendMax=fLossSend;
					if(fLossSend<fLossSendMin) fLossSendMin=fLossSend;
					if(fJitterSend>fJitterSendMax) fJitterSendMax=fJitterSend;
					if(fJitterSend<fJitterSendMin) fJitterSendMin=fJitterSend;

					fLossRecvAvg+=fLossRecv;
					fLossSendAvg+=fLossSend;
					fJitterRecvAvg+=fJitterRecv;
					fJitterSendAvg+=fJitterSend;

					fLossRecvAvgX2+=fLossRecv*fLossRecv;
					fLossSendAvgX2+=fLossSend*fLossSend;
					fJitterRecvAvgX2+=fJitterRecv*fJitterRecv;
					fJitterSendAvgX2+=fJitterSend*fJitterSend;

					//stdev = sqrt((sum_x2 / n) - (mean * mean))

					float fMean;

					fMean=fLossRecvAvg/guNumCalls;
					fLossRecvStd=sqrt((fLossRecvAvgX2/guNumCalls) - (fMean*fMean));

					fMean=fLossSendAvg/guNumCalls;
					fLossSendStd=sqrt((fLossSendAvgX2/guNumCalls) - (fMean*fMean));

					fMean=fJitterRecvAvg/guNumCalls;
					fJitterRecvStd=sqrt((fJitterRecvAvgX2/guNumCalls) - (fMean*fMean));

					fMean=fJitterSendAvg/guNumCalls;
					fJitterSendStd=sqrt((fJitterSendAvgX2/guNumCalls) - (fMean*fMean));

					guNumCalls++;
				}//valid data
			}//valid line
		}//while lines
		pclose(fp);
	}//popen


	logfileLine("ProcessSingleQOS","end",uContainer);

}//void ProcessSingleQOS(void)

