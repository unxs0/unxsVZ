/*
FILE
	main.c
	$Id$
	/usr/sbin/unxsvz-autonomics
AUTHOR
	(C) 2009, Gary Wallis for Unixservice USA
PURPOSE
	Calculate every 10 seconds autonomic rules.
	If a rule is triggered take configured action.
	This file: Start, stop handle signals of autonomics daemon.
NOTES
*/

#include "autonomics.h"

//global data storage
FILE *gEfp;
MYSQL gMysql;
char gcLine[256];
unsigned guDryrun=0;
char gcHostname[100];
char gcNodeWarnEmail[100]={""};
char gcDatacenterWarnEmail[100]={""};
char gcQuery[1024];
unsigned guDatacenter=0;
unsigned guNode=0;

int main(int iArgc, char *cArgv[])
{
	if(iArgc==2 && !strncmp(cArgv[1],"--fg",4))
	{
		printf("Running %s in foreground\n",cArgv[0]);
		gEfp=stderr;
	}
	else if(iArgc==2 && !strncmp(cArgv[1],"--help",6))
	{
		printf("%s help\n\n--help\t\tthis page\n--fg\t\trun program in foreground mode\n"
				"--dryrun\treport recommended autonomics in log but take no action\n",
				cArgv[0]);
		return(0);
	}
	else if(iArgc>=2 && strncmp(cArgv[1],"--dryrun",8))
	{
		printf("Usage: %s [--help | --fg | --dryrun]\n",cArgv[0]);
		return(0);
	}
	else if(1)
	{
		if(iArgc==2 && !strncmp(cArgv[1],"--dryrun",8))
			guDryrun=1;
		daemonize();
		if((gEfp=fopen(ERRLOG,"a"))==NULL)
		{
			fprintf(stderr,"Could not open error log!\n");
			return(300);
        	}
		//Set signal handler
		(void) signal(SIGINT,sighandlerLeave);
		(void) signal(SIGHUP,sighandlerLeave);
		(void) signal(SIGTERM,sighandlerLeave);
	}

        mysql_init(&gMysql);
        if(!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,0,NULL,0))
        {
		if(!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,0,NULL,0))
		{
               		logfileLine("database server unavailable\n");
			fclose(gEfp);
			mysql_close(&gMysql);
			return(100);
		}
	}

	gethostname(gcHostname,99);
	sprintf(gcLine,"%s started. gcHostname=%s, guDryrun=%u",
			cArgv[0],gcHostname,guDryrun);
	logfileLine(gcLine);

	while(1)
	{
		sleep(7);
		if(DatacenterAutonomics())
			break;
		if(NodeAutonomics())
			break;
		if(ContainerAutonomics())
			break;
	}//while(1)

	logfileLine("abnormal ending");
	fclose(gEfp);
	mysql_close(&gMysql);
	return(0);

}//main()


void sighandlerLeave(int iSig)
{
	sprintf(gcLine,"interrupted by signal:%d",iSig);
	logfileLine(gcLine);

	fclose(gEfp);
	mysql_close(&gMysql);

        exit(iSig);

}//void sighandlerLeave(int iSig)


void logfileLine(char *cLogline)
{
	time_t luClock;
	char cTime[32];

	time(&luClock);
	ctime_r(&luClock,cTime);
	cTime[strlen(cTime)-1]=0;

        fprintf(gEfp,"%s:%s\n",cTime,cLogline);
	fflush(gEfp);

}//void logfileLine(char *cLogline)


void daemonize(void)
{
	switch(fork())
	{
		default:
			_exit(0);

		case -1:
			fprintf(stderr,"fork failed\n");
			_exit(1);

		case 0:
		break;
	}

	if(setsid()<0)
	{
		fprintf(stderr,"setsid failed\n");
		_exit(1);
	}

}//void daemonize(void)

