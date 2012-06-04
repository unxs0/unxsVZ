/*
FILE 
	unxsrad.h
	$Id$
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

//Global vars
extern MYSQL gMysql;
extern char gcQuery[];
extern char *gcBuildInfo;
