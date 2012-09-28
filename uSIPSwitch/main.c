/*
FILE 
	unxsVZ/uSIPSwitch/main.c
	$Id$
PURPOSE
	Develop simple proxy for sharing port 5060 with
	private LAN IP PBXs.

	Then extend for full SBC gateway proxy with
	CDR support.

	Development info moved to NOTES file.
	
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
char gcServerIP[16]={"127.0.0.1"};
static FILE *gLfp=NULL;
char gcQuery[1024];
unsigned guLogLevel=5;//1 errors, 2 warnings, 3 info, 4 debug, 5 packet capture to log file
memcached_st *gsMemc;
int giSock;

//TOC
void readEv(int fd,short event,void* arg);
void logfileLine(const char *cFunction,const char *cLogline);
void daemonize(void);
void sigHandler(int iSignum);
int iCheckLibEventVersion(void);
int iSetupAndTestMemcached(void);
int iSendUDPMessage(char *cMsg,char *cIP,unsigned uPort);


void readEv(int fd,short event,void* arg)
{

	//
	//Load message
	ssize_t len;
	char cMessage[2048]={""};
	socklen_t l=sizeof(struct sockaddr);
	struct sockaddr_in sourceAddr;
	len=recvfrom(fd,(void *)cMessage,2047,0,(struct sockaddr*)&sourceAddr,&l);
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
	//Silently discard keep alive and other non SIP protocol packets
	//until we really know what to do with them.
	else if(len<5)
	{
		if(guLogLevel>3)
			logfileLine("readEv","small packet discarded");
		return;
	}

	guCount++;

	//	
	//Save source IP and port for future use.
	//
	char cSourceIP[INET_ADDRSTRLEN]={""};
	char cDestinationIP[256]={""};
	unsigned uSourcePort=ntohs(sourceAddr.sin_port);
	unsigned uDestinationPort=0;
	inet_ntop(AF_INET,&sourceAddr.sin_addr,cSourceIP,sizeof(cSourceIP));

	if(guLogLevel>3 && gLfp!=NULL)
		fprintf(gLfp,"[%s]\n",cMessage);

//symbolic link your SIP implementation files to module
#include "module/parsemessage.h"
#include "module/postparsecheck.h"
#include "module/process.h"

}//void readEv(int fd,short event,void* arg)


int main(int iArgc, char *cArgv[])
{

	if(iArgc!=3)
	{
		printf("usage: %s <listen ipv4> <listen port>\n",cArgv[0]);
		exit(0);
	}

	if(cArgv[1][0])
		sprintf(gcServerIP,"%.15s",cArgv[1]);
	if(cArgv[2][0])
		sscanf(cArgv[2],"%u",&guServerPort);

	if(guServerPort==0 || guServerPort>9999 || guServerPort<1025)
	{
		printf("usage: %s <listen ipv4> <listen port>\n",cArgv[0]);
		exit(0);
	}

	//debug only
	//printf("gcServerIP=%s guServerPort=%u\n",gcServerIP,guServerPort);
	//exit(0);

	//This if for unxs default logging function.
	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		perror(cLOGFILE);
		logfileLine("main","fopen logfile failed");
		exit(1);
	}

	if(guLogLevel>3)
		logfileLine("main","started");

	FILE *fp;
	if((fp=fopen(cPIDFILE,"r"))!=NULL)
	{
		perror("uSIPSwitch pid file exists");
		logfileLine("main","uSIPSwitch may already be running");
		fclose(fp);
		exit(1);
	}

	if(guLogLevel>3)
		logfileLine("main pid file created: ",cPIDFILE);

	if(iCheckLibEventVersion())
	{
		perror("libevent too old");
		logfileLine("main","libevent version too old");
		exit(1);
	}

	if(iSetupAndTestMemcached())
	{
		perror("memcached failed");
		logfileLine("main","memcached failed");
		exit(1);
	}

	int yes=1;
	int len=sizeof(struct sockaddr);
	struct sockaddr_in addr;
	if((giSock=socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("socket()");
		logfileLine("main","socket()");
		return 1;
	}
	if(setsockopt(giSock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) < 0)
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
	if(bind(giSock,(struct sockaddr*)&addr, len)<0)
	{
		perror("bind()");
		logfileLine("main","bind()");
		return 1;
	}

	daemonize();
	if(guLogLevel>3)
		logfileLine("main","forked");
	signal(SIGINT,sigHandler);
	sprintf(gcQuery,"listening on %s:%u",gcServerIP,guServerPort);
	logfileLine("main",gcQuery);

	struct event ev;
	event_init();
	event_set(&ev,giSock,EV_READ|EV_PERSIST,readEv,&ev);
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

		fprintf(gLfp,"%s uSIPSwitch[%u]::%s %s.\n",cTime,pidThis,cFunction,cLogline);
		fflush(gLfp);
	}
	else
	{
		fprintf(stderr,"%s: uSIPSwitch::%s.\n",cFunction,cLogline);
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
	memcached_return rc;
	char *key= "uSIPSwitch";
	char *value= "$Id$";

	memcached_server_st *memcached_servers_parse(const char *server_strings);
	gsMemc=memcached_create(NULL);

	servers=memcached_server_list_append(servers,"localhost",11211,&rc);
	rc=memcached_server_push(gsMemc, servers);
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(gcQuery,"couldn't add server: %s",memcached_strerror(gsMemc, rc));
		logfileLine("iSetupAndTestMemcached",gcQuery);
		return(-1);
	}

	rc=memcached_set(gsMemc,key,strlen(key),value,strlen(value),(time_t)0,(uint32_t)0);
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(gcQuery,"couldn't store test key: %s",memcached_strerror(gsMemc, rc));
		logfileLine("iSetupAndTestMemcached",gcQuery);
		return(-1);
	}

	char cValue[100]={""};
	size_t size=100;
	uint32_t flags=0;
	sprintf(cValue,"%.99s",memcached_get(gsMemc,key,strlen(key),&size,&flags,&rc));
	if(rc!=MEMCACHED_SUCCESS)
	{
		sprintf(gcQuery,"couldn't retrieve test key: %s",memcached_strerror(gsMemc, rc));
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


int iSendUDPMessage(char *cMsg,char *cIP,unsigned uPort)
{
	register int rc;
	struct sockaddr_in sourceServAddr;

	inet_aton(cIP,&sourceServAddr.sin_addr);
	sourceServAddr.sin_family=AF_INET;
	sourceServAddr.sin_port=htons(uPort);

	//Send reponse message from same socket as the listner is on
	rc=sendto(giSock,cMsg,strlen(cMsg),0,(struct sockaddr *)&sourceServAddr,sizeof(sourceServAddr));
	if(rc<0)
	{
		perror("sendto()");
		logfileLine("SendUDPMessage","sendto()");
		return(3);
	}
	//close(sd);

	return(0);
}//int iSendUDPMessage(char *cMsg,char *cIP,unsigned uPort)


