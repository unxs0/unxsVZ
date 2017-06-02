/*
FILE
	svn ID removed
PURPOSE
	Wrapper for mysql_real_connect() that supports very fast
	connect to main or alternative local.h set MySQL servers.
AUTHOR
	(C) 2010-2014 Gary Wallis for Unixservice, LLC.
NOTES
	Based on unxsBind/mysqlping.c test code.
*/

#include "mysqlrad.h"
#include "local.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>

//This is an important setting that depends on your network setup
#define SELECT_TIMEOUT_USEC 500000
#define SELECT_UBC_TIMEOUT_USEC 800000

//TOC protos
void ConnectDb(char *cMessage);
unsigned TextConnectDb(void);
unsigned ConnectDbUBC(void);


void ConnectDb(char *cMessage)
{
	//Handle quick cases first
	//Port is irrelevant here. Make it clear.
	mysql_init(&gMysql);
	if(!DBIP0[0])
	{
		if (mysql_real_connect(&gMysql,NULL,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
			return;
	}
	if(!DBIP1[0])
	{
		if (mysql_real_connect(&gMysql,NULL,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
			return;
	}

	//Now we can use AF_INET/IPPROTO_TCP cases (TCP connections via IP number)
	char cPort[16]={"3306"};//(*1)
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
			sprintf(cMessage,"Could not create ConnectDB() socket DBIP0");
			return;
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
			sprintf(cMessage,"Could not create ConnectDB() socket DBIP1");
			return;
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
	if(DBIP1[0] && DBIP0[0])
		sprintf(cMessage,"Could not connect to %1$s:%3$s or %2$s:%3$s\n",DBIP0,DBIP1,cPort);
	else if(!DBIP1[0] && !DBIP0[0])
		sprintf(cMessage,"Could not connect to local socket\n");
	else if(DBIP0[0] && !DBIP1[0])
		sprintf(cMessage,"Could not connect to %s:%s or local socket (DBIP1)\n",DBIP0,cPort);
	else if(!DBIP0[0] && DBIP1[0])
		sprintf(cMessage,"Could not connect to %s:%s or local socket (DBIP0)\n",DBIP1,cPort);
	else if(1)
		sprintf(cMessage,"Could not connect unexpected case\n");

}//void ConnectDb(char *cMessage)


unsigned TextConnectDb(void)
{
	//Handle quick cases first
	//Port is irrelevant here. Make it clear.
	mysql_init(&gMysql);
	if(!DBIP0[0])
	{
		if (mysql_real_connect(&gMysql,NULL,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
			return(0);
	}
	if(!DBIP1[0])
	{
		if (mysql_real_connect(&gMysql,NULL,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
			return(0);
	}

	//Now we can use AF_INET/IPPROTO_TCP cases (TCP connections via IP number)
	char cPort[16]={"3306"};//(*1)
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
			printf("Could not create TextConnectDB() socket DBIP0\n");
			return(1);
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
						if(mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,
											DBNAME,DBPORT,DBSOCKET,0))
							return(0);
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
			printf("Could not create TextConnectDB() socket DBIP1\n");
			return(1);
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
						if(mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,
											DBNAME,DBPORT,DBSOCKET,0))
							return(0);
					}
				} 
			} 
		}
		close(iSock);//Don't need anymore.
	}

	//Failure exit 4 cases
	char cMessage[256];
	if(DBIP1[0] && DBIP0[0])
		sprintf(cMessage,"Could not connect to %1$s:%3$s or %2$s:%3$s\n",DBIP0,DBIP1,cPort);
	else if(!DBIP1[0] && !DBIP0[0])
		sprintf(cMessage,"Could not connect to local socket\n");
	else if(DBIP0[0] && !DBIP1[0])
		sprintf(cMessage,"Could not connect to %s:%s or local socket (DBIP1)\n",DBIP0,cPort);
	else if(!DBIP0[0] && DBIP1[0])
		sprintf(cMessage,"Could not connect to %s:%s or local socket (DBIP0)\n",DBIP1,cPort);
	else if(1)
		sprintf(cMessage,"Could not connect unexpected case\n");

	printf(cMessage);
	return(1);

}//unsigned TextConnectDb()


unsigned ConnectDbUBC(void)
{
	//Handle quick cases first
	//Port is irrelevant here. Make it clear.
	//local.h order is important for best performance local should be first
	mysql_init(&gMysqlUBC);
	if(!gcUBCDBIP0[0])
	{
		if (mysql_real_connect(&gMysqlUBC,NULL,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
			return(0);
		else
			logfileLine("unxsVZ:ConnectDbUBC","null or empty fail 0");
	}
	if(!gcUBCDBIP1[0])
	{
		if (mysql_real_connect(&gMysqlUBC,NULL,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
			return(0);
		else
			logfileLine("unxsVZ:ConnectDbUBC","null or empty fail 1");
	}

	//Now we can use AF_INET/IPPROTO_TCP cases (TCP connections via IP number)
	char cPort[16]={"3306"};//(*1)
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

	if(gcUBCDBIP0[0])
	{
		if((iSock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		{
			logfileLine("unxsVZ:ConnectDbUBC","Could not create socket gcUBCDBIP0");
			return(1);
		}

		// Set non-blocking 
		lFcntlArg=fcntl(iSock,F_GETFL,NULL); 
		lFcntlArg|=O_NONBLOCK; 
		fcntl(iSock,F_SETFL,lFcntlArg); 

		//DBIP0 has priority if we can create a connection we
		//move forward immediately.
		memset(&sockaddr_inMySQLServer,0,sizeof(sockaddr_inMySQLServer));
		sockaddr_inMySQLServer.sin_family=AF_INET;
		sockaddr_inMySQLServer.sin_addr.s_addr=inet_addr(gcUBCDBIP0);
		sockaddr_inMySQLServer.sin_port=htons(atoi(cPort));
		iConRes=connect(iSock,(struct sockaddr *)&sockaddr_inMySQLServer,sizeof(sockaddr_inMySQLServer));
		if(iConRes<0)
		{
			if(errno==EINPROGRESS)
			{
				tv.tv_sec=0; 
				tv.tv_usec=SELECT_UBC_TIMEOUT_USEC; 
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
						if(mysql_real_connect(&gMysqlUBC,gcUBCDBIP0,DBLOGIN,DBPASSWD,
											DBNAME,DBPORT,DBSOCKET,0))
						{
							logfileLine("unxsVZ:ConnectDbUBC","gcUBCDBIP0 ok");
							return(0);
						}
						else
						{
							logfileLine("unxsVZ:ConnectDbUBC","gcUBCDBIP0 fail");
						}
					}
				} 
			} 
		}
		close(iSock);//Don't need anymore.
	}

	if(gcUBCDBIP1[0])
	{
		if((iSock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		{
			logfileLine("unxsVZ:ConnectDbUBC","Could not create socket gcUBCDBIP1");
			return(1);
		}

		// Set non-blocking 
		lFcntlArg=fcntl(iSock,F_GETFL,NULL); 
		lFcntlArg|=O_NONBLOCK; 
		fcntl(iSock,F_SETFL,lFcntlArg); 

		//Fallback to DBIP1
		memset(&sockaddr_inMySQLServer,0,sizeof(sockaddr_inMySQLServer));
		sockaddr_inMySQLServer.sin_family=AF_INET;
		sockaddr_inMySQLServer.sin_addr.s_addr=inet_addr(gcUBCDBIP1);
		sockaddr_inMySQLServer.sin_port=htons(atoi(cPort));
		iConRes=connect(iSock,(struct sockaddr *)&sockaddr_inMySQLServer,sizeof(sockaddr_inMySQLServer));
		if(iConRes<0)
		{
			if(errno==EINPROGRESS)
			{
				tv.tv_sec=0; 
				tv.tv_usec=SELECT_UBC_TIMEOUT_USEC; 
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
						if(mysql_real_connect(&gMysqlUBC,gcUBCDBIP1,DBLOGIN,DBPASSWD,
											DBNAME,DBPORT,DBSOCKET,0))
						{
							logfileLine("unxsVZ:ConnectDbUBC","gcUBCDBIP1 ok");
							return(0);
						}
						else
						{
							logfileLine("unxsVZ:ConnectDbUBC","gcUBCDBIP1 fail");
						}
					}
				} 
			} 
		}
		close(iSock);//Don't need anymore.
	}

	//Failure exit 4 cases
	char cMessage[256];
	if(gcUBCDBIP1[0] && gcUBCDBIP0[0])
		sprintf(cMessage,"Could not connect to gcUBCDBIP0:%1$s:%2$s or gcUBCDBIP1:%3$s:%2$s\n",gcUBCDBIP0,cPort,gcUBCDBIP1);
	else if(!gcUBCDBIP1[0] && !gcUBCDBIP0[0])
		sprintf(cMessage,"Could not connect to local socket\n");
	else if(gcUBCDBIP0[0] && !gcUBCDBIP1[0])
		sprintf(cMessage,"Could not connect to gcUBCDBIP0:%s:%s or local socket (gcUBCDBIP1)\n",gcUBCDBIP0,cPort);
	else if(!gcUBCDBIP0[0] && gcUBCDBIP1[0])
		sprintf(cMessage,"Could not connect to gcUBCDBIP1:%s:%s or local socket (gcUBCDBIP0)\n",gcUBCDBIP1,cPort);
	else if(1)
		sprintf(cMessage,"Could not connect unexpected case\n");

	logfileLine("unxsVZ:ConnectDbUBC",cMessage);
	return(1);

}//unsigned ConnectDbUBC()

