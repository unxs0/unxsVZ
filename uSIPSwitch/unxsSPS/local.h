/*
FILE
        svn ID removed
PURPOSE
        Localization constants, configuration settings and release info.
LEGAL
        Copyright 2001-2012 Gary Wallis for Unixservice, LLC.
*/

//Change for your setup DO NOT USE THESE DEFAULTS: DANGER!
#define DBIP0 NULL
#define DBIP1 "127.0.0.1"
#define DBNAME "unxssps"
#define DBLOGIN "unxssps"
#define DBPASSWD "wsxedc"
#define DBPORT 0
#define DBSOCKET NULL
//Example for host running two mysqld
//See makefile and mysqlrad.h CustomExample sections
//#define DBPORT 4306
//#define DBSOCKET "/var/lib/mysql/mysql4.sock"

//Master tClient and related tAuthorize can be centralized
// to be used by all mysqlISP2 related subsystems.
#define TCLIENT "tClient"
//#define TCLIENT "mysqlISP2.tClient"
#define TAUTHORIZE "tAuthorize"
//#define TAUTHORIZE "mysqlISP2.tAuthorize"


//unxsrad can report to us once a week some useful for
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
#define RELEASE ""
#define REV "0.1"

