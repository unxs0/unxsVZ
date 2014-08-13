/*
FILE
	container-resolv/container-resolv.h
AUTHOR
	(C) 2014 Gary Wallis for Unixservice.
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
#include <signal.h>

#include <mysql/mysql.h>
extern MYSQL gMysql; 

#define cLOGFILE "/var/log/unxsContainerResolver"
#define cPIDFILE "/var/run/unxsContainerResolver.pid"
