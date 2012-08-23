/*
FILE 
	unxsVZ/unxsSIPProxy/main.c
	$Id$
PURPOSE
	Listen for SIP traffic. Based on destination we forward traffic
	to the correct SIP server and return answers to incoming message source. If no message
	is available send appropiate SIP error message.

	The SIP server port and IP that we relay the data to will come from a table, each
	SIP server may have several IP, port tuples available that will be tried in a preference
	based manner. We can also mark tuples as not available if certain SIP protocol expected
	answers fail a certain number of times.

	We will use a SIP server table with IPs, ports and preference. This will initially be used directly
	from MySQL but later we will libmemcache it.

	The table will be created from another hostname based table that will gather
	the IPs, ports and preference info from DNS. This will be then memcached for
	high speed access.

	Simple rules mapping SIP servers to SIP servers will then be implemented.

	We need to keep track of SIP transactions or maybe a only limited subset of dialogues.

INITIAL ARCHITECTURE

	Build DNS based gateway, server IP, port, preference table. A signal can rebuild this
	table during run time. The table can also have non DNS based static IP and port and preference
	tuples if no noted in the source gateway tables by using IPv4 instead of hostnames.
	
	The table is a libmemcached construct.
	
AUTHOR/LEGAL
	(C) 2012 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
	Includes code based on public domain beej.us networking source code.
	Libevent sections based on public domain libevent-test.c by Brian Smith
OTHER
	Only tested on CentOS 5.
	yum install libmemcached, memcached, libevent
FREE HELP
	support @ openisp . net
	supportgrp @ unixservice . com
	Join mailing list: https://lists.openisp.net/mailman/listinfo/unxsvz
*/

#include "sipproxy.h"


//Global data
MYSQL gMysql; 
unsigned guCount=0;
unsigned guServerPort=5060;
unsigned guClientPort=5060;
char gcServerIP[16]={"127.0.0.1"};
char gcClientIP[16]={""};
static FILE *gLfp=NULL;
char gcQuery[1024];

//TOC
void readEv(int fd,short event,void* arg);
void logfileLine(const char *cFunction,const char *cLogline);
void daemonize(void);
void sigHandler(int iSignum);
int iCheckLibEventVersion(void);
int iSetupAndTestMemcached(void);


void readEv(int fd,short event,void* arg)
{
	ssize_t len;
	char cPacket[2048]={""};
	//struct event *ev = arg;

	socklen_t l=sizeof(struct sockaddr);
	struct sockaddr_in cAddr;

	len=recvfrom(fd,(void *)cPacket,2047,0,(struct sockaddr*)&cAddr,&l);

	if(len== -1)
	{
		perror("recvfrom()");
		logfileLine("readEv","recvfrom()");
		return;
	}
	else if(len==0)
	{
		perror("connection closed");
		logfileLine("readEv","connection closed");
		return;
	}

	guCount++;

	//
	//Response
	//Decide on which response message to send
	//This is based on the type of message and also based on the memcached routing table
	//Examples: No matter what message, if cIPStr is not a PBX or Carrier gateway we can ignore and return before socket()
	//	if did@gw yields no match for INVITE we can return a 400 response.
	//	if we forward to gw then we return a 100 response.
	//To the server the message came from
	register int sd,rc;
	struct sockaddr_in remoteServAddr;
	char cMsg[100]={"SIP/2.0 200 OK\n"};
	char cIPStr[INET_ADDRSTRLEN];
	unsigned uPort=ntohs(cAddr.sin_port);
	//Get other side IP number and create quickest socket
	inet_ntop(AF_INET,&cAddr.sin_addr,cIPStr,sizeof(cIPStr));
	inet_aton(cIPStr,&remoteServAddr.sin_addr);
	remoteServAddr.sin_family=AF_INET;
	remoteServAddr.sin_port=htons(uPort);
	sd=socket(AF_INET,SOCK_DGRAM,0);
	if(sd<0)
	{
		perror("socket()");
		logfileLine("readEv","socket()");
		return;
	}
 	//In some cases we need to make sure we send messages from a specific IP 
	if(gcClientIP[0])
	{
		struct sockaddr_in cliAddr;

		cliAddr.sin_family=AF_INET;
		inet_aton(gcClientIP, &cliAddr.sin_addr);
		cliAddr.sin_port=htons(0);
		rc=bind(sd,(struct sockaddr *)&cliAddr,sizeof(cliAddr));
		if(rc<0)
		{
			perror("bind()");
			logfileLine("readEv","bind()");
			return;
		}
	}
	//Send reponse message
	rc=sendto(sd,cMsg,strlen(cMsg),0,(struct sockaddr *)&remoteServAddr,sizeof(remoteServAddr));
	if(rc<0)
	{
		perror("sendto()");
		logfileLine("readEv","sendto()");
		return;
	}
	close(sd);
	sprintf(gcQuery,"reply sent to %s:%u",cIPStr,uPort);
	logfileLine("readEv",gcQuery);

	//
	//Forward


}//void readEv(int fd,short event,void* arg)


int main(int iArgc, char *cArgv[])
{
	//This if for unxs default logging function.
	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		logfileLine("main","fopen logfile failed");
		exit(1);
	}

	if(iCheckLibEventVersion())
	{
		logfileLine("main","libevent version too old");
		exit(1);
	}

	if(iSetupAndTestMemcached())
	{
		logfileLine("main","memcached failed");
		exit(1);
	}

	int sock;
	int yes=1;
	int len=sizeof(struct sockaddr);
	struct sockaddr_in addr;
	if((sock=socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("socket()");
		logfileLine("main","socket()");
		return 1;
	}
	if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) < 0)
	{
		perror("setsockopt()");
		logfileLine("main","setsockopt()");
		return 1;
	}
	addr.sin_family=AF_INET;
	addr.sin_port=htons(guServerPort);
	if(gcServerIP[0])
		inet_aton(gcServerIP,&addr.sin_addr);
	else
		addr.sin_addr.s_addr=INADDR_ANY;
	bzero(&(addr.sin_zero),8);
	if(bind(sock,(struct sockaddr*)&addr, len)<0)
	{
		perror("bind()");
		logfileLine("main","bind()");
		return 1;
	}

	daemonize();
	signal(SIGINT|SIGHUP|SIGKILL,sigHandler);
	sprintf(gcQuery,"listening on %s:%u",gcServerIP,guServerPort);
	logfileLine("main",gcQuery);

	struct event ev;
	event_init();
	event_set(&ev, sock, EV_READ | EV_PERSIST, readEv, &ev);
	event_add(&ev, NULL);
	event_dispatch();

	//We should never reach here unless something wrong has happened
	logfileLine("main","unexpected return");
	return(1);

}//main()


void logfileLine(const char *cFunction,const char *cLogline)
{
	if(gLfp!=NULL)
	{
		time_t luClock;
		char cTime[32];
		pid_t pidThis;
		const struct tm *tmTime;

		pidThis=getpid();

		time(&luClock);
		tmTime=localtime(&luClock);
		strftime(cTime,31,"%b %d %T",tmTime);

		fprintf(gLfp,"%s unxsSIPProxy::%s[%u]: %s.\n",cTime,cFunction,pidThis,cLogline);
		fflush(gLfp);
	}
	else
	{
		fprintf(stderr,"%s: unxsSIPProxy::%s.\n",cFunction,cLogline);
	}

}//void logfileLine()


void daemonize(void)
{
	FILE *fp;

	switch(fork())
	{

		default:
			_exit(0);

		case -1:
			fprintf(stderr,"fork failed\n");
			logfileLine("daemonize","fork failed");
			_exit(1);

		case 0:
			if((fp=fopen(cPIDFILE,"w"))!=NULL)
			{
				fprintf(fp,"%u\n",getpid());
				fclose(fp);
			}
		break;
	}

	if(setsid()<0)
	{
		fprintf(stderr,"setsid failed\n");
		logfileLine("daemonize","setsid failed");
		_exit(1);
	}

}//void daemonize(void)


void sigHandler(int iSignum)
{
	sprintf(gcQuery,"guCount=%u",guCount);
	logfileLine("sigHandler",gcQuery);
	if(gLfp) fclose(gLfp);
	system("rm -f "cPIDFILE);
	exit(0);
}//void sigHandler(int iSignum)


int iCheckLibEventVersion(void)
{
	const char *v=event_get_version();
	if (!strncmp(v,"0.",2) ||
		!strncmp(v,"1.1",3) ||
		!strncmp(v,"1.2",3) ||
		!strncmp(v,"1.3",3) )
	{

		logfileLine("iCheckLibEventVersion","libevent is very old. Consider upgrading.");
		return(-1);
	}
	else
	{
		sprintf(gcQuery,"running with Libevent version %s",v);
		logfileLine("iCheckLibEventVersion",gcQuery);
		return(0);
	}
}//int iCheckLibEventVersion(void)


int iSetupAndTestMemcached(void)
{
	memcached_server_st *servers = NULL;
	memcached_st *memc;
	memcached_return rc;
	char *key= "unxsSIPProxy";
	char *value= "$Id$";

	memcached_server_st *memcached_servers_parse(const char *server_strings);
	memc=memcached_create(NULL);

	servers=memcached_server_list_append(servers,"localhost",11211,&rc);
	rc=memcached_server_push(memc, servers);
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(gcQuery,"couldn't add server: %s",memcached_strerror(memc, rc));
		logfileLine("iSetupAndTestMemcached",gcQuery);
		return(-1);
	}

	rc=memcached_set(memc,key,strlen(key),value,strlen(value),(time_t)0,(uint32_t)0);
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(gcQuery,"couldn't store test key: %s",memcached_strerror(memc, rc));
		logfileLine("iSetupAndTestMemcached",gcQuery);
		return(-1);
	}

	char cValue[100]={""};
	size_t size=100;
	uint32_t flags=0;
	sprintf(cValue,"%.99s",memcached_get(memc,key,strlen(key),&size,&flags,&rc));
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(gcQuery,"couldn't retrieve test key: %s",memcached_strerror(memc, rc));
		logfileLine("iSetupAndTestMemcached",gcQuery);
		return(-1);
	}

	if(strncmp(cValue,value,size))
	{
		sprintf(gcQuery,"keys differ: (%s) (%s)",cValue,value);
		logfileLine("iSetupAndTestMemcached",gcQuery);
		return(-1);
	}
	logfileLine("iSetupAndTestMemcached","memcached running");

	return(0);

}//int iSetupAndTestMemcached(void)

