/*
FILE
	nodeqos.c
	svn ID removed
PURPOSE
	Collection of Asterisk PBX QOS data from containers running on a hardware node.
	Collection of tshark determined problem streams.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2012-2013.
	GPLv2 License applies. See LICENSE file.
NOTES
*/

#include "../../mysqlrad.h"
#include "local.h"
#include <sys/sysinfo.h>
#include <ctype.h>
#include <math.h>

#define cPbxQOSLOGFILE "/var/log/PbxQOS.log"

MYSQL gMysql;
char gcQuery[8192]={""};
char gcHostname[100]={""};
char gcProgram[100]={""};
char gcInterface[100]={"eth0"};
unsigned guDebug=0;
unsigned guRRDTool=0;
unsigned guTShark=0;
unsigned guNodeQOS=0;
unsigned guVEID=0;
unsigned guNumCalls=0;
unsigned guNode=0;
unsigned guOwner=1;

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
void ProcessAsteriskSingleQOS(unsigned uContainer);
void ProcessTShark(void);
void SendAlertEmail(char *cMsg);
void SendMTREmail(char *cIP,unsigned uContainer,char *cHostname,float fPacketLoss);
void ProcessNodeQOS(void);
unsigned unxsVZModule(unsigned uA,char *cSrcIP,unsigned uSrcPort,char *cDstIP,unsigned uDstPort,float fPacketLossPercent,
				unsigned uScanCount,unsigned uDidNotSendMTREmail);

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
			if(!strcmp(cArgv[i],"--tshark"))
				guTShark=1;
			if(!strcmp(cArgv[i],"--rrdtool"))
				guRRDTool=1;
			if(!strcmp(cArgv[i],"--debug"))
				guDebug=1;
			if(!strcmp(cArgv[i],"--nodeqos"))
				guNodeQOS=1;
			if(!strncmp(cArgv[i],"--veid",6))
				sscanf(cArgv[i],"--veid=%u",&guVEID);
			if(!strncmp(cArgv[i],"--owner",7))
				sscanf(cArgv[i],"--owner=%u",&guOwner);
			if(!strncmp(cArgv[i],"--interface",11))
				sscanf(cArgv[i],"--interface=%[a-z0-9\\.]",gcInterface);
			if(!strcmp(cArgv[i],"--help"))
			{
				printf("usage: %s [--help] [--version] [--tshark] [--owner=1] [--nodeqos]"
					" [--interface=eth0] [--rrdtool] [--veid=VEID] [--debug]\n",cArgv[0]);
				exit(0);
			}
			if(!strcmp(cArgv[i],"--version"))
			{
				printf("version: %s svn ID removed
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
		
	logfileLine("main","start",0);
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


        MYSQL_RES *res;
        MYSQL_ROW field;
	if(gethostname(gcHostname,99)!=0)
	{
		logfileLine("main","gethostname() failed",0);
		exit(1);
	}

	//Uses login data from local.h
	TextConnectDb();

	sprintf(gcQuery,"SELECT uNode FROM tNode WHERE cLabel='%.99s'",gcHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine("main",mysql_error(&gMysql),0);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&guNode);
	}
	mysql_free_result(res);
	if(!guNode)
	{
		char *cp;

		//FQDN vs short name of 2nd NIC mess
		if((cp=strchr(gcHostname,'.')))
			*cp=0;
		sprintf(gcQuery,"SELECT uNode FROM tNode WHERE cLabel='%.99s'",gcHostname);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("main",mysql_error(&gMysql),0);
			mysql_close(&gMysql);
			exit(2);
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&guNode);
		}
		mysql_free_result(res);
	}

	if(!guNode)
	{
		logfileLine("main","Could not determine guNode",0);
		mysql_close(&gMysql);
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

	if(guTShark)
	{
		ProcessTShark();
	}
	else if(guNodeQOS)
	{
		ProcessNodeQOS();
	}
	else if(1)
	{

		if(guVEID)
			ProcessAsteriskSingleQOS(guVEID);
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
		}//end 	report section

	}//end container based data
	

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
	unsigned uContainer=0;

	if(guDebug) printf("ProcessQOS() start\n");

	//Main loop. TODO use defines for tStatus.uStatus values.
	sprintf(gcQuery,"SELECT uContainer FROM tContainer WHERE uNode=%u AND uStatus=1",guNode);
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
		ProcessAsteriskSingleQOS(uContainer);

	}
	mysql_free_result(res);
	mysql_close(&gMysql);

	if(guDebug) printf("ProcessQOS() end\n");

}//void ProcessQOS(void)


void ProcessAsteriskSingleQOS(unsigned uContainer)
{
	char cCommand[256];
	char cLine[256];
	FILE *fp;
	unsigned uFirst=1;

	logfileLine("ProcessAsteriskSingleQOS","start",uContainer);

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
			char cCallID[256]={""};

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
				uFields=sscanf(cLine,"%*[0-9.]\t%[a-zA-Z@0-9--_]\t%*[a-zA-Z@0-9--_.: \\t]( %f%%) %f %*[a-zA-Z@0-9--_.: \\t]( %f%%) %f",
										cCallID,&fLossRecv,&fJitterRecv,&fLossSend,&fJitterSend);
				char *cp;
				if((cp=strchr(cLine,':')))
					sscanf(cp-2,"%u:%u:%u",&uHrs,&uMins,&uSecs);
				if(guDebug)
					printf("uHrs=%u uMins=%u uSecs=%u fLossRecv=%f fJitterRecv=%f fLossSend=%f fJitterSend=%f\n",
						uHrs,uMins,uSecs,fLossRecv,fJitterRecv,fLossSend,fJitterSend);
				if(guDebug)
				{
					FILE *fp;
					char cLine[256];
					printf("cCallID=(%s)\n",cCallID);
					sprintf(cCommand,"/usr/sbin/vzctl exec2 %u \"/usr/sbin/asterisk -rx 'sip show channel %s'\"",
						uContainer,cCallID);
					if((fp=popen(cCommand,"r")))
					{
						while(fgets(cLine,255,fp)!=NULL)
                				{
							if(strstr(cLine,"Received") || strstr(cLine,"Audio"))
							printf("%s",cLine);
						}
						pclose(fp);
					}
				}
					
				unsigned uCallDuration=uSecs+uMins*60+uHrs*3600;
				if(guDebug && uCallDuration<60)
					printf("Short called ignored\n");
				if(uFields==5 && uCallDuration>59)
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


	logfileLine("ProcessAsteriskSingleQOS","end",uContainer);

}//void ProcessAsteriskSingleQOS(void)


void ProcessTShark(void)
{
	logfileLine("ProcessTShark","start",0);

	unsigned uScanCount=0;
	unsigned uDidNotSendMTREmail=0;
	char cCommand[256];
	FILE *fp;
	//# /usr/sbin/tshark -i venet0 -a duration:55 -q -f 'udp portrange 16384-32768' -o rtp.heuristic_rtp:TRUE -z rtp,streams -w /tmp/qoscap2
	sprintf(cCommand,"/usr/sbin/tshark -i %s -a duration:55 -q -f 'udp portrange 16384-32768'"
				" -o rtp.heuristic_rtp:TRUE -z rtp,streams -w /tmp/qoscap2 2> /dev/null | /bin/grep X",gcInterface);
	if(guDebug) printf("%s\n",cCommand);
	if((fp=popen(cCommand,"r")))
	{
		char cLine[256];
		while(fgets(cLine,255,fp)!=NULL)
                {
			char cSrcIP[256]={""};
			char cDstIP[256]={""};
			unsigned uFields=0;
			unsigned uDstPort=0;
			unsigned uSrcPort=0;
			unsigned uA=0;
			float fPacketLossPercent=0.0;

			if(guDebug) printf("%s",cLine);
/*
   75.148.66.17 11800 174.121.136.170 19108 0x0001A372 ITU-T G.711 PCMU  2603   117 (4.3%)           64.73            4.64            1.29 X
  199.199.10.15 18058  174.121.136.11 12026 0x889F59A4 ITU-T G.711 PCMU  1407  -692 (-96.8%)           43.20          812.00            0.06 X
*/
			uFields=sscanf(cLine,"%u.%[0-9\\.]%*[ \\t]%u%*[ \\t]%[0-9\\.]%*[ \\t]%u%*[ \\t]%*[0-9A-Fx]"
						"%*[ \\t]%*[A-Z--]%*[ \\t]%*[0-9A-Z\\.]%*[ \\t]%*[A-Z]%*[ \\t]%*d%*[ \\t]%*d%*[ \\t](%f%%)",
							&uA,cSrcIP,&uSrcPort,cDstIP,&uDstPort,&fPacketLossPercent);
			if(uFields==6)
			{
				if(guDebug)
					printf("cSrcIP=%u.%s:%u cDstIP=%s:%u %2.2f\n",uA,cSrcIP,uSrcPort,cDstIP,uDstPort,fPacketLossPercent);

				//unxsVZ integration function also has per container email report code
				//	once loop is done another email report summary may need to be sent*
				unsigned uRetVal=unxsVZModule(uA,cSrcIP,uSrcPort,cDstIP,uDstPort,fPacketLossPercent,uScanCount,uDidNotSendMTREmail);
				if(uRetVal==1)
					break;
				else if(uRetVal==2)
					continue;
			}
			else
			{
				if(guDebug) printf("uFields=%u\n",uFields);
			}
		}//while lines
		pclose(fp);
	}//popen
	else
	{
		if(guDebug) printf("%s",cCommand);
	}


	//*this is also a unxsVZ integration required code block	
	logfileLine("ProcessTShark","uScanCount",uScanCount);
	if(uScanCount>10)
	{
		if(uDidNotSendMTREmail)
			sprintf(gcQuery,"!%u QOS issues and %u MTRs were skipped!",uScanCount,uDidNotSendMTREmail);
		else
			sprintf(gcQuery,"ProcessTShark has detected %u QOS issues.",uScanCount);
		SendAlertEmail(gcQuery);
	}
	logfileLine("ProcessTShark","end",0);

}//void ProcessTShark(void)


void SendAlertEmail(char *cMsg)
{
	FILE *pp;
	pid_t pidChild;

	pidChild=fork();
	if(pidChild!=0)
		return;

	pp=popen("/usr/lib/sendmail -t","w");
	if(pp==NULL)
	{
		logfileLine("SendAlertEmail","popen() /usr/lib/sendmail",0);
		return;
	}
			
	//should be defined in local.h
	fprintf(pp,"To: %s\n",cQOS_MAILTO);
	if(cQOS_BCC!=NULL)
	{
		char cBcc[512]={""};
		sprintf(cBcc,"%.511s",cQOS_BCC);
		if(cBcc[0])
			fprintf(pp,"Bcc: %s\n",cBcc);
	}
	fprintf(pp,"From: %s\n",cQOS_FROM);
	fprintf(pp,"Subject: %s %s\n",cQOS_SUBJECT,gcHostname);

	fprintf(pp,"%s\n",cMsg);

	fprintf(pp,".\n");

	pclose(pp);

	logfileLine("SendAlertEmail","email attempt ok",0);

}//void SendAlertEmail(char *cMsg)


void SendMTREmail(char *cIP,unsigned uContainer,char *cHostname,float fPacketLoss)
{
	pid_t pidChild;
	FILE *pp;
	time_t luClock;
	char cTime[32];
	const struct tm *tmTime;

	char cCommand[128];
	char cReport[2048]={""};
	unsigned uReportLen=0;
	unsigned uOneTimeOnly=0;

	pidChild=fork();
	if(pidChild!=0)
		return;

	time(&luClock);
	tmTime=localtime(&luClock);
	strftime(cTime,31,"%b %d %T",tmTime);

	uReportLen+=strlen("\nCall information:\n");
	sprintf(cCommand,"/usr/sbin/vzctl exec2 %u \"/usr/sbin/asterisk -rx 'core show channels verbose'|/bin/grep trunk\"",uContainer);
	if((pp=popen(cCommand,"r")))
	{
		char cLine[256]={""};
		while(fgets(cLine,255,pp)!=NULL)
		{
			if(!uOneTimeOnly++)
			{
				strncat(cReport,"\nCall information\n",32);
				uReportLen=strlen("\nCall information\n");
			}
			uReportLen+=strlen(cLine);
			if(uReportLen>2047) break;
			strncat(cReport,cLine,255);
		}
		pclose(pp);
	}

	if(!uOneTimeOnly)
	{
		logfileLine("SendMTREmail","email attempt aborted no call info",0);
		return;
	}

	uReportLen+=strlen("\nmtr -c 10 -r:\n");
	if(!(uReportLen>2047))
	{
		strncat(cReport,"\nmtr -c 10 -r:\n",32);
		sprintf(cCommand,"/usr/sbin/mtr -c 10 -r %.32s 2> /dev/null",cIP);
		if((pp=popen(cCommand,"r")))
		{
			char cLine[256]={""};
			while(fgets(cLine,255,pp)!=NULL)
			{
				uReportLen+=strlen(cLine);
				if(uReportLen>2047) break;
				strncat(cReport,cLine,255);
			}
			pclose(pp);
		}
	}

	pp=popen("/usr/lib/sendmail -t","w");
	if(pp==NULL)
	{
		logfileLine("SendMTREmail","popen() /usr/lib/sendmail",0);
		return;
	}
			
	//should be defined in local.h
	fprintf(pp,"To: %s\n",cQOS_MAILTO);
	if(cQOS_BCC!=NULL)
	{
		char cBcc[512]={""};
		sprintf(cBcc,"%.511s",cQOS_BCC);
		if(cBcc[0])
			fprintf(pp,"Bcc: %s\n",cBcc);
	}
	fprintf(pp,"From: %s\n",cQOS_FROM);
	fprintf(pp,"Subject: mtr report for %s from %s:%s %s\n",cIP,gcHostname,cHostname,cTime);

	fprintf(pp,"\n%s (%u) Loss:%2.2f\n%s",cHostname,uContainer,fPacketLoss,cReport);
	fprintf(pp,".\n");

	pclose(pp);

	logfileLine("SendMTREmail","email attempt ok",0);

}//void SendMTREmail()


void ProcessNodeQOS(void)
{
	logfileLine("ProcessNodeQOS","start",0);

	unsigned uScanCount=0;
	unsigned uDidNotSendMTREmail=0;
	char cCommand[256];
	FILE *fp;
	sprintf(cCommand,"/usr/sbin/tshark -i %s -a duration:55 -q -f 'udp portrange 16384-32768'"
				" -o rtp.heuristic_rtp:TRUE -z rtp,streams -w /tmp/qoscap3 2> /dev/null",gcInterface);
	if(guDebug) printf("%s\n",cCommand);
	if((fp=popen(cCommand,"r")))
	{
		float fAvgPacketLossPercent=0.0;
		float fAvgMaxDelta=0.0;
		float fAvgMaxJitter=0.0;
		float fAvgMeanJitter=0.0;
		unsigned uCount=0;

		char cLine[256];
		while(fgets(cLine,255,fp)!=NULL)
                {
			char cSrcIP[256]={""};
			char cDstIP[256]={""};
			unsigned uDstPort=0;
			unsigned uSrcPort=0;
			unsigned uA=0;
			float fPacketLossPercent=0.0;
			float fMaxDelta=0.0;
			float fMaxJitter=0.0;
			float fMeanJitter=0.0;
			unsigned uFields=0;
			char cProblems[256]={""};

			if(guDebug) printf("%s",cLine);
/*
  Src IP addr   Port  Dest IP addr    Port  SSRC       Payload           Pkts   Lost         Max Delta(ms)  Max Jitter(ms) Mean Jitter(ms) Problems?
   75.148.66.17 11800 174.121.136.170 19108 0x0001A372 ITU-T G.711 PCMU  2603   117 (4.3%)   64.73          4.64           1.29		   X
  199.199.10.15 18058 174.121.136.11  12026 0x889F59A4 ITU-T G.711 PCMU  1407  -692 (-96.8%) 43.20          812.00         0.06		   X
*/
			uFields=sscanf(cLine,"%u." //first IP digit uA
						"%[0-9\\.]" //rest of first IP numbers and periods cSrcIP
						"%*[ \\t]" //skip space and tabs
						"%u" //uSrcPort
						"%*[ \\t]" //skip space and tabs
						"%[0-9\\.]" //second IP cDstIP
						"%*[ \\t]" //skip space and tabs
						"%u" //uDstPort
						"%*[ \\t]" //skip space and tabs
						"%*[0-9A-Fx]" //skip hex code e.g. 0x0001A372
						"%*[ \\t]" //
						"%*[A-Z--]" //ITU code
						"%*[ \\t]" //
						"%*[0-9A-Z\\.]" //CODEC
						"%*[ \\t]" //
						"%*[A-Z]" //PCMU
						"%*[ \\t]" //
						"%*d" //Packets
						"%*[ \\t]" //
						"%*d" //Packets
						"%*[ \\t]" //
						" (%f%%)" //fPacketLossPercent
						"%*[ \\t]" //
						"%f" //max delta
						"%*[ \\t]" //
						"%f" // max jitter
						"%*[ \\t]" //
						"%f" // mean jitter
						"%s" //
							,&uA,cSrcIP,&uSrcPort,cDstIP,&uDstPort,
							&fPacketLossPercent,&fMaxDelta,&fMaxJitter,&fMeanJitter,cProblems);
			if(uFields>=9)
			{
				if(guDebug)
					printf("fPacketLossPercent:%2.2f fMaxDelta:%2.2f fMaxJitter:%2.2f fMeanJitter:%2.2f\n",
						fPacketLossPercent,fMaxDelta,fMaxJitter,fMeanJitter);

				if(fPacketLossPercent<100 && fMaxJitter<1000.0 && fMeanJitter < 1000.0)
				{
					fAvgPacketLossPercent+=fPacketLossPercent;
					fAvgMaxDelta+=fMaxDelta;
					fAvgMaxJitter+=fMaxJitter;
					fAvgMeanJitter+=fMeanJitter;
					uCount++;
				}

				if(guDebug)
					printf("cProblems='%s'\n",cProblems);
				if(strchr(cProblems,'X'))
				{
					//unxsVZ integration function also has per container email report code
					//	once loop is done another email report summary may need to be sent*
					unsigned uRetVal=unxsVZModule(uA,cSrcIP,uSrcPort,cDstIP,uDstPort,fPacketLossPercent,uScanCount,uDidNotSendMTREmail);
					if(uRetVal==1)
						break;
				}
			}
			else
			{
				if(guDebug) printf("uFields=%u\n",uFields);
			}
		}//while lines
		if(uCount>1)
		{
			fAvgPacketLossPercent=fAvgPacketLossPercent/uCount;
			fAvgMaxDelta=fAvgMaxDelta/uCount;
			fAvgMaxJitter=fAvgMaxJitter/uCount;
			fAvgMeanJitter=fAvgMeanJitter/uCount;
		}
		if(guDebug)
			printf("fAvgPacketLossPercent:%2.2f fAvgMaxDelta:%2.2f fAvgMaxJitter:%2.2f fAvgMeanJitter:%2.2f uCount=%u\n",
						fAvgPacketLossPercent,fAvgMaxDelta,fAvgMaxJitter,fAvgMeanJitter,uCount);
		else
			printf("%2.2f %2.2f %2.2f %2.2f %u\n",fAvgPacketLossPercent,fAvgMaxDelta,fAvgMaxJitter,fAvgMeanJitter,uCount);
		pclose(fp);
	}//popen
	else
	{
		if(guDebug) printf("%s",cCommand);
	}
	
	//*this is also a unxsVZ integration required code block	
	logfileLine("ProcessNodeQOS","uScanCount",uScanCount);
	if(uScanCount>10)
	{
		if(uDidNotSendMTREmail)
			sprintf(gcQuery,"!%u QOS issues and %u MTRs were skipped!",uScanCount,uDidNotSendMTREmail);
		else
			sprintf(gcQuery,"ProcessTShark has detected %u QOS issues.",uScanCount);
		SendAlertEmail(gcQuery);
	}
	logfileLine("ProcessNodeQOS","end",0);

}//void ProcessNodeQOS(void)


unsigned unxsVZModule(unsigned uA,char *cSrcIP,unsigned uSrcPort,char *cDstIP,unsigned uDstPort,float fPacketLossPercent,
				unsigned uScanCount,unsigned uDidNotSendMTREmail)
{
	if(fPacketLossPercent>2.0 && fPacketLossPercent<90.0)
	{
       		MYSQL_RES *res;
		MYSQL_ROW field;
		unsigned uContainer=0;
		unsigned uProperty=0;
		char cMatchIP[16]={""};
		char cValue[256]={""};
		char cIP[16]={""};
		char cNode[32]={""};
		char cHostname[100]={""};
		sprintf(gcQuery,"SELECT uContainer,tIP.cLabel,tNode.cLabel,tContainer.cHostname FROM tContainer,tIP,tNode"
				" WHERE tContainer.uIPv4=tIP.uIP"
				" AND tContainer.uNode=tNode.uNode"
				" AND tContainer.uNode=%u"
				" AND tContainer.uStatus=1"
				" AND (tIP.cLabel='%u.%s' OR tIP.cLabel='%s')"
					,guNode,uA,cSrcIP,cDstIP);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessTShark",mysql_error(&gMysql),0);
			if(guDebug) printf("%s\n",mysql_error(&gMysql));
			mysql_close(&gMysql);
			return(1);//break on return
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uContainer);
			sprintf(cMatchIP,"%.15s",field[1]);
			sprintf(cNode,"%.31s",field[2]);
			sprintf(cHostname,"%.99s",field[3]);
			if(guDebug) printf("uContainer=%u\n",uContainer);
		}

		if(uContainer==0)
			return(1);//continue on return

		if(!strcmp(cDstIP,cMatchIP))
			sprintf(cIP,"%u.%.12s",uA,cSrcIP);
		else
			sprintf(cIP,"%.15s",cDstIP);

		//determine if cIP is a registered phone
		//rfc1918 ignore them not carriers issue with packets and/or PBX
		unsigned uPhone=0;
		unsigned uAc=0,uBc=0;
		if(2==sscanf(cIP,"%u.%u.%*u.%*u",&uAc,&uBc))
		{
			if( (uAc==192 && uBc==168) || (uAc==10) || (uAc==172 && uBc>=16 && uBc<32) )
				uPhone=1;
		}
		//Dont waste time with expensive system call if packet had private lan ip
		if(!uPhone)
		{
			char cCommand[256];
			FILE *pp;
			sprintf(cCommand,"/usr/sbin/vzctl exec2 %u \"/usr/sbin/asterisk -rx 'sip show peers'|/bin/grep -c %s\"",
						uContainer,cIP);
			if(guDebug) printf("%s\n",cCommand);
			if((pp=popen(cCommand,"r")))
			{
				char cPLine[256];
				if(fgets(cPLine,255,pp)!=NULL)
				{
					sscanf(cPLine,"%u",&uPhone);
					if(guDebug) printf("cpLine=%s",cPLine);
				}
				pclose(pp);
			}
			else
			{
				logfileLine("ProcessTShark","sip show peers",uContainer);
			}
		}

		sprintf(gcQuery,"SELECT uProperty,cValue FROM tProperty"
				" WHERE uKey=%u"
				" AND uType=3"
				" AND cName=CONCAT('cOrg_QOS',WEEKOFYEAR(NOW()),SUBSTR(DAYNAME(NOW()),1,3),'%s')"
					,uContainer,cIP);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			logfileLine("ProcessTShark",mysql_error(&gMysql),uContainer);
			if(guDebug) printf("%s\n",mysql_error(&gMysql));
			mysql_close(&gMysql);
			return(1);//break
		}
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			sscanf(field[0],"%u",&uProperty);
			sprintf(cValue,"%.255s",field[1]);
			if(guDebug) printf("uProperty=%u\n",uProperty);
		}

		unsigned uPacketLossPercent=fPacketLossPercent;
		sprintf(gcQuery,"cIP=%s uPacketLossPercent=%u",cIP,uPacketLossPercent);
		logfileLine("ProcessTShark-cIP-PL",gcQuery,uContainer);
		if(uProperty)
		{
			float fMax=0.0,fMin=0.0,fAvg=0.0;
			unsigned uCount=0;
			sscanf(cValue,"Last=%*f%%; Max=%f%%; Min=%f%%; Count=%u; Avg=%f%%;",&fMax,&fMin,&uCount,&fAvg);
			if(fPacketLossPercent>fMax)
				fMax=fPacketLossPercent;
			else if(fPacketLossPercent<fMin)
				fMin=fPacketLossPercent;
			uCount++;
			//moving average
			//mAvg(i)=mAvg(i-1)*(i-1)/i + x(i)/i
			//fix old records with a wild estimated average
			if(fAvg==0.0)
				fAvg=(fMax+fMin)/2.0;
			else
				fAvg=((fAvg*(float)(uCount-1))/(float)uCount)+(fPacketLossPercent/(float)uCount);
			sprintf(gcQuery,"UPDATE tProperty"
				" SET cValue=CONCAT"
				"('Last=%2.2f%%; Max=%2.2f%%; Min=%2.2f%%; Count=%u; Avg=%2.2f%%; CPE=%u; Node=%s; ',TIME(NOW())),"
				" uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())"
				" WHERE uProperty=%u"
					,fPacketLossPercent,fMax,fMin,uCount,fAvg,uPhone,cNode,uProperty);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessTShark",mysql_error(&gMysql),uContainer);
				if(guDebug) printf("%s\n",mysql_error(&gMysql));
				mysql_close(&gMysql);
				return(1);//break
			}
			if(guDebug) printf("%s\n",gcQuery);
		}
		else
		{
			sprintf(gcQuery,"INSERT INTO tProperty"
				" SET cValue=CONCAT"
				"('Last=%2.2f%%; Max=%2.2f%%; Min=%2.2f%%; Count=1; Avg=%2.2f%%; CPE=%u; Node=%s; ',TIME(NOW())),"
				" cName=CONCAT('cOrg_QOS',WEEKOFYEAR(NOW()),SUBSTR(DAYNAME(NOW()),1,3),'%s'),"
				" uKey=%u,"
				" uType=3,"
				" uOwner=%u,"
				" uCreatedBy=1,"
				" uCreatedDate=UNIX_TIMESTAMP(NOW())"
					,fPacketLossPercent,fPacketLossPercent,fPacketLossPercent,fPacketLossPercent,uPhone,cNode,
					cIP,uContainer,guOwner);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				logfileLine("ProcessTShark",mysql_error(&gMysql),uContainer);
				if(guDebug) printf("%s\n",mysql_error(&gMysql));
				mysql_close(&gMysql);
				return(1);//break
			}
			if(guDebug) printf("%s\n",gcQuery);
		}
		uScanCount++;
//#define DEBUG1 On
#ifdef DEBUG1
		//debug only
		SendMTREmail(cIP,uContainer,cHostname,fPacketLossPercent);
#else

		//fork and create an mtr report and email it
		if(fPacketLossPercent>30.0 && uPhone==0)
		{
			//Node is busy enough
			if(uScanCount<10)
				SendMTREmail(cIP,uContainer,cHostname,fPacketLossPercent);
			else
				uDidNotSendMTREmail++;
		}
#endif
	}//more than 2 percent less than 90 percent

	return(0);//not break

}//unsigned unxsVZModule()
