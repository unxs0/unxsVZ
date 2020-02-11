/*
FILE
	interfaces/OneLogin/mysqlconnect.c
PURPOSE
	Wrapper for mysql_real_connect() that supports very fast
	connect to main or alternative local.h set MySQL servers.
	Interface version.
AUTHOR
	(C) 2010-2017 Gary Wallis for Unixservice, LLC.
NOTES
	Based on unxsBind/mysqlping.c test code.
*/

#include "interface.h"
#include "local.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>

//This is an important setting that depends on your network setup
#define SELECT_TIMEOUT_USEC 500000
//10 ms

//TOC protos
void InterfaceConnectDb(void);


void InterfaceConnectDb(void)
{
	//Handle quick cases first
	//Port is irrelevant here. Make it clear.
	mysql_init(&gMysql);
	if(!DBIP0[0])
	{
		if (mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
			return;
	}
	if(!DBIP1[0])
	{
		if (mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
			return;
	}

	//Now we can use AF_INET/IPPROTO_TCP cases (TCP connections via IP number)
	char *cPort="3306";//(*1)
	int iSock,iConRes;
	long lFcntlArg;
	struct sockaddr_in sockaddr_inMySQLServer;
	fd_set myset; 
	struct timeval tv; 
	int valopt;
	socklen_t lon; 

	//Default port should really be gathered from a different source
	//but for now we use the known MySQL server CentOS default port (*1).
	if(DBPORT!=0)
		sprintf(cPort,"%u",DBPORT);

	if(DBIP0[0])
	{
		if((iSock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		{
			printf("Content-type: text/plain\n\n");
			printf("Could not create ConnectDB() socket DBIP0\n");
			exit(0);
		}

		// Set non-blocking 
		lFcntlArg=fcntl(iSock,F_GETFL,NULL); 
		lFcntlArg|=O_NONBLOCK; 
		fcntl(iSock,F_SETFL,lFcntlArg); 

		//DBIP0 has priority if we can create a connection we
		//move forward immediately.
		memset(&sockaddr_inMySQLServer,0,sizeof(sockaddr_inMySQLServer));
		sockaddr_inMySQLServer.sin_family=AF_INET;
		sockaddr_inMySQLServer.sin_addr.s_addr=inet_addr(DBIP0);
		sockaddr_inMySQLServer.sin_port=htons(atoi(cPort));
		iConRes=connect(iSock,(struct sockaddr *)&sockaddr_inMySQLServer,sizeof(sockaddr_inMySQLServer));
		if(iConRes<0)
		{
			if(errno==EINPROGRESS)
			{
				tv.tv_sec=0; 
				tv.tv_usec=SELECT_TIMEOUT_USEC; 
				FD_ZERO(&myset); 
				FD_SET(iSock,&myset); 
				if(select(iSock+1,NULL,&myset,NULL,&tv)>0)
				{ 
					lon=sizeof(int); 
					getsockopt(iSock,SOL_SOCKET,SO_ERROR,(void*)(&valopt),&lon); 
					if(!valopt)
					{
						//Valid fast connection
						close(iSock);//Don't need anymore.
						mysql_init(&gMysql);
						if(mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,
											DBNAME,DBPORT,DBSOCKET,0))
							return;
					}
				} 
			} 
		}
		close(iSock);//Don't need anymore.
	}

	if(DBIP1[0])
	{
		if((iSock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		{
			printf("Content-type: text/plain\n\n");
			printf("Could not create ConnectDB() socket DBIP1\n");
			exit(0);
		}

		// Set non-blocking 
		lFcntlArg=fcntl(iSock,F_GETFL,NULL); 
		lFcntlArg|=O_NONBLOCK; 
		fcntl(iSock,F_SETFL,lFcntlArg); 

		//Fallback to DBIP1
		memset(&sockaddr_inMySQLServer,0,sizeof(sockaddr_inMySQLServer));
		sockaddr_inMySQLServer.sin_family=AF_INET;
		sockaddr_inMySQLServer.sin_addr.s_addr=inet_addr(DBIP1);
		sockaddr_inMySQLServer.sin_port=htons(atoi(cPort));
		iConRes=connect(iSock,(struct sockaddr *)&sockaddr_inMySQLServer,sizeof(sockaddr_inMySQLServer));
		if(iConRes<0)
		{
			if(errno==EINPROGRESS)
			{
				tv.tv_sec=0; 
				tv.tv_usec=SELECT_TIMEOUT_USEC; 
				FD_ZERO(&myset); 
				FD_SET(iSock,&myset); 
				if(select(iSock+1,NULL,&myset,NULL,&tv)>0)
				{ 
					lon=sizeof(int); 
					getsockopt(iSock,SOL_SOCKET,SO_ERROR,(void*)(&valopt),&lon); 
					if(!valopt)
					{
						//Valid fast connection
						close(iSock);//Don't need anymore.
						mysql_init(&gMysql);
						if(mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,
											DBNAME,DBPORT,DBSOCKET,0))
							return;
					}
				} 
			} 
		}
		close(iSock);//Don't need anymore.
	}

	//Failure exit 4 cases
	char cMessage[256];
	if(DBIP1[0] && DBIP0[0])
		sprintf(cMessage,"Could not connect to %2$s:%1$s or %3$s:%1$s\n",cPort,DBIP0,DBIP1);
	else if(!DBIP1[0] && !DBIP0[0])
		sprintf(cMessage,"Could not connect to local socket\n");
	else if(DBIP0[0] && !DBIP1[0])
		sprintf(cMessage,"Could not connect to %s:%s or local socket (DBIP1)\n",DBIP0,cPort);
	else if(!DBIP0[0] && DBIP1[0])
		sprintf(cMessage,"Could not connect to %s:%s or local socket (DBIP0)\n",DBIP0,cPort);
	else if(1)
		sprintf(cMessage,"Could not connect unexpected case\n");

	printf("Content-type: text/plain\n\n");
	printf(cMessage);
	exit(0);

}//InterfaceConnectDb()
