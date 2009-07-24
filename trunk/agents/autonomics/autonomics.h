/*
FILE
	autonomics.h
	$Id$
AUTHOR
	(C) 2009, Gary Wallis for Unixservice USA
PURPOSE
	unxsvz-autonomics main include header file.
NOTES
*/


#include "../../mysqlrad.h"
//#include <ctype.h>
char *strsignal(int sig);



#define ERRLOG "/var/log/unxsvz-autonomics.log"
#define mysqlQuery_Err_Exit	mysql_query(&gMysql,gcQuery);\
				if(mysql_errno(&gMysql))\
				{\
					logfileLine((char *)mysql_error(&gMysql));\
					sighandlerLeave(400);\
				}


//global data
extern FILE *gEfp;
extern MYSQL gMysql;
extern char gcLine[];
extern unsigned guDryrun;
extern char gcNodeInstalledRam[];
extern char gcNodeAutonomics[];
extern char gcDatacenterAutonomics[];
extern char gcNodeWarnEmail[];
extern char gcDatacenterWarnEmail[];
extern char gcHostname[];
extern char gcQuery[];
extern unsigned guDatacenter;
extern unsigned guNode;
extern int giAutonomicsPrivPagesWarnRatio;
extern int giAutonomicsPrivPagesActRatio;
extern unsigned guWarned;
extern unsigned guActedOn;
extern char gcLogKey[];


//main.c
void daemonize(void);
void sighandlerLeave(int iSig);
void sighandlerReload(int iSig);
void logfileLine(char *cLogline);

//datacenter.c
int DatacenterAutonomics(void);
//node.c
int NodeAutonomics(void);
unsigned iNodeMemConstraints(void);
void Log(char *cMessage);
void SendPrivPagesEmail(char *cEmail, char *cSubjectPrefix);
//container.c
int ContainerAutonomics(void);
