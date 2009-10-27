/*
FILE
	main.c
	$Id$
	/usr/sbin/unxsvz-autonomics
AUTHOR
	(C) 2009, Gary Wallis for Unixservice USA
PURPOSE
	Calculate every 7 seconds autonomic rules.
	If a rule is triggered take configured action.
	This file: Start, stop handle signals of autonomics daemon.
NOTES
*/

#include "autonomics.h"
#include <sys/wait.h>

//global data storage
FILE *gLfp;
MYSQL gMysql;
char gcLine[256];
unsigned guDryrun=0;
char gcNodeInstalledRam[100]={""};
char gcNodeAutonomics[100]={""};
char gcDatacenterAutonomics[100]={""};
char gcHostname[100];
char gcNodeWarnEmail[100]={""};
char gcDatacenterWarnEmail[100]={""};
char gcQuery[2048];
char gcLogKey[100]={"Kjhsdfjkui9345kijhsdfiuh43908khjlkjh"};
unsigned guDatacenter=0;
unsigned guNode=0;
int giAutonomicsPrivPagesWarnRatio=0;//default never
int giAutonomicsPrivPagesActRatio=0;//default never
unsigned guWarned=0;
unsigned guActedOn=0;
pid_t gpidMain;

void sighandlerChild(int iSig);

int main(int iArgc, char *cArgv[])
{
	if(iArgc==2 && !strncmp(cArgv[1],"--fg",4))
	{
		printf("Running %s in foreground\n",cArgv[0]);
		gLfp=stderr;
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
		if((gLfp=fopen(cLOGFILE,"a"))==NULL)
		{
			fprintf(stderr,"Could not open error log!\n");
			return(300);
        	}
		//Set signal handler
		//Exit sigs
		(void) signal(SIGINT,sighandlerLeave);
		(void) signal(SIGTERM,sighandlerLeave);
		//Cont sigs
		(void) signal(SIGHUP,sighandlerReload);
		(void) signal(SIGUSR1,sighandlerReload);
		(void) signal(SIGUSR2,sighandlerReload);
		(void) signal(SIGCHLD,sighandlerChild);
	}

        mysql_init(&gMysql);
        if(!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,0,NULL,0))
        {
		if(!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,0,NULL,0))
		{
               		logfileLine("main","database server unavailable\n");
			fclose(gLfp);
			mysql_close(&gMysql);
			return(100);
		}
	}

	gpidMain=getpid();
	gethostname(gcHostname,99);
	sprintf(gcLine,"%s started. gcHostname=%s, guDryrun=%u, pid=%u",
			cArgv[0],gcHostname,guDryrun,gpidMain);
	logfileLine("main",gcLine);

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

	logfileLine("main","abnormal ending");
	fclose(gLfp);
	mysql_close(&gMysql);
	return(0);

}//main()


void sighandlerLeave(int iSig)
{
	sprintf(gcLine,"interrupted by signal:%d",iSig);
	logfileLine("sighandlerLeave",gcLine);

	fclose(gLfp);
	mysql_close(&gMysql);

        exit(iSig);

}//void sighandlerLeave(int iSig)


void sighandlerChild(int iSig)
{
	int iStatus;
	pid_t pid;

	pid=getpid();
	if(pid==gpidMain)
	{
		sprintf(gcLine,"interrupted by child signal:%d",iSig);
		logfileLine("sighandlerChild",gcLine);

		wait(&iStatus);
	}

}//void sighandlerLeave(int iSig)


void sighandlerReload(int iSig)
{
	//node.c scoped
	extern int giAutonomicsPrivPagesWarnRatio;
	extern int giAutonomicsPrivPagesActRatio;

	sprintf(gcLine,"interrupted by signal:%d %s",iSig,strsignal(iSig));
	logfileLine("sighandlerReload",gcLine);
	guDryrun=0;
	gcNodeInstalledRam[0]=0;
	gcNodeAutonomics[0]=0;
	gcDatacenterAutonomics[0]=0;
	gcNodeWarnEmail[0]=0;
	gcDatacenterWarnEmail[0]=0;
	giAutonomicsPrivPagesWarnRatio=0;
	giAutonomicsPrivPagesActRatio=0;
	guDatacenter=0;
	guNode=0;
	guWarned=0;
	guActedOn=0;

}//void sighandlerReload(int iSig)


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

        fprintf(gLfp,"%s autonomics.%s[%u]: %s\n",cTime,cFunction,pidThis,cLogline);
	fflush(gLfp);

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

