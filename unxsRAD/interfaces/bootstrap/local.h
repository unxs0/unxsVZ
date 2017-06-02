/*
FILE
        unxsRAD/interface/bootstrap/local.h.default
PURPOSE
        Localization constants, configuration settings and release info.
LEGAL
        (C) 2001-2017 Gary Wallis for Unixservice, LLC.
*/

#define LOCALCOPYRIGHT "&copy; 2001-2017 Unixservice, LLC. All Rights Reserved."

//Change for your setup DO NOT USE THESE DEFAULTS: DANGER!
//DBIP0 an DBIP1 can not be NULL amymore. Empty string is the same as NULL.
#define DBIP0 ""
#define DBIP1 ""
#define DBNAME "unxsrad"
#define DBLOGIN "unxsrad"
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

//You should customize these for your own ISP
#define EXTISPNAME "unxs.io"
#define EXTISPURL "unxs.io"
#define HEADER_TITLE "unxsRAD"

#define RELEASE "1.0"
#define REV ".1"

//For interfaces
#define cMAILTO "noc@someisp.net"
#define cBCC ""
#define cFROM "noc@someisp.net"
#define cSUBJECT "mobile.cgi system message"
#define cUNXSVZORGLOGFILE "/var/log/mobile.cgi.log"

