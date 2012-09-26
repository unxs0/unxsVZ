/*
FILE
	uSIPSwitch/sipproxy.h
AUTHOR
	(C) 2012 Gary Wallis for Unixservice.
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
