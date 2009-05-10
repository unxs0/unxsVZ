/*
FILE
        $Id: local.h.template 1262 2007-06-30 22:36:38Z Gary $
PURPOSE
        Localization constants, configuration settings and release info.
LEGAL
        Copyright 2001-2007 Gary Wallis
*/

//Change for your setup DO NOT USE THESE DEFAULTS: DANGER!
#define DBIP0 NULL
#define DBIP1 NULL
//#define DBIP0 "192.168.0.4"
//#define DBIP1 "192.168.0.5"
#define DBNAME "mysqlradacct2"
#define DBLOGIN "mysqlradacct2"
#define DBPASSWD "wsxedc"
#define DBPORT0 3306
#define DBPORT1 3307
#define DBSOCKET NULL
//Example for host running two mysqld
//See makefile and mysqlrad.h CustomExample sections
//#define DBPORT 4306
//#define DBSOCKET "/var/lib/mysql/mysql4.sock"

//mysqlradacct2 can report to us once a week some useful for
//debugging and development statistics.
//This is very helpful to us and in the end we hope for yourself.
//For improving and developing this software
//#define DEBUG_REPORT_STATS_OFF

//Setup your own CA and sign a cert and start up your Apache/mod_ssl server
//for security.
//Make sure mySQL ports are firewalled correctly if used on other servers.
//VPN them?
//#define SSLONLY

//You should customize these for your own ISP
#define EXTISPNAME "openisp"
#define EXTISPURL "openisp.net"


//End-users: Do not change these!
//Developers: See makefile also when changing these
#define RELEASE "2.3"
#define REV "2.3"

//Master tClient
#define TCLIENT "tClient"
//#define TCLIENT "mysqlisp2.tClient"
#define TAUTHORIZE "tAuthorize"
//#define TAUTHORIZE "mysqlisp2.tAuthorize"
//If you use the global ISP tClient and tAuthorize tables in mysqlisp2 db you may need to
//	issue a SQL commands like: 
//
//	mysql> grant select on mysqlisp2.tClient to mysqlradacct2@localhost identified by 'wsxedc';
//	mysql> grant select on mysqlisp2.tAuthorize to mysqlradacct2@localhost identified by 'wsxedc';
//
//	Note: You will have to turn off MOD, DEL and NEW functionality in tclientfunc.h and tauthorizefunc.h
//	if you only grant select. If you grant all...then understand the possible consequences.

