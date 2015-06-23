/*
FILE 
	unxsVZ/uSIPSwitch/unxsSIPHealth/main.c
	$Id$
PURPOSE
	Develop simple daemon for marking SIP end points as up or down
	using unxsSPS data.

AUTHOR/LEGAL
	(C) 2015 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
	Includes code based on public domain beej.us networking source code.
	Libevent sections based on public domain libevent-test.c by Brian Smith
OTHER
	Only tested on CentOS 6.
	yum install libmemcached, memcached, libevent
FREE HELP
	supportgrp @ unixservice . com
	Join mailing list: https://lists.openisp.net/mailman/listinfo/unxsvz
NOTES
	This software is part of a distributed "HA/Failover" system for cluster of uSIPSwitch's

	The idea behind this daemon is to provide updated memcached IP data to uSIPSwitch
	via unxsSPS  where only a single IP is listed at any given time for a carrier gateway or
	a customer PBX. For every IP we get from DNS and fixed data entry: We mark the IP as 
	unavailable then we send an OPTIONS packet to the IP if we get a 200 OK answer back
	we mark it as available. The uSIPSwitch provisioning system via unxsLoadFromSPS then
	provides only a single IP based on availability and then based on priority and then 
	weight.

	This keeps uSIPSwitch very straightforward since it does not have to keep track of
	up and down endpoints on it's own with all the timers and data structure that this 
	would entail.

	unxsLoadFromSPS 
	How to deal with uAvailable=0 window while we wait for answer from endpoint that may never arrive.
	(Or how to avoid programming per IP timers!)
	We select a single available IP in the following manner:
	 1. Select the lowest uPriority uAvailable=1 tAddress for the given PBX or GW
	 2. If 1. returns an empty set, we then select for highest uUptime then the lowest priority uAvailable=0 all 
	    with uHealthCheckedDate+120>NOW()
	 3. If 2. returns an empty set we set to a default endpoint for a media server that has audio for all circuits
	    busy try again later type of message.
	We should also use an uUptime counter that is incremented every time an endpoint is marked available.
	This is used to select the most likely endpoint candidate while this daemon is checking in step 2.
*/

#include "../sipproxy.h"
void TextConnectDb(void);//../mysqlconnect.c

//Global data
#define DEFAULT_PORT 6060
MYSQL gMysql; 
unsigned guCount=0;
unsigned guServerPort=DEFAULT_PORT;
char gcServerIP[16]={"127.0.0.1"};
static FILE *gLfp=NULL;
char gcQuery[1024];
unsigned guLogLevel=3;//0,1 errors, 2 warnings, 3 info, 4 debug, 5 packet capture to log file, 6 rule dump
memcached_st *gsMemc;
int giSock;

//TOC
void vTimerA(int fd, short event, void *arg);
void readEv(int fd,short event,void* arg);
void logfileLine(const char *cFunction,const char *cLogline);
void daemonize(void);
void sigHandler(int iSignum);
void sigChangeLogLevelDown(int iSignum);
void sigChangeLogLevelUp(int iSignum);
int iCheckLibEventVersion(void);
int iSetupAndTestMemcached(void);
int iSendUDPMessage(char *cMsg,char *cIP,unsigned uPort);
int iSendUDPMessageWrapper(char *cMsg,char *cDestIP,unsigned uDestPort);

//
char cCSeq[64]={""};
char cCallID[100]={""};


int iSendUDPMessageWrapper(char *cMsg,char *cDestIP,unsigned uDestPort)
{
	if(!strncmp(cDestIP,gcServerIP,strlen(gcServerIP)))
	{
		logfileLine("readEv-send to same server error",cDestIP);
		return(2);
	}

	char *cp;
	if(!iSendUDPMessage(cMsg,cDestIP,uDestPort))
	{
		if(guLogLevel>2)
		{
			if((cp=strchr(cMsg,'\r'))) *cp=0;
			sprintf(gcQuery,"%.64s to %s:%u %s %.32s",cMsg,cDestIP,uDestPort,cCSeq,cCallID);
			logfileLine("send",gcQuery);
		}
		return(0);
	}
	else
	{
		if(guLogLevel>0)
		{
			if((cp=strchr(cMsg,'\r'))) *cp=0;
			sprintf(gcQuery,"%.64s failed to %s:%u",cMsg,cDestIP,uDestPort);
			logfileLine("readEv-send",gcQuery);
		}
	}
	return(1);

}//int iSendUDPMessageWrapper()

#include <ctype.h>
void vRandom32Chars(char *cOut)
{
	FILE *fp;
	if((fp=fopen("/dev/urandom","r"))==NULL)
		return;

	char cBuffer[128];
	if(fread(cBuffer,128,1,fp)!=1)
	{
		fclose(fp);
		return;
	}
	fclose(fp);
	int i,j=0;
	for(i=0;i<128 && j<32;i++)
	{
		if(!islower(cBuffer[i]) && !isdigit(cBuffer[i]))
		{
			cBuffer[i]=cBuffer[i]%16+'a';
		}

		if(islower(cBuffer[i])||isdigit(cBuffer[i]))
		{
			cOut[j]=cBuffer[i];
			j++;
		}
	}
	cOut[j]=0;

}//void vRandom32Chars(char *cOut)


void vSendOPTIONSRequest(char *cIP,unsigned uPort,char *cDate)
{
//Sample OPTIONS exchange 
//OPTIONS sip:5009@181.111.2.106:10273 SIP/2.0
//Via: SIP/2.0/UDP 69.61.19.10:5060;branch=z9hG4bK1886cc29;rport
//Max-Forwards: 70
//From: "Unknown" <sip:Unknown@69.61.19.10>;tag=as2c012818
//To: <sip:5009@181.111.2.106:10273>
//Contact: <sip:Unknown@69.61.19.10:5060>
//Call-ID: 1fb396b0676c407a17f42b9f7c7b02f1@69.61.19.10:5060
//CSeq: 102 OPTIONS
//User-Agent: asterisk
//Date: Wed, 27 May 2015 21:29:17 GMT
//Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, SUBSCRIBE, NOTIFY, INFO, PUBLISH
//Supported: replaces, timer
//Content-Length: 0
//
//
//SIP/2.0 200 OK
//Via: SIP/2.0/UDP 69.61.19.10:5060;branch=z9hG4bK1886cc29;rport=5060
//From: "Unknown" <sip:Unknown@69.61.19.10>;tag=as2c012818
//To: <sip:5009@181.111.2.106:10273>;tag=1965203006
//Call-ID: 1fb396b0676c407a17f42b9f7c7b02f1@69.61.19.10:5060
//CSeq: 102 OPTIONS
//Supported: replaces, path, timer, eventlist
//User-Agent: Grandstream HT-503  V1.1B 1.0.10.9  chip V2.2
//Allow: INVITE, ACK, OPTIONS, CANCEL, BYE, SUBSCRIBE, NOTIFY, INFO, REFER, UPDATE
//Content-Length: 0
//
	char cLargeMsg[1024]={""};
	char cMsg[128]={""};

	sprintf(cLargeMsg,"OPTIONS sip:any@%s:%u SIP/2.0\r\n",cIP,uPort);

	sprintf(cMsg,"Via: SIP/2.0/UDP %s:%u;branch=z9hG4bK1886cc29;rport\r\n",gcServerIP,guServerPort);
	strcat(cLargeMsg,cMsg);

	sprintf(cMsg,"Max-Forwards: 8\r\n");
	strcat(cLargeMsg,cMsg);

	sprintf(cMsg,"From: \"Unknown\" <sip:Unknown@%s>;tag=as2c012818\r\n",gcServerIP);
	strcat(cLargeMsg,cMsg);

	sprintf(cMsg,"To: <sip:any@%s:%u>\r\n",cIP,uPort);
	strcat(cLargeMsg,cMsg);

	sprintf(cMsg,"Contact: <sip:Unknown@%s:%u>\r\n",gcServerIP,guServerPort);
	strcat(cLargeMsg,cMsg);

	char cCallIDString[33]={"1fb396b0676c407a17f42b9f7c7b02f1"};
	vRandom32Chars(cCallIDString);
	sprintf(cMsg,"Call-ID: %s@%s:%u\r\n",cCallIDString,gcServerIP,guServerPort);
	strcat(cLargeMsg,cMsg);

	sprintf(cMsg,"CSeq: 102 OPTIONS\r\n");
	strcat(cLargeMsg,cMsg);

	sprintf(cMsg,"Supported: %s\r\n","replaces, timer");
	strcat(cLargeMsg,cMsg);

	sprintf(cMsg,"User-Agent: %s\r\n","Unixservice unxsSIPHealth v0.1");
	strcat(cLargeMsg,cMsg);

	sprintf(cMsg,"Date: %s\r\n",cDate);
	strcat(cLargeMsg,cMsg);

	sprintf(cMsg,"Allow: %s\r\n","INVITE, ACK, OPTIONS, CANCEL, BYE");
	strcat(cLargeMsg,cMsg);

	sprintf(cMsg,"Content-Length: %u\r\n",0);
	strcat(cLargeMsg,cMsg);

	iSendUDPMessageWrapper(cLargeMsg,cIP,uPort);

}//void vSendOPTIONSRequest(char *cIP,unsigned uPort,char *cDate)



#define uTIMERA_SECS 60
void vTimerA(int fd, short event, void *arg)
{

	//do something
	if(guLogLevel>2)
		logfileLine("vTimerA","running");

        MYSQL_RES *res;
        MYSQL_ROW field;

	//Date: Wed, 27 May 2015 21:29:17 GMT
	char cDate[32]={"Wed, 27 May 2015 21:29:17 GMT"};
	sprintf(gcQuery,"SELECT DATE_FORMAT(CONVERT_TZ(NOW(),'+0:0','+6:0'),'%%a, %%d %%M %%Y %%T GMT')");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		logfileLine("vTimerA",mysql_error(&gMysql));
	}
	else
	{
        	res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			logfileLine("vTimerA",field[0]);
			sprintf(cDate,"%.31s",field[0]);
		}
		mysql_free_result(res);
	}

	//do it again later
	//sprintf(gcQuery,"SELECT cIP,uPort,uAddress,FROM_UNIXTIME(uHealthCheckedDate),NOW() FROM tAddress"
	sprintf(gcQuery,"SELECT DISTINCT cIP,uPort FROM tAddress"
			" WHERE (uPBX>0 OR uGateway>0) AND (((uHealthCheckedDate+120)<UNIX_TIMESTAMP(NOW())) OR uAvailable=0)"
			" LIMIT 128");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		logfileLine("vTimerA",mysql_error(&gMysql));
	}
	else
	{
		//char cIPPrevious[32]={"nosuchip"};
		//unsigned uPortPrevious=0;
        	res=mysql_store_result(&gMysql);
		while((field=mysql_fetch_row(res)))
		{
			//logfileLine("vTimerA uHealthCheckedDate",field[3]);
			//logfileLine("vTimerA NOW()",field[4]);
			unsigned uPort=0;
			sscanf(field[1],"%u",&uPort);
			//if(!strcmp(field[0],cIPPrevious) && uPort==uPortPrevious) continue;
			//uPortPrevious=uPort;	
			//sprintf(cIPPrevious,"%.31s",field[0]);
			if(uPort && field[0][0] && cDate[0])
			{
				vSendOPTIONSRequest(field[0],uPort,cDate);
				sprintf(gcQuery,"UPDATE tAddress SET uHealthCheckedDate=UNIX_TIMESTAMP(NOW()),uAvailable=0"
						" WHERE cIP='%s' AND uPort=%u",
						field[0],uPort);
        			mysql_query(&gMysql,gcQuery);
        			if(mysql_errno(&gMysql))
					logfileLine("vTimerA",mysql_error(&gMysql));
				logfileLine("vTimerA",field[0]);
			}
		}
		mysql_free_result(res);
	}

	//do it again later
	struct event *ev = arg;
	struct timeval tvTimerA;
	tvTimerA.tv_sec = uTIMERA_SECS;
	tvTimerA.tv_usec = 0;
	evtimer_add(ev,&tvTimerA);

}//void vTimerA(int fd, short event, void *arg)


void readEv(int fd,short event,void* arg)
{

	//
	//Load message
	ssize_t len;
	char cMessage[2048]={""};
	//char cMessageModified[2048]={""};
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
	//char cDestinationIP[256]={""};
	//unsigned uSourcePort=ntohs(sourceAddr.sin_port);
	//unsigned uDestinationPort=0;
	inet_ntop(AF_INET,&sourceAddr.sin_addr,cSourceIP,sizeof(cSourceIP));

	if(guLogLevel>3 && gLfp!=NULL)
		fprintf(gLfp,"[%s]\n",cMessage);

#include "endpoint.c"

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
	if((gLfp=fopen(cLOGFILE_HEALTH,"a"))==NULL)
	{
		perror(cLOGFILE_HEALTH);
		logfileLine("main","fopen logfile failed");
		exit(1);
	}

	if(guLogLevel>3)
		logfileLine("main","started");

	FILE *fp;
	if((fp=fopen(cPIDFILE_HEALTH,"r"))!=NULL)
	{
		perror("unxsSIPHealth pid file exists");
		logfileLine("main","unxsSIPHealth may already be running");
		fclose(fp);
		exit(1);
	}

	if(guLogLevel>3)
		logfileLine("main pid file created: ",cPIDFILE_HEALTH);

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
	signal(SIGINT,sigHandler);//removes pid file and exits daemon cleanly
	signal(SIGUSR1,sigChangeLogLevelDown);//change guLogLevel--
	signal(SIGUSR2,sigChangeLogLevelUp);//change guLogLevel++
	sprintf(gcQuery,"listening on %s:%u with guLogLevel=%u",gcServerIP,guServerPort,guLogLevel);
	logfileLine("main",gcQuery);

	struct event ev;
	event_init();
	event_set(&ev,giSock,EV_READ|EV_PERSIST,readEv,&ev);
	event_add(&ev, NULL);

	//Timer A
	struct event evTimerA;
	struct timeval tvTimerA;
	tvTimerA.tv_sec = uTIMERA_SECS;
	tvTimerA.tv_usec = 0;
	evtimer_set(&evTimerA,vTimerA,&evTimerA);
	evtimer_add(&evTimerA,&tvTimerA);

	//Connect to unxsSPS MySQL
	TextConnectDb();

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

		fprintf(gLfp,"%s unxsSIPHealth[%u]::%s %s.\n",cTime,pidThis,cFunction,cLogline);
		fflush(gLfp);
	}
	else
	{
		fprintf(stderr,"%s: unxsSIPHealth::%s.\n",cFunction,cLogline);
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
			if((fp=fopen(cPIDFILE_HEALTH,"w"))!=NULL)
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
	system("rm -f "cPIDFILE_HEALTH);
	exit(0);
}//void sigHandler(int iSignum)


void sigChangeLogLevelDown(int iSignum)
{
	if(guLogLevel>0)
		guLogLevel--;
	char cMsg[128];
	sprintf(cMsg,"guLogLevel=%u",guLogLevel);
	logfileLine("sigChangeLogLevelDown",cMsg);
}
//void sigChangeLogLevelDown(int iSignum)


void sigChangeLogLevelUp(int iSignum)
{
	if(guLogLevel<6)
		guLogLevel++;
	char cMsg[128];
	sprintf(cMsg,"guLogLevel=%u",guLogLevel);
	logfileLine("sigChangeLogLevelUp",cMsg);
}
//void sigChangeLogLevelDown(int iSignum)


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
	char *key= "unxsSIPHealth";
	char *value= "$Id: main.c 2884 2015-06-02 14:01:15Z Dylan $";

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
	if(guLogLevel>2)
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
