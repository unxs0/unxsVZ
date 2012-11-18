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

	if((fp=popen("/usr/sbin/vzAllContainerCmd3.sh \"/usr/sbin/asterisk -rx 'sip show channelstats'\"","r")))
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


			if(strstr(cLine,"Peer"))
				continue;
			if(strstr(cLine,"active SIP channels"))
				continue;
			if(strstr(cLine,"No RTP active"))
				continue;

			if(isdigit(cLine[0])&&(isdigit(cLine[1])||cLine[1]=='.'))
			{
				if(iArgc>1)
					printf("%s",cLine);
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
				if(iArgc>2)
					printf("uHrs=%u uMins=%u uSecs=%u fLossRecv=%f fJitterRecv=%f fLossSend=%f fJitterSend=%f\n",
						uHrs,uMins,uSecs,fLossRecv,fJitterRecv,fLossSend,fJitterSend);
					
				unsigned uCallDuration=uSecs+uMins*60+uHrs*3600;
				if(iArgc>2 && uCallDuration<60)
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


	if(uNumCalls==0) return(0);

	fLossRecvAvg=fLossRecvAvg/uNumCalls;
	fJitterRecvAvg=fJitterRecvAvg/uNumCalls;
	fLossSendAvg=fLossSendAvg/uNumCalls;
	fJitterSendAvg=fJitterSendAvg/uNumCalls;

	printf("uNumCalls=%u\n",uNumCalls);
	printf("fLossRecvMin=%f fJitterRecvMin=%f fLossSendMin=%f fJitterSendMin=%f\n",
				fLossRecvMin,fJitterRecvMin,fLossSendMin,fJitterSendMin);
	printf("fLossRecvMax=%f fJitterRecvMax=%f fLossSendMax=%f fJitterSendMax=%f\n",
				fLossRecvMax,fJitterRecvMax,fLossSendMax,fJitterSendMax);
	printf("fLossRecvAvg=%f fJitterRecvAvg=%f fLossSendAvg=%f fJitterSendAvg=%f\n",
				fLossRecvAvg,fJitterRecvAvg,fLossSendAvg,fJitterSendAvg);
	printf("fLossRecvStd=%f fJitterRecvStd=%f fLossSendStd=%f fJitterSendStd=%f\n",
				fLossRecvStd,fJitterRecvStd,fLossSendStd,fJitterSendStd);

	return(0);
}
