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
#define DBNAME "unxsmail"
#define DBLOGIN "unxsmail"
#define DBPASSWD "wsxedc"
#define DBPORT 0
#define DBSOCKET NULL
//Example for host running two mysqld
//See makefile and mysqlrad.h CustomExample sections
//#define DBPORT 4306
//#define DBSOCKET "/var/lib/mysql/mysql4.sock"

//unxsmail can report to us once a week some useful for
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


//End-users: Do not change these!
//Developers: See makefile also when changing these
#define RELEASE "rpm-1.3"
#define REV "rpm-1.3"

//Master tClient
#define TCLIENT "tClient"
#define TAUTHORIZE "tAuthorize"
//#define TCLIENT "mysqlisp2.tClient"
//#define TAUTHORIZE "mysqlisp2.tAuthorize"
//

