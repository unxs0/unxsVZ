/*
FILE
        $Id: local.h,v 1.11 2004/08/03 15:13:39 ggw Exp $
PURPOSE
        Localization constants
LEGAL
        Copyright 2002-2009 Gary Wallis for Unixservice
*/
//#define DBIP0 "10.10.10.100"
//#define DBIP1 "10.10.10.101"
//These two lines next should be in the public distro:
#define DBIP0 NULL
#define DBIP1 NULL
#define DBNAME "unxsradius"
#define DBLOGIN "unxsradius"
#define DBPASSWD "wsxedc"
#define DBPORT0 3306
#define DBPORT1 3306
#define DBSOCKET NULL
//Example for host running two mysqld
//See makefile and mysqlrad.h CustomExample sections
//#define DBPORT 4306
//#define DBSOCKET "/var/lib/mysql/mysql4.sock"


//mysqlSendmail can report to us once a week some useful for
//debugging and development statistics.
//This is very helpful to us and in the end we hope for yourself.
//For improving and developing this software
//#define DEBUG_REPORT_STATS_OFF

//Setup your own CA and sign a cert a start up your Apache/mod_ssl server
//for security.
//Make sure mySQL ports are firewalled correctly if used on other servers.
//VPN them?
//#define SSLONLY

#define EXTISPNAME "openisp.net"
#define EXTISPURL "openisp.net"

//For openisp staff mod only. Staff: Remember to change makefile also.
#define RELEASE "Release 2.1"
#define RELEASESHORT "2.1"
#define REV "2.1"


//Master tClient
#define TCLIENT "tClient"
//#define TCLIENT "unxsisp.tClient"
#define TAUTHORIZE "tAuthorize"
//#define TAUTHORIZE "unxsisp.tAuthorize"

//We will shortly remove all non freeradius code
#define FREE_RADIUS
