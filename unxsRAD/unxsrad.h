/*
FILE 
	unxsrad.h
	svn ID removed
PURPOSE
	unxsRAD job queue CLI standalone program header file
LEGAL
	(C) Gary Wallis 2012 for Unixservice, LLC. All Rights Reserved.
	LICENSE file should be included in distribution.
OTHER
HELP

*/

#include "mysqlrad.h"
#include "local.h"
#include <ctype.h>

#define cLOGFILE "/var/log/unxsRAD.log"

//Global vars
extern MYSQL gMysql;
extern char gcQuery[];
extern char *gcBuildInfo;
