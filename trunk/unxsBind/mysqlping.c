/*
FILE
	$Id$
PURPOSE
	Test verifying which server is reachable before mysql_real_connect;
AUTHOR
	(C) 2010 Gary Wallis for Unixservice, LLC.
NOTES
	mysql_real_connect() handles the local PF_UNIX just fine, so we can ignore pre 
	testing in those cases.

	Once this code is tested it can replace all current unxsVZ ConnectDB() type functions.
	It was not needed before for mysqlproxy w/lua failover based installs.
*/

#include "mysqlrad.h"
#include "local.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

MYSQL gMysql;


int main()
{

	char *cPort="3306";//(*1)
	int iSock;
	struct sockaddr_in sockaddr_inMySQLServer;

	//Default port should really be gathered from a different source
	//but for now we use the known MySQL server CentOS default port (*1).
	if(DBPORT!=0)
		sprintf(cPort,"%u",DBPORT);

	//Handle quick cases first
	//Port is irrelevant here. Make it clear.
	mysql_init(&gMysql);
	if(DBIP0==NULL)
	{
		if (mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
		{
			printf("Connected to local socket DBIP0==NULL\n");
			mysql_close(&gMysql);
			exit(0);
		}
	}
	if(DBIP1==NULL)
	{
		if (mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
		{
			printf("Connected to local socket DBIP1==NULL\n");
			mysql_close(&gMysql);
			exit(0);
		}
	}

	//Now we can use AF_INET/IPPROTO_TCP cases (TCP connections via IP number)

	//This goes first. Since if it doesn't work we can't really use
	//anything that follows
	if((iSock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
	{
		printf("Could not create socket\n");
		exit(1);
	}


	if(DBIP0!=NULL)
	{
		//DBIP0 has priority if we can create a connection we
		//move forward immediately.
		memset(&sockaddr_inMySQLServer,0,sizeof(sockaddr_inMySQLServer));
		sockaddr_inMySQLServer.sin_family=AF_INET;
		sockaddr_inMySQLServer.sin_addr.s_addr=inet_addr(DBIP0);
		sockaddr_inMySQLServer.sin_port=htons(atoi(cPort));
		if(connect(iSock,(struct sockaddr *)&sockaddr_inMySQLServer,sizeof(sockaddr_inMySQLServer))>=0)
		{
			close(iSock);//Don't need anymore.
			mysql_init(&gMysql);
			if(mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
			{
				printf("Connected to %s:%s\n",(char *)DBIP0,cPort);
				mysql_close(&gMysql);
				exit(0);
			}
		}
	}

	if(DBIP1!=NULL)
	{
		//Fallback to DBIP1
		memset(&sockaddr_inMySQLServer,0,sizeof(sockaddr_inMySQLServer));
		sockaddr_inMySQLServer.sin_family=AF_INET;
		sockaddr_inMySQLServer.sin_addr.s_addr=inet_addr(DBIP1);
		sockaddr_inMySQLServer.sin_port=htons(atoi(cPort));
		if(connect(iSock,(struct sockaddr *)&sockaddr_inMySQLServer,sizeof(sockaddr_inMySQLServer))>=0)
		{
			close(iSock);//Don't need anymore.
			mysql_init(&gMysql);
			if(mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
			{
				printf("Connected to %s:%s\n",(char *)DBIP1,cPort);
				mysql_close(&gMysql);
				exit(0);
			}
		}
	}

	//Failure exit 4 cases
	if(DBIP1!=NULL && DBIP0!=NULL)
		printf("Could not connect to %s:%s or %s:%s\n",(char *)DBIP0,cPort,(char *)DBIP1,cPort);
	else if(DBIP1==NULL && DBIP0==NULL)
		printf("Could not connect. Tried to use local socket\n");
	else if(DBIP0!=NULL && DBIP1==NULL)
		printf("Could not connect to %s:%s or local socket (DBIP1)\n",(char *)DBIP0,cPort);
	else if(DBIP0==NULL && DBIP1!=NULL)
		printf("Could not connect to %s:%s or local socket (DBIP0)\n",(char *)DBIP1,cPort);
	else if(1)
		printf("Could not connect unexpected case\n");
	
	exit(1);

}//main()
