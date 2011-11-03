#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(int iArg, char **cArg)
{
	FILE *pp;
	float fIn,fOut,fMaxIn=0.0,fMaxOut=0.0;
	char cBuf[256];
	long unsigned luDate=0;
	long unsigned luMaxInDate=0;
	long unsigned luMaxOutDate=0;

	if(iArg!=4)
	{
		printf("usage: %s <rrd file> <start time> <end time>\n",cArg[0]);
		exit(0);
	}

	sprintf(cBuf,"rrdtool fetch %.128s AVERAGE -s %.32s -e %.32s",cArg[1],cArg[2],cArg[3]);
	pp=popen(cBuf,"r");
	if(pp==NULL)
	{
		printf("error %s\n",cBuf);
		exit(1);
	}

	while(fgets(cBuf,255,pp)!=NULL)
	{
		if(strchr(cBuf,':'))
		{
			sscanf(cBuf,"%lu: %f %f",&luDate,&fIn,&fOut);
			printf("%2.2f %2.2f %s",fIn,fOut,ctime(&luDate));
			if(fIn>fMaxIn)
			{
				fMaxIn=fIn;
				luMaxInDate=luDate;
			}
			if(fOut>fMaxOut)
			{
				fMaxOut=fOut;
				luMaxOutDate=luDate;
			}
		}
	}

	if(luMaxInDate || luMaxOutDate)
	{
		printf("Max in: %2.2f at %s (%lu)\n",fMaxIn,ctime(&luMaxInDate),luMaxInDate);
		printf("Max out: %2.2f at %s (%lu)\n",fMaxOut,ctime(&luMaxOutDate),luMaxOutDate);
	}
	else
	{
		printf("No valid data found\n");
	}

	pclose(pp);

	exit(0);
	return(0);
}
