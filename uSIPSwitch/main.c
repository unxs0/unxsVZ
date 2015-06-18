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

//Global data
#define DEFAULT_SIP_PORT 5060
structRule gsRuleTest[MAX_RULES];
MYSQL gMysql; 
unsigned guCount=0;
unsigned guServerPort=DEFAULT_SIP_PORT;
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
void sigLoadRules(int iSigNum);

#define uTIMERA_SECS 300
void vTimerA(int fd, short event, void *arg)
{

	//do something
	if(guLogLevel>2)
		logfileLine("vTimerA","running");

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
	signal(SIGINT,sigHandler);//removes pid file and exits daemon cleanly
	signal(SIGHUP,sigLoadRules);//load rules
	signal(SIGUSR1,sigChangeLogLevelDown);//change guLogLevel--
	signal(SIGUSR2,sigChangeLogLevelUp);//change guLogLevel++
	sprintf(gcQuery,"listening on %s:%u",gcServerIP,guServerPort);
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


void sigLoadRules(int iSigNum)
{
	//quick init
	memset((void *)&gsRuleTest[0],0,sizeof(structRule)*MAX_RULES);

	register int i;
	char cData[RULE_BUFFER_SIZE]={""};
	char cKey[32];
	size_t sizeData;
	uint32_t flags;
	unsigned uRule=0;
	memcached_return rc;
#define MAX_DB_RULE 32
	for(i=1;i<=MAX_DB_RULE;i++)
	{
		//load from memcached
		sprintf(cKey,"%d-rule",i);
		sprintf(cData,"%.4191s",memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc));
		if(rc!=MEMCACHED_SUCCESS)
		{
			//logfileLine("sigLoadRules skip",cKey);
			continue;
		}
		else
		{
			register int i;
			unsigned uLine=0;
			unsigned uAddr=0;
			char *cp=cData;
			for(i=0;i<RULE_BUFFER_SIZE && cData[i];i++)
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
								logfileLine("sigLoadRules uAddr",gcQuery);
							gsRuleTest[uRule-1].sAddr[uAddr].uPort=uPort;
							gsRuleTest[uRule-1].sAddr[uAddr].uPriority=uPriority;
							gsRuleTest[uRule-1].sAddr[uAddr].uWeight=uWeight;
							sprintf(gsRuleTest[uRule-1].sAddr[uAddr].cIP,"%.31s",cIP);
							uAddr++;
						}
						else if(!uRule)
						{
							if(guLogLevel>3)
								logfileLine("sigLoadRules","!uRule");
						}
						else if(1)
						{
							if(guLogLevel>3)
								logfileLine("sigLoadRules Ex",gcQuery);
						}
					}
					else
					{
						sscanf(cp,"uRule=%u;",&uRuleNum);
						gsRuleTest[uRule].uRule=uRuleNum;

						if((cp1=strstr(cp,";uPriority=")))
						{
							unsigned uPriority=0;
							if(sscanf(cp1,";uPriority=%u;",&uPriority)==1)
								gsRuleTest[uRule].uPriority=uPriority;
						}

						//preset cp2 also jic
						if((cp2=cp1=strstr(cp,"cPrefix=")))
						{
							if((cp2=strchr(cp1+strlen("cPrefix="),';')))
							{
								*cp2=0;
								sprintf(cPrefix,"%.32s",cp1+strlen("cPrefix="));
							}
						}
						sprintf(gsRuleTest[uRule].cPrefix,"%.31s",cPrefix);

						//cOption= must come after cPrefix
						if(strstr(cp2+strlen(cPrefix),"RoundRobin=yes"))
							gsRuleTest[uRule].usRoundRobin=1;
						else
							gsRuleTest[uRule].usRoundRobin=0;

						if(strstr(cp2+strlen(cPrefix),"Qualify=yes"))
							gsRuleTest[uRule].usQualify=1;
						else
							gsRuleTest[uRule].usQualify=0;

						if(guLogLevel>3)
						{
							//uRule=1;cLabel=Default;cPrefix=Any;uPriority=1000;cOption=RoundRobin=yes;
							logfileLine("sigLoadRules l1",cp);
							sprintf(gcQuery,"(%u) uRuleNum=%u;cPrefix=%s;uPriority=%u;usQualify=%u;usRoundRobin=%u;",
									uRule,
									gsRuleTest[uRule].uRule,
									gsRuleTest[uRule].cPrefix,
									gsRuleTest[uRule].uPriority,
									gsRuleTest[uRule].usQualify,
									gsRuleTest[uRule].usRoundRobin);
							logfileLine("sigLoadRules",gcQuery);
						}
						uRule++;
						if(uRule>=MAX_RULES)
							break;
					}
					cp=cData+i+1;
					gsRuleTest[uRule-1].usNumOfAddr=uAddr;
				}//if newline
			}
		}
	}

	if(guLogLevel>2)
	{
		sprintf(gcQuery,"Loaded %d rules",uRule);
		logfileLine("sigLoadRules",gcQuery);
	}

	//simple bubble sort (ok for now and small rule sets) based on tRule.uPriority
	//this allows the DR code to be simple
	//but requires that backend rule priority field be set correctly in a global context
	//note that tRules needs uCluster urgently!
	for(i=0;i<uRule;i++)
	{
		register int j;
		structRule tmp;
		for(j=0;j<uRule-i-1;j++)
		{
			if(gsRuleTest[j].uPriority>gsRuleTest[j+1].uPriority)
			{
				memcpy((void *)&tmp,(void *)&gsRuleTest[j],sizeof(structRule));
				memcpy((void *)&gsRuleTest[j],(void *)&gsRuleTest[j+1],sizeof(structRule));
				memcpy((void *)&gsRuleTest[j+1],(void *)&tmp,sizeof(structRule));
			}
		}
	}
     

	if(guLogLevel>2)
	{
		//debug only
		for(i=0;i<MAX_RULES;i++)
		{
			register int j=0;
		
			if(gsRuleTest[i].uRule)	
			{
				sprintf(gcQuery,"(%d) uRuleNum=%u;cPrefix=%s;uPriority=%u;usRoundRobin=%u;usQualify=%u;usNumOfAddr=%u;",
						i,
						gsRuleTest[i].uRule,
						gsRuleTest[i].cPrefix,
						gsRuleTest[i].uPriority,
						gsRuleTest[i].usRoundRobin,
						gsRuleTest[i].usQualify,
						gsRuleTest[i].usNumOfAddr);
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
