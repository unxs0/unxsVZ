/*
FILE
        $Id: local.h 1306 2007-07-12 15:41:30Z Gary $
PURPOSE
        Localization constants, configuration settings and release info.
LEGAL
        Copyright 2001-2007 Gary Wallis
*/

//Change for your setup DO NOT USE THESE DEFAULTS: DANGER!
#define DBNAME "mysqlapache2"
//#define DBIP0 "70.38.123.57"
//#define DBIP1 "70.38.123.58"
#define DBIP0 NULL 
#define DBIP1 NULL
#define DBLOGIN "mysqlapache2"
#define DBPASSWD "wsxedc"
#define DBPORT 0
#define DBSOCKET NULL
//Example for host running two mysqld
//See makefile and mysqlrad.h CustomExample sections
//#define DBPORT 4306
//#define DBSOCKET "/var/lib/mysql/mysql4.sock"

//mysqlapache2 can report to us once a week some useful for
//debugging and development statistics.
//This is very helpful to us and in the end we hope for yourself.
//For improving and developing this software
//#define DEBUG_REPORT_STATS_OFF

//Setup your own CA and sign a cert and start up your Apache/mod_ssl server
//for security.
//Make sure mySQL ports are firewalled correctly if used on other servers.
//VPN them?
#define SSLONLY

//You should customize these for your own ISP
#define EXTISPNAME "openisp"
#define EXTISPURL "openisp.net"

//Master tClient
#define TCLIENT "tClient"
//#define TCLIENT "mysqlisp2.tClient"
#define TAUTHORIZE "tAuthorize"
//#define TAUTHORIZE "mysqlisp2.tAuthorize"

//End-users: Do not change these!
//Developers: See makefile also when changing these
#define RELEASE ""
#define REV "1.3"

#define GSHADOW 1
