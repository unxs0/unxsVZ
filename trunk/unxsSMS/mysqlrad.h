/*
FILE
	sms/mysqlrad.h
AUTHOR
	(C) 2010 Gary Wallis for Unixservice.
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

extern MYSQL gMysql; 
