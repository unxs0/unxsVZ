/*
FILE
        $Id$
PURPOSE
        Localization constants, configuration settings and release info.
LEGAL
        Copyright 2001-2009 Gary Wallis and Hugo Urquiza
*/

//Change for your setup DO NOT USE THESE DEFAULTS: DANGER!
#define DBIP0 NULL
#define DBIP1 NULL
//#define DBIP0 "200.43.140.17"
//#define DBIP1 "200.43.140.250"
#define DBNAME "unxsisp"
#define DBLOGIN "unxsisp"
#define DBPASSWD "wsxedc"
#define DBPORT 0
#define DBSOCKET NULL
//Example for host running two mysqld
//See makefile and mysqlrad.h CustomExample sections
//#define DBPORT 4306
//#define DBSOCKET "/var/lib/mysql/mysql4.sock"

//unxsisp can report to us once a week some useful for
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
#define RELEASE "2rc1"
#define REV "2.1"
#define RELEASESHORT "2rc1"

//This should almost never have to changed since it is for integrating other
//unxsISP related software with unxsISP.tClient and .tAuthorize
#define TCLIENT "tClient"
#define TAUTHORIZE "tAuthorize"

