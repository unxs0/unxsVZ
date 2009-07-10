/*
FILE
        $Id$
PURPOSE
        Localization constants
LEGAL
        Copyright 2002-2009 Gary Wallis and Hugo Urquiza for Unixservice
	GPL license applies (see www.fsf.org for more info.)
	License included with this distribution.
*/

//Set these up to match your mySQL database settings and location
//NULL is for stream socket on local server. Much faster than localhost tcp/ip.
#define DBIP0 NULL
#define DBIP1 NULL
#define DBNAME "idns"
#define DBLOGIN "idns"
#define DBPASSWD "wsxedc"
#define DBPORT 0
#define DBSOCKET NULL
//Example for host running two mysqld
//See makefile and mysqlrad.h CustomExample sections
//#define DBPORT 4306
//#define DBSOCKET "/var/lib/mysql/mysql4.sock"

//Master tClient
#define TCLIENT "tClient"
#define TAUTHORIZE "tAuthorize"
//#define TCLIENT "mysqlisp2.tClient"
//#define TAUTHORIZE "mysqlisp2.tAuthorize"

//If you have Apache/mod_ssl uncomment below much better unless intranet
//ultra firewalled and secured.
#define SSLONLY

//Set the default hostmaster for zone operations
#define HOSTMASTER "dns.unixservice.com"

//Set to your ISP name if you like
#define EXTISPNAME "unixservice.com"
#define EXTISPURL "unixservice.com"

//iDNS can report to us once a week some useful for
//debugging and development statistics.
//This is very useful to us and yourself for improving
//and developing this software
//On any mysql_error will send us the problem so we can fix it.
//#define DEBUG_REPORT_STATS_OFF

//If you would like to use your native language edit language.h or if 
//appropiate language2.h. cp it to language.h and recompile. This only covers 
//60-70% of language used. The rest can be changed in the .h schema independent 
//table related files.

//For openisp staff mod only. Staff: Remember to change makefile also.
#define RELEASE "Release 2.7/rpm-1.20"
#define REV "2.7/rpm-1.20"
