/*
FILE
	sms.c
	$Id: sms.c 1466 2010-05-18 16:32:59Z Gary $
PURPOSE
	SMS gateway preprocessor interface
AUTHOR
	Gary Wallis for Unxiservice, LLC. (C) 2010.
	GPLv2 License applies. See LICENSE file.
NOTES
	See README in this dir.
*/

#include "mysqlrad.h"
#include <sys/sysinfo.h>

MYSQL gMysql;
FILE *gLfp=NULL;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};
char gcProgram[32]={""};
unsigned guStatus=0;//not a valid status
unsigned uDebug=1;

//local protos
void TextConnectDb(void);
void Initialize(void);

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

		fprintf(gLfp,"%s %s[%u]: %s.\n",cTime,cFunction,pidThis,cLogline);
		if(uDebug)
			fprintf(stdout,"%s %s[%u]: %s.\n",cTime,cFunction,pidThis,cLogline);
		fflush(gLfp);
	}

}//void logfileLine()


int main(int iArgc, char *cArgv[])
{
	struct sysinfo structSysinfo;

	if((gLfp=fopen("/var/log/unxsSMS.log","a"))==NULL)
	{
		fprintf(stderr,"Could not open /var/log/unxsSMS.log error exit\n");
		exit(1);
	}

	sprintf(gcProgram,"%.31s",cArgv[0]);

	if(sysinfo(&structSysinfo))
	{
		logfileLine("main","sysinfo() failed");
		fclose(gLfp);
		exit(1);
	}
#define LINUX_SYSINFO_LOADS_SCALE 65536
#define JOBQUEUE_MAXLOAD 20 //This is equivalent to uptime 20.00 last 1 min avg load
	if(structSysinfo.loads[0]/LINUX_SYSINFO_LOADS_SCALE>JOBQUEUE_MAXLOAD)
	{
		logfileLine("main","structSysinfo.loads[0] larger than JOBQUEUE_MAXLOAD");
		fclose(gLfp);
		exit(1);
	}

	switch(iArgc)
	{
		case 2:
			if(!strncmp(cArgv[1],"initialize",10))
			{
				Initialize();
			}
			else
			{
				sprintf(gcQuery,"unknown command cArgv[1]=%.32s",cArgv[1]);
				logfileLine("main",gcQuery);
				fclose(gLfp);
				exit(1);
			}
		break;

		default:
			sprintf(gcQuery,"unknown command iArgc=%d",iArgc);
			logfileLine("main",gcQuery);
			fclose(gLfp);
			exit(1);
	}

	fclose(gLfp);
	return(0);
}//main()


void Initialize(void)
{
	if(uDebug)
		printf("Initialize()\n");
	
}//void Initialize(void)

