/*
FILE
	main.c
	$Id$
	/usr/sbin/unxsvz-autonomics
AUTHOR
	(C) 2006-2009, Gary Wallis for Unixservice USA
PURPOSE
NOTES
TODO
*/

#include "../../mysqlrad.h"
#include <ctype.h>

//#define DBIP "localhost"
#define ERRLOG "/var/log/unxsvz-autonomics.log"

void daemonize(void);
void sighandlerLeave(int iSig);
void logfileLine(char *cLogline);

//global data
FILE *gEfp;
MYSQL gMysql;
char gcLine[256];

int main(int iArgc, char *cArgv[])
{
//	MYSQL_RES *res;
//	MYSQL_ROW field;
	char cHostname[100];

	if(iArgc==2 && !strncmp(cArgv[1],"--fg",4))
	{
		printf("Running %s in foreground\n",cArgv[0]);
		gEfp=stderr;
	}
	else if(iArgc==2 && !strncmp(cArgv[1],"--help",6))
	{
		printf("%s help\n\n--help\tthis page\n--fg\trun program in foreground mode\n",
				cArgv[0]);
		return(0);
	}
	else if(iArgc>=2)
	{
		printf("Usage: %s [--help | --fg]\n",cArgv[0]);
		return(0);
	}
	else if(1)
	{
		daemonize();
		if((gEfp=fopen(ERRLOG,"a"))==NULL)
		{
			fprintf(stderr,"Could not open error log!\n");
			return(3);
        	}
		//Set signal handler
		(void) signal(SIGINT,sighandlerLeave);
	}

        mysql_init(&gMysql);

	gethostname(cHostname,99);
	sprintf(gcLine,"%s started and mysql_init() ran ok. cHostname=%s",cArgv[0],cHostname);
	logfileLine(gcLine);

	while(1)
	{
		;
	}//while(1)

//Return_Point:
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

