/*
FILE
	uSIPSwitch/sipproxy.h
AUTHOR
	(C) 2012-2105 Gary Wallis for Unixservice.
*/

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <event.h>
#include <signal.h>
#include <libmemcached/memcached.h>

#include <mysql/mysql.h>
extern MYSQL gMysql; 

#define cLOGFILE "/var/log/uSIPSwitch"
#define cPIDFILE "/var/run/uSIPSwitch.pid"
#define cLOGFILE_HEALTH "/var/log/unxsSIPHealth"
#define cPIDFILE_HEALTH "/var/run/unxsSIPHealth.pid"

//struct sizes
#define MAX_ADDR 16
#define MAX_RULES 32
#define RULE_BUFFER_SIZE 4192

typedef struct {
	char cIP[32];
	unsigned uPort;
	unsigned uPriority;
	unsigned uWeight;
} structAddr;

typedef struct {
	char cPrefix[32];
	unsigned uRule;
	unsigned short usRoundRobin;
	unsigned short usQualify;
	unsigned short usNumOfAddr;
	structAddr sAddr[MAX_ADDR];
} structRule;

