/*
FILE
        svn ID removed
PURPOSE
        Localization constants, configuration settings and release info.
LEGAL
        Copyright 2001-2007 Gary Wallis
*/

//Change for your setup DO NOT USE THESE DEFAULTS: DANGER!
#define DBIP NULL
#define DBIP0 NULL
#define DBIP1 NULL
#define DBNAME "mysqlblog2"
#define DBLOGIN "mysqlblog2"
#define DBPASSWD "wsxedc"
#define DBPORT 0
#define DBSOCKET NULL
//Example for host running two mysqld
//See makefile and mysqlrad.h CustomExample sections
//#define DBPORT 4306
//#define DBSOCKET "/var/lib/mysql/mysql4.sock"

//mysqlblog2 can report to us once a week some useful for
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

#define TCLIENT "tClient"
#define TAUTHORIZE "tAuthorize"

//End-users: Do not change these!
//Developers: See makefile also when changing these
#define RELEASE ""
#define REV "0.1"

