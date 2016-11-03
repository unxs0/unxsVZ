/*
FILE
	svn ID removed
PURPOSE
	Wrapper for mysql_real_connect() that supports very fast
	connect to main or alternative local.h set MySQL servers.
AUTHOR
	(C) 2010 Gary Wallis for Unixservice, LLC.
NOTES
	Based on unxsBind/mysqlping.c test code.
*/

#include "../../mysqlrad.h"
#include "../../local.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>

//This is an important setting that depends on your network setup
#define SELECT_TIMEOUT_USEC 600000

//extern protos
void logfileLine0(const char *cFunction,const char *cLogline,const unsigned uContainer);

extern MYSQL gMysqlUBC;
extern char *gcUBCDBIP0;
extern char *gcUBCDBIP1;
extern char gcUBCDBIP0Buffer[];
extern char gcUBCDBIP1Buffer[];
extern FILE *gLfp0;
extern char gcQuery[];

//TOC protos
void TextConnectDb0(void);
void TextConnectDbUBC(void);
void ConnectToOptionalUBCDb0(unsigned uDatacenter);


void TextConnectDb0(void)
{
	//Handle quick cases first
	//Port is irrelevant here. Make it clear.
	mysql_init(&gMysql);
	if(DBIP0==NULL)
	{
		if (mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
			return;
	}
	if(DBIP1==NULL)
	{
		if (mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
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

	if(DBIP0!=NULL)
	{
		if((iSock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		{
			logfileLine0("TextConnectDb0","Could not create socket DBIP0",0);
			if(rmdir("/tmp/ubc.lock"))
				logfileLine0("TextConnectDb0","could not rmdir(/tmp/ubc.lock)",0);
			exit(1);
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

	if(DBIP1!=NULL)
	{
		if((iSock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		{
			logfileLine0("TextConnectDb0","Could not create socket DBIP1",0);
			if(rmdir("/tmp/ubc.lock"))
				logfileLine0("TextConnectDb0","could not rmdir(/tmp/ubc.lock)",0);
			exit(1);
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
	char cMessage[256];
	if(DBIP1!=NULL && DBIP0!=NULL)
		sprintf(cMessage,"Could not connect to DBIP0:%1$s or DBIP1:%1$s\n",cPort);
	else if(DBIP1==NULL && DBIP0==NULL)
		sprintf(cMessage,"Could not connect to local socket\n");
	else if(DBIP0!=NULL && DBIP1==NULL)
		sprintf(cMessage,"Could not connect to DBIP0:%s or local socket (DBIP1)\n",cPort);
	else if(DBIP0==NULL && DBIP1!=NULL)
		sprintf(cMessage,"Could not connect to DBIP1:%s or local socket (DBIP0)\n",cPort);
	else if(1)
		sprintf(cMessage,"Could not connect unexpected case\n");

	logfileLine0("TextConnectDb0",cMessage,0);
	if(rmdir("/tmp/ubc.lock"))
		logfileLine0("TextConnectDb0","could not rmdir(/tmp/ubc.lock)",0);
	exit(1);

}//TextConnectDb0()


void TextConnectDbUBC(void)
{
	//Handle quick cases first
	//Port is irrelevant here. Make it clear.
	mysql_init(&gMysqlUBC);
	if(gcUBCDBIP0==NULL)
	{
		if (mysql_real_connect(&gMysqlUBC,gcUBCDBIP0,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
			return;
	}
	if(gcUBCDBIP1==NULL)
	{
		if (mysql_real_connect(&gMysqlUBC,gcUBCDBIP1,DBLOGIN,DBPASSWD,DBNAME,0,DBSOCKET,0))
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

	if(gcUBCDBIP0!=NULL)
	{
		if((iSock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		{
			logfileLine0("TextConnectDbUBC","Could not create TextConnectDbUBC() socket gcUBCDBIP0",0);
			exit(1);
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
						if(mysql_real_connect(&gMysqlUBC,gcUBCDBIP0,DBLOGIN,DBPASSWD,
											DBNAME,DBPORT,DBSOCKET,0))
							return;
					}
				} 
			} 
		}
		close(iSock);//Don't need anymore.
	}

	if(gcUBCDBIP1!=NULL)
	{
		if((iSock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		{
			logfileLine0("TextConnectDb","Could not create TextConnectDbUBC() socket gcUBCDBIP1",0);
			exit(1);
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
						if(mysql_real_connect(&gMysqlUBC,gcUBCDBIP1,DBLOGIN,DBPASSWD,
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
	if(gcUBCDBIP1!=NULL && gcUBCDBIP0!=NULL)
		sprintf(cMessage,"Could not connect to gcUBCDBIP0:%1$s:%2$s or gcUBCDBIP1:%3$s:%2$s",gcUBCDBIP0Buffer,cPort,gcUBCDBIP1Buffer);
	else if(gcUBCDBIP1==NULL && gcUBCDBIP0==NULL)
		sprintf(cMessage,"Could not connect to local socket");
	else if(gcUBCDBIP0!=NULL && gcUBCDBIP1==NULL)
		sprintf(cMessage,"Could not connect to gcUBCDBIP0:%s or local socket (gcUBCDBIP1)",cPort);
	else if(gcUBCDBIP0==NULL && gcUBCDBIP1!=NULL)
		sprintf(cMessage,"Could not connect to gcUBCDBIP1:%s or local socket (gcUBCDBIP0)",cPort);
	else if(1)
		sprintf(cMessage,"Could not connect unexpected case");

	logfileLine0("TextConnectDbUBC",cMessage,0);
	exit(1);

}//TextConnectDbUBC()


void ConnectToOptionalUBCDb0(unsigned uDatacenter)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	//UBC MySQL server per datacenter option. Get db IPs
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u"
				" AND uType=1"
				" AND cName='gcUBCDBIP0'"
						,uDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine0("ConnectToOptionalUBCDb0",mysql_error(&gMysql),uDatacenter);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		unsigned uA=0,uB=0,uC=0,uD=0;
		if(sscanf(field[0],"%*u.%*u.%*u.%*u Public %u.%u.%u.%u",&uA,&uB,&uC,&uD)==4)
		{
			sprintf(gcUBCDBIP0Buffer,"%u.%u.%u.%u",uA,uB,uC,uD);
			gcUBCDBIP0=gcUBCDBIP0Buffer;
			logfileLine0("ConnectToOptionalUBCDb0",gcUBCDBIP0Buffer,uDatacenter);
		}
	}
	sprintf(gcQuery,"SELECT cValue FROM tProperty WHERE uKey=%u"
				" AND uType=1"
				" AND cName='gcUBCDBIP1'"
						,uDatacenter);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		logfileLine0("ConnectToOptionalUBCDb0",mysql_error(&gMysql),uDatacenter);
		mysql_close(&gMysql);
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		unsigned uA=0,uB=0,uC=0,uD=0;
		if(sscanf(field[0],"%*u.%*u.%*u.%*u Public %u.%u.%u.%u",&uA,&uB,&uC,&uD)==4)
		{
			sprintf(gcUBCDBIP1Buffer,"%u.%u.%u.%u",uA,uB,uC,uD);
			gcUBCDBIP1=gcUBCDBIP1Buffer;
			logfileLine0("ConnectToOptionalUBCDb0",gcUBCDBIP1Buffer,uDatacenter);
		}
	}
	//If gcUBCDBIP1 or gcUBCDBIP1 exist then we will use another MySQL db for UBC tProperty
	//	data
	TextConnectDbUBC();

}//void ConnectToOptionalUBCDb0()


void logfileLine0(const char *cFunction,const char *cLogline,const unsigned uContainer)
{
	FILE *fp=stdout;

	if(gLfp0!=NULL)
		fp=gLfp0;

	time_t luClock;
	char cTime[32];
	pid_t pidThis;
	const struct tm *tmTime;

	pidThis=getpid();

	time(&luClock);
	tmTime=localtime(&luClock);
	strftime(cTime,31,"%b %d %T",tmTime);

	fprintf(fp,"%s unxsDiskUtil.%s[%u]: %s.",cTime,cFunction,pidThis,cLogline);
	if(uContainer)
		fprintf(fp," %u",uContainer);
	fprintf(fp,"\n");
	fflush(fp);

}//void logfileLine0()

