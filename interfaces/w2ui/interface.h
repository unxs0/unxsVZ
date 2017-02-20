/*
FILE
	interface.h
AUTHOR
	(C) 2017 Gary Wallis for Unixservice, LLC.

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
#include <unistd.h>

#include <mysql/mysql.h>
#include "local.h"


//Global vars all declared in main.c
//libtemplate.a required
extern MYSQL gMysql;
//Multipurpose buffer
extern char gcQuery[];


void InterfaceConnectDb(void);
