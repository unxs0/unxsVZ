/*
FILE
	mysqlrad.h
AUTHOR
	(C) 2001-2014 Gary Wallis for Unixservice, LLC.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <crypt.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/file.h>

#include <mysql/mysql.h>

#include <unistd.h>
#include <locale.h>
#include <monetary.h>

//tStatus constants
#define uACTIVE		1
#define uONHOLD		2
#define uOFFLINE	3
#define uAWAITMOD	4
#define uAWAITDEL	5
#define uAWAITACT	6
#define uCANCELED	7
#define uMODIFIED	8
#define uMODPROB	9
#define uINITSETUP	11
#define uAWAITMIG	21
#define uSTOPPED	31
#define uAWAITSTOP	41
#define uAWAITTML	51
#define uAWAITHOST	61
#define uAWAITIP	71
#define uAWAITCLONE	81
#define uAWAITFAIL	91
#define uREMOTEAPPLIANCE 101
#define uAWAITRESTART	111
#define uAWAITDNSMIG	121

//tProperty fixed types aka constants
//Actually tType constants
#define PROP_DATACENTER "1"
#define PROP_NODE "2"
#define PROP_CONTAINER "3"
#define PROP_GROUP "4"
#define PROP_GROUPTYPE "5"
#define PROP_CONFIG "6"
#define PROP_NAMESERVER "7"
#define PROP_OSTEMPLATE "8"
#define PROP_SEARCHDOMAIN "9"
#define uPROP_DATACENTER 1
#define uPROP_NODE 2
#define uPROP_CONTAINER 3
#define uPROP_GROUP 4
#define uPROP_GROUPTYPE 5
#define uPROP_CONFIG 6
#define uPROP_NAMESERVER 7
#define uPROP_OSTEMPLATE 8
#define uPROP_SEARCHDOMAIN 9
#define uPROP_RECJOB	10	

//tJobStatus constants
#define uWAITING 	1
#define uRUNNING 	2
#define uDONEOK		3
#define uDONEERROR	4
#define uSUSPENDED	5
#define uREDUNDANT	6
#define uCANCELED	7
#define uREMOTEWAITING	10
#define uERROR		14


extern MYSQL gMysql;
