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

#define ERRLOG "/var/log/unxsvz-autonomics.log"

//global data
extern FILE *gEfp;
extern MYSQL gMysql;
extern char gcLine[];
extern unsigned guDryrun;
extern char gcHostname[];
extern char gcQuery[];
extern unsigned guDatacenter;
extern unsigned guNode;


//main.c
void daemonize(void);
void sighandlerLeave(int iSig);
void logfileLine(char *cLogline);

//datacenter.c
int DatacenterAutonomics(void);
//node.c
int NodeAutonomics(void);
//container.c
int ContainerAutonomics(void);
