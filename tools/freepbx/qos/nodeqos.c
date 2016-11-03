/*
FILE
	nodeqos.c
	svn ID removed
PURPOSE
	Gather Asterisk QOS data from every PBX on this node.
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2012.
	GPLv2 License applies. See LICENSE file.
NOTES
	Graph on node via rrdtool script.
	Provide Zabbix interface for remote logging and alert triggering.
DEPENDENCIES
	ALLPBXScript below
CRON
	#special
	* * * * * /usr/sbin/nodeQOSGraph.sh > /dev/null;

*/

#define ALLPBXScript "/usr/sbin/vzAllContainerCmd3.sh \"/usr/sbin/asterisk -rx 'sip show channelstats'\""

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int main(int iArgc, char *cArgv[])
{
	FILE *fp;
	char cLine[256];
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
	unsigned uFirst=1;
	unsigned uNumCalls=0;
	unsigned uDebug=0;
	unsigned uRRDTool=0;

	if(iArgc==1)
	{
		printf("usage: %s [--run] [--help] [--version] [--rrdtool] [--debug|--debug2]\n",cArgv[0]);
		return(0);
	}

	//process command line args
	register int i;
	for(i=1;i<iArgc;i++)
	{
		if(!strncmp(cArgv[i],"--version",9))
		{
			printf("svn ID removed
			return(0);
		}
		else if(!strncmp(cArgv[i],"--help",6))
		{
			printf("usage: %s [--run] [--help] [--version] [--rrdtool] [--debug1|--debug2]\n",cArgv[0]);
			return(0);
		}
		else if(!strncmp(cArgv[i],"--debug1",8))
		{
			uDebug=1;
		}
		else if(!strncmp(cArgv[i],"--debug2",8))
		{
			uDebug=2;
		}
		else if(!strncmp(cArgv[i],"--rrdtool",9))
		{
			uRRDTool=1;
		}
	}

	if(uDebug)
	{
		printf("uDebug=%u uRRDTool=%u\n",uDebug,uRRDTool);
	}

	if((fp=popen(ALLPBXScript,"r")))
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

			if(uDebug>1)
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
				if(uDebug>0)
					printf("uHrs=%u uMins=%u uSecs=%u fLossRecv=%f fJitterRecv=%f fLossSend=%f fJitterSend=%f\n",
						uHrs,uMins,uSecs,fLossRecv,fJitterRecv,fLossSend,fJitterSend);
					
				unsigned uCallDuration=uSecs+uMins*60+uHrs*3600;
				if(uDebug>0 && uCallDuration<60)
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

					fMean=fLossRecvAvg/uNumCalls;
					fLossRecvStd=sqrt((fLossRecvAvgX2/uNumCalls) - (fMean*fMean));

					fMean=fLossSendAvg/uNumCalls;
					fLossSendStd=sqrt((fLossSendAvgX2/uNumCalls) - (fMean*fMean));

					fMean=fJitterRecvAvg/uNumCalls;
					fJitterRecvStd=sqrt((fJitterRecvAvgX2/uNumCalls) - (fMean*fMean));

					fMean=fJitterSendAvg/uNumCalls;
					fJitterSendStd=sqrt((fJitterSendAvgX2/uNumCalls) - (fMean*fMean));

					uNumCalls++;
				}
			}
		}
	}
	else
	{
		printf("Could not popen() %s\n",ALLPBXScript);
		return(1);
	}

	if(uNumCalls>0)
	{
		fLossRecvAvg=fLossRecvAvg/uNumCalls;
		fJitterRecvAvg=fJitterRecvAvg/uNumCalls;
		fLossSendAvg=fLossSendAvg/uNumCalls;
		fJitterSendAvg=fJitterSendAvg/uNumCalls;
	}

	if(uRRDTool)
	{
		if(uDebug)
			printf("fLossSendAvg fLossRecvAvg fJitterSendAvg*1000 fJitterRecvAvg*1000 uNumCalls\n");
		printf("%.0f %.0f %.0f %.0f %u\n",
				fLossSendAvg*100,fLossRecvAvg*100,fJitterSendAvg*100,fJitterRecvAvg*100,uNumCalls);
		return(0);
	}
	
	printf("uNumCalls=%u\n",uNumCalls);
	if(uNumCalls==0) return(0);

	printf("fLossRecvMin=%f fJitterRecvMin=%f fLossSendMin=%f fJitterSendMin=%f\n",
				fLossRecvMin,fJitterRecvMin,fLossSendMin,fJitterSendMin);
	printf("fLossRecvMax=%f fJitterRecvMax=%f fLossSendMax=%f fJitterSendMax=%f\n",
				fLossRecvMax,fJitterRecvMax,fLossSendMax,fJitterSendMax);
	printf("fLossRecvAvg=%f fJitterRecvAvg=%f fLossSendAvg=%f fJitterSendAvg=%f\n",
				fLossRecvAvg,fJitterRecvAvg,fLossSendAvg,fJitterSendAvg);
	printf("fLossRecvStd=%f fJitterRecvStd=%f fLossSendStd=%f fJitterSendStd=%f\n",
				fLossRecvStd,fJitterRecvStd,fLossSendStd,fJitterSendStd);

	return(0);
}//main()
