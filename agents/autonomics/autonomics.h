/*
FILE
	autonomics.h
	svn ID removed
AUTHOR
	(C) 2009, Gary Wallis for Unixservice USA
PURPOSE
	unxsvz-autonomics main include header file.
NOTES
*/


#include "../../mysqlrad.h"
//#include <ctype.h>
char *strsignal(int sig);



#define mysqlQuery_Err_Exit	mysql_query(&gMysql,gcQuery);\
				if(mysql_errno(&gMysql))\
				{\
					logfileLine("mysqlQuery_Err_Exit",mysql_error(&gMysql));\
					sighandlerLeave(400);\
				}


//global data
extern FILE *gLfp;
extern MYSQL gMysql;
extern char gcLine[];
extern unsigned guDryrun;
extern char gcHostname[];
extern char gcQuery[];
extern unsigned guDatacenter;
extern unsigned guNode;
extern unsigned guContainer;
extern char gcLogKey[];

struct structAutonomicsState
{
	char cNodeInstalledRam[100];
	char cNodeAutonomics[100];
	char cDatacenterAutonomics[100];
	char cDatacenterWarnEmail[100];
	char cNodeWarnEmail[100];
	char cContainerWarnEmail[100];
	unsigned uNodePrivPagesWarnRatio;
	unsigned uNodePrivPagesActRatio;
	unsigned uNodeRamUtilActRatio;
	unsigned uNodeRamUtilWarnRatio;
	unsigned uNodeHDUtilActRatio;
	unsigned uNodeHDUtilWarnRatio;
	unsigned uNodePrivPagesWarned;
	unsigned uNodePrivPagesActedOn;
	unsigned uNodeRamUtilWarned;
	unsigned uNodeRamUtilActedOn;
	unsigned uNodeHDUtilWarned;
	unsigned uNodeHDUtilActedOn;

	long unsigned luNodeInstalledRam;
	long unsigned luNodeInstalledDiskSpace;

	char cuNodeWarnEmailRepeat[16];
};

extern struct structAutonomicsState gsAutoState;

//main.c
void daemonize(void);
void sighandlerLeave(int iSig);
void sighandlerReload(int iSig);
void logfileLine(const char *cFunction,const char *cLogline);

//datacenter.c
int DatacenterAutonomics(void);
//node.c
int NodeAutonomics(void);
unsigned iNodeMemConstraints(void);
void Log(char *cMessage);
void SendEmail(char *cEmail, char *cSubjectPrefix, char *cMessage);
//container.c
int ContainerAutonomics(void);
