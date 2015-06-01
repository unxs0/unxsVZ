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
DESIGN NOTES
	No dynamic memory operations are used to avoid memory leakage and cleanup issues. 
	We do not care about efficient memory use, we care about reliability and speed.
	
AUTHOR/LEGAL
	(C) 2013 Gary Wallis for Unixservice, LLC.
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

typedef struct {
	char cIP[32];
	unsigned uPort;
	unsigned uPriority;
	unsigned uWeight;
} structAddr;

#define MAX_ADDR 16
typedef struct {
	char cPrefix[32];
	unsigned uRule;
	structAddr sAddr[MAX_ADDR];
} structRule;

//Global data
#define MAX_RULES 32
#define DEFAULT_SIP_PORT 5060
structRule gsRuleTest[MAX_RULES];
MYSQL gMysql; 
unsigned guCount=0;
unsigned guServerPort=DEFAULT_SIP_PORT;
char gcServerIP[16]={"127.0.0.1"};
static FILE *gLfp=NULL;
char gcQuery[1024];
unsigned guLogLevel=6;//1 errors, 2 warnings, 3 info, 4 debug, 5 packet capture to log file, 6 rule dump
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
void sigLoadRules(int iSigNum);


void readEv(int fd,short event,void* arg)
{

	//
	//Load message
	ssize_t len;
	char cMessage[2048]={""};
	char cMessageModified[2048]={""};
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
#include "module/parsemessage.c"
#include "module/postparsecheck.c"
#include "module/process.c"

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

	//Preload rules
	sigLoadRules(0);

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
	signal(SIGHUP,sigLoadRules);
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


void sigLoadRules(int iSigNum)
{
	//quick init
	memset((void *)&gsRuleTest[0],0,sizeof(structRule)*MAX_RULES);

	register int i;
	char cData[512];
	char cKey[32];
	size_t sizeData;
	uint32_t flags;
	unsigned uRule=0;
	memcached_return rc;
	for(i=0;i<MAX_RULES;i++)
	{
		//load from memcached
		sprintf(cKey,"%d-rule",i);
		sprintf(cData,"%.511s",memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc));
		if(rc!=MEMCACHED_SUCCESS)
		{
			logfileLine("sigLoadRules",cKey);
			break;
		}
		else
		{
			register int i;
			unsigned uLine=0;
			unsigned uAddr=0;
			char *cp=cData;
			for(i=0;i<512 && cData[i];i++)
			{
				if(cData[i]=='\n')
				{
					unsigned uRuleNum=0;
					unsigned uA=0,uB=0,uC=0,uD=0;
					unsigned uPort=0;
					unsigned uPriority=0;
					unsigned uWeight=0;
					char cPrefix[32]={""};
					char cIP[32]={""};
					char *cp1,*cp2;
					//process a line
					cData[i]=0;
					uLine++;
					if(uLine>1)
					{
						sscanf(cp,"cDestinationIP=%u.%u.%u.%u;uDestinationPort=%u;uPriority=%u,uWeight=%u;",
								&uA,&uB,&uC,&uD,&uPort,&uPriority,&uWeight);
						if(guLogLevel>3)
							logfileLine("sigLoadRules ln",cp);
						sprintf(cIP,"%u.%u.%u.%u",uA,uB,uC,uD);
						if(guLogLevel>3)
							sprintf(gcQuery,"(%u) cIP=%s;uPort=%u;uPriority=%u,uWeight=%u;",
								uAddr,
								cIP,uPort,uPriority,uWeight);
						if(uAddr<MAX_ADDR && uRule)
						{
							if(guLogLevel>3)
								logfileLine("sigLoadRules",gcQuery);
							gsRuleTest[uRule-1].sAddr[uAddr].uPort=uPort;
							gsRuleTest[uRule-1].sAddr[uAddr].uPriority=uPriority;
							gsRuleTest[uRule-1].sAddr[uAddr].uWeight=uWeight;
							sprintf(gsRuleTest[uRule-1].sAddr[uAddr].cIP,"%.31s",cIP);
							uAddr++;
						}
						else
						{
							if(guLogLevel>3)
								logfileLine("sigLoadRules Ex",gcQuery);
						}
					}
					else
					{
						sscanf(cp,"uRule=%u;",&uRuleNum);
						if((cp1=strstr(cp,"cPrefix=")))
						{
							if((cp2=strchr(cp1+strlen("cPrefix="),';')))
							{
								*cp2=0;
								sprintf(cPrefix,"%.32s",cp1+strlen("cPrefix="));
							}
						}
						gsRuleTest[uRule].uRule=uRuleNum;
						sprintf(gsRuleTest[uRule].cPrefix,"%.31s",cPrefix);
						if(guLogLevel>3)
						{
							logfileLine("sigLoadRules l1",cp);
							sprintf(gcQuery,"(%u) uRuleNum=%u;cPrefix=%s;",uRule,uRuleNum,cPrefix);
							logfileLine("sigLoadRules",gcQuery);
						}
						uRule++;
					}
					cp=cData+i+1;
				}//if newline
			}
		}
	}

	if(guLogLevel>1)
	{
		sprintf(gcQuery,"Loaded %d rules",i);
		logfileLine("sigLoadRules",gcQuery);
	}

	if(guLogLevel>5)
	{
		//debug only
		for(i=0;i<MAX_RULES;i++)
		{
			register int j=0;
		
			if(gsRuleTest[i].uRule)	
			{
				sprintf(gcQuery,"(%d) uRuleNum=%u;cPrefix=%s;",i,gsRuleTest[i].uRule,gsRuleTest[i].cPrefix);
				logfileLine("sigLoadRules rreport",gcQuery);
			}
			for(j=0;j<MAX_ADDR;j++)
			{
				if(gsRuleTest[i].sAddr[j].cIP[0] || gsRuleTest[i].sAddr[j].uPort || gsRuleTest[i].sAddr[j].uPriority
					|| gsRuleTest[i].sAddr[j].uWeight)
				{
					sprintf(gcQuery,"(%d) cIP=%s;uPort=%u;uPriority=%u,uWeight=%u;",
						j,
						gsRuleTest[i].sAddr[j].cIP,
						gsRuleTest[i].sAddr[j].uPort,
						gsRuleTest[i].sAddr[j].uPriority,
						gsRuleTest[i].sAddr[j].uWeight);
					logfileLine("sigLoadRules rreport",gcQuery);
				}
			}
		}
	}

}//void sigLoadRules(int iSigNum)
