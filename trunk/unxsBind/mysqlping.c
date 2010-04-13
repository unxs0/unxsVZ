/*
FILE
	$Id$
PURPOSE
	Test verifying which server is reachable before mysql connect;
AUTHOR
	(C) 2010 Gary Wallis for Unixservice, LLC.
NOTES
	We must be able to handle mixed local socket and IP based
	connections. TODO
*/

#include "mysqlrad.h"
#include "local.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

MYSQL gMysql;


int main()
{

	char *cIP0="127.0.0.1";
	char *cPort="3306";
	char *cIP1="127.0.0.1";
	int sock;
	int domain0=PF_INET;//PF_UNIX
	int domain1=PF_INET;//PF_UNIX
	unsigned uUseIP=0;
	struct sockaddr_in sockaddr_inMySQLServer;

	if(DBPORT==0)
		cPort="3306";
	else
		sprintf(cPort,"%u",DBPORT);

	if(DBIP0==NULL)
		cIP0="127.0.0.1";
	else
		cIP0=DBIP0;

	if(DBIP1==NULL)
		cIP1="127.0.0.1";
	else
		cIP1=DBIP1;


	mysql_init(&gMysql);

	//Determine which IP is up at given port
	//DBIP0
	if((sock=socket(domain0,SOCK_STREAM,IPPROTO_TCP))<0)
	{
		printf("Could not create test ip:port socket\n");
		exit(1);
	}
	//Depending on domain0 this section varies
	memset(&sockaddr_inMySQLServer, 0, sizeof(sockaddr_inMySQLServer));
	sockaddr_inMySQLServer.sin_family = AF_INET;
	sockaddr_inMySQLServer.sin_addr.s_addr = inet_addr(cIP0);
	sockaddr_inMySQLServer.sin_port = htons(atoi(cPort));
	if(connect(sock,(struct sockaddr *) &sockaddr_inMySQLServer,sizeof(sockaddr_inMySQLServer))<0)
	{
		uUseIP=0;
	}
	else
	{
		//DBIP1
		close(sock);
		if((sock=socket(domain1,SOCK_STREAM,IPPROTO_TCP))<0)
		{
			printf("Could not create test ip:port socket\n");
			exit(1);
		}
		//Depending on domain1 this section varies
		memset(&sockaddr_inMySQLServer, 0, sizeof(sockaddr_inMySQLServer));
		sockaddr_inMySQLServer.sin_family = AF_INET;
		sockaddr_inMySQLServer.sin_addr.s_addr = inet_addr(cIP1);
		sockaddr_inMySQLServer.sin_port = htons(atoi(cPort));
		if(connect(sock,(struct sockaddr *) &sockaddr_inMySQLServer,sizeof(sockaddr_inMySQLServer))<0)
		{
			uUseIP=1;
		}
	}
	close(sock);

	//Depending on live IP try to connect
	if(uUseIP)
	{
		if (!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
			exit(1);
		else
			printf("Connected to %s:%s\n",cIP1,cPort);
	}
	else
	{
		if (!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
			exit(1);
		else
			printf("Connected to %s:%s\n",cIP0,cPort);
	}

	mysql_close(&gMysql);
	exit(0);

}//main()
