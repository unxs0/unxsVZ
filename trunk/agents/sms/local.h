/*
FILE
        $Id: local.h.default 726 2009-09-23 13:13:33Z Gary $
PURPOSE
        Localization constants, configuration settings and release info.
LEGAL
        Copyright 2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies see LICENSE file.
*/

//Change for your setup DO NOT USE THESE DEFAULTS: DANGER!
#define DBIP0 NULL
#define DBIP1 NULL
#define DBNAME "unxsvz"
#define DBLOGIN "unxsvz"
#define DBPASSWD "wsxedc"
#define DBPORT 0
#define DBSOCKET NULL
//Example for host running two mysqld
//See makefile and mysqlrad.h CustomExample sections
//#define DBPORT 4306
//#define DBSOCKET "/var/lib/mysql/mysql4.sock"

//End-users: Do not change these!
//Also edit main.c header RAD status line if you change the following
#define RELEASE "0.1/alpha"
