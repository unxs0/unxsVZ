/*
FILE 
	unxsVZ/unxsSIPProxy/main.c
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
unsigned guLogLevel=4;//1 errors, 2 warnings, 3 info, 4 debug
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
	guCount++;

	//	
	//Save source IP and port for future use.
	//
	char cSourceIP[INET_ADDRSTRLEN]={""};
	char cDestinationIP[256]={""};
	unsigned uSourcePort=ntohs(sourceAddr.sin_port);
	unsigned uDestinationPort=0;
	inet_ntop(AF_INET,&sourceAddr.sin_addr,cSourceIP,sizeof(cSourceIP));


	//
	//Parse message
	char *cp;
	char *cp1;
/*
fprintf(gLfp,"[%s]\n",cMessage); returns this
after sipsak -v -s sip:nobody@127.0.0.1:

[OPTIONS sip:nobody@127.0.0.1 SIP/2.0
Via: SIP/2.0/UDP 72.52.75.232:44736;branch=z9hG4bK.3bcb1ce3;rport;alias
From: sip:sipsak@72.52.75.232:44736;tag=808799b
To: sip:nobody@127.0.0.1
Call-ID: 134773147@72.52.75.232
CSeq: 1 OPTIONS
Contact: sip:sipsak@72.52.75.232:44736
Content-Length: 0
Max-Forwards: 70
User-Agent: sipsak 0.9.6
Accept: text/plain

]

Note most common sip uri scheme:

From: "103" <sip:103@vcinternaltestingonly.callingcloud.net:6002>;tag=1764399065
To: "103" <sip:103@vcinternaltestingonly.callingcloud.net:6002>;tag=as2e679ce3

Notes:
	Lines are cr-lf terminated
	Last text/plain line is double cr-lf terminated.
*/


	char cFirstLine[100]={""};
	if((cp1=strchr(cMessage,'\r')))
	{
		*cp1=0;
		sprintf(cFirstLine,"%.99s",cMessage);
		*cp1='\r';
	}
	if(guLogLevel>3 && cFirstLine[0])
		logfileLine("readEv cFirstLine",cFirstLine);

	char cCallID[100]={""};
	if((cp=strstr(cMessage,"Call-ID: ")))
	{
		if((cp1=strchr(cp+strlen("Call-ID: "),'\r')))
		{
			*cp1=0;
			sprintf(cCallID,"%.99s",cp+strlen("Call-ID: "));
			*cp1='\r';
		}
	}
	if(guLogLevel>3 && cCallID[0])
		logfileLine("readEv cCallID",cCallID);
	if(guLogLevel>1 && !cCallID[0])
		logfileLine("readEv","No Call-ID");
	//cCallID

	char cTo[100]={""};
	if((cp=strstr(cMessage,"To: ")))
	{
		if((cp1=strchr(cp+strlen("To: "),'\r')))
		{
			*cp1=0;
			sprintf(cTo,"%.99s",cp+strlen("To: "));
			*cp1='\r';
		}
	}//cTo

	char cToDomain[100]={""};
	unsigned uToPort=0;
	if(cTo[0])
	{
		if((cp=strchr(cTo,'@')))
		{
			sprintf(cToDomain,"%.99s",cp+1);
			if((cp=strchr(cToDomain,':')))
			{
				//get port then chop off
				sscanf(cp+1,"%u",&uToPort);
				*cp=0;
			}
			if((cp=strchr(cToDomain,'>')))
				*cp=0;
			if((cp=strchr(cToDomain,';')))
				*cp=0;
		}
	}//cToDomain

	char cFrom[100]={""};
	if((cp=strstr(cMessage,"From: ")))
	{
		if((cp1=strchr(cp+strlen("From: "),'\r')))
		{
			*cp1=0;
			sprintf(cFrom,"%.99s",cp+strlen("From: "));
			*cp1='\r';
		}
	}//cFrom

	char cFromDomain[100]={""};
	unsigned uFromPort=0;
	if(cFrom[0])
	{
		if((cp=strchr(cFrom,'@')))
		{
			sprintf(cFromDomain,"%.99s",cp+1);
			if((cp=strchr(cFromDomain,':')))
			{
				//get port then chop off
				sscanf(cp+1,"%u",&uFromPort);
				*cp=0;
			}
			if((cp=strchr(cFromDomain,'>')))
				*cp=0;
			if((cp=strchr(cFromDomain,';')))
				*cp=0;
		}
	}//cFromDomain

	if(guLogLevel>3)
	{
		sprintf(gcQuery,"cTo:%s cToDomain:%s:%u",cTo,cToDomain,uToPort);
		logfileLine("readEv",gcQuery);
		sprintf(gcQuery,"cFrom:%s cFromDomain:%s:%u",cFrom,cFromDomain,uFromPort);
		logfileLine("readEv",gcQuery);
	}

	//
	//If anything is amiss send back an error message and return.
	//
	char cMsg[100]={""};
	if(!cCallID[0])
	{
		sprintf(cMsg,"481 Call/Transaction Does Not Exist\n");
		if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
		{
			if(guLogLevel>3)
			{
				sprintf(gcQuery,"reply sent to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv",gcQuery);
			}
		}
		else
		{
			if(guLogLevel>1)
			{
				sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv",gcQuery);
			}
		}
		if(guLogLevel>3)
			logfileLine("readEv 481 Call/Transaction Does Not Exist",cSourceIP);	
	}
	if(!cToDomain[0])
	{
		//Empty cToDomain
		sprintf(cMsg,"SIP/2.0 416 Unsupported URI\n");
		if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
		{
			if(guLogLevel>3)
			{
				sprintf(gcQuery,"reply sent to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv",gcQuery);
			}
		}
		else
		{
			if(guLogLevel>1)
			{
				sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv",gcQuery);
			}
		}
		if(guLogLevel>3)
			logfileLine("readEv cToDomain 416 empty",cSourceIP);
		return;
	}

	//Determine if message is a request or a reply
	unsigned uReply=0;
	if(cMessage[0]=='S') uReply=1;

	char cData[256]={""};
	char cKey[128]={""};
	size_t sizeData=255;
	uint32_t flags=0;
	memcached_return rc;

	//
	//Process requests
	//	(e.g. commands like INVITE, ACK, BYE, CANCEL, OPTIONS, REGISTER and INFO)
	//
	if(!uReply)
	{
		if(cToDomain[0])
		{
			sprintf(cKey,"%s-pbx",cToDomain);
			sprintf(cData,"%.255s",memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc));
			if(rc!=MEMCACHED_SUCCESS)
			{
				//Not found. Try the from domain
				sprintf(cKey,"%s-pbx",cFromDomain);
				sprintf(cData,"%.255s",memcached_get(gsMemc,cKey,strlen(cKey),&sizeData,&flags,&rc));
				if(rc!=MEMCACHED_SUCCESS)
				{
					//Not found
					sprintf(cMsg,"SIP/2.0 404 User not found\n");
					if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
					{
						if(guLogLevel>3)
						{
							sprintf(gcQuery,"reply sent to %s:%u",cSourceIP,uSourcePort);
							logfileLine("readEv",gcQuery);
						}
					}
					else
					{
						if(guLogLevel>1)
						{
							sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
							logfileLine("readEv",gcQuery);
						}
					}
					if(guLogLevel>3)
						logfileLine("readEv cToDomain/cFromDomain 404 not found",cKey);
					return;
				}
				else if(rc==MEMCACHED_SUCCESS)
				{
					//The destination is the cToDomain and the uToPort
					//If cToDomain is not an IP this will not work.
					sprintf(cDestinationIP,"%.15s",cToDomain);
					uDestinationPort=uToPort;
				}
			}
			else if(rc==MEMCACHED_SUCCESS)
			{
				//parse cData for forwarding information
				if((cp=strstr(cData,"cDestinationIP=")))
				{
					if((cp1=strchr(cp+15,';')))
					{
						*cp1=0;
						sprintf(cDestinationIP,"%.15s",cp+15);
						*cp1=';';
					}
				}
				if((cp=strstr(cData,"uDestinationPort=")))
					sscanf(cp+17,"%u",&uDestinationPort);
				if(guLogLevel>3)
				{
					sprintf(gcQuery,"cDestinationIP:%s uDestinationPort:%u",cDestinationIP,uDestinationPort);
					logfileLine("readEv",gcQuery);
				}
				if(!cDestinationIP[0] || !uDestinationPort)
					logfileLine("readEv pbx parse error",cData);
			}
		}
		sprintf(cData,"cSourceIP=%.15s;uSourcePort=%u;",cSourceIP,uSourcePort);
		rc=memcached_set(gsMemc,cCallID,strlen(cCallID),cData,strlen(cData),(time_t)0,(uint32_t)0);
		if(rc!=MEMCACHED_SUCCESS)
		{
			sprintf(cMsg,"SIP/2.0 500 Could not create transaction\n");
			if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
			{
				if(guLogLevel>3)
				{
					sprintf(gcQuery,"reply sent to %s:%u",cSourceIP,uSourcePort);
					logfileLine("readEv",gcQuery);
				}
			}
			else
			{
				if(guLogLevel>1)
				{
					sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
					logfileLine("readEv",gcQuery);
				}
			}
			if(guLogLevel>3)
				logfileLine("readEv 500 Could not create transaction",cKey);
			return;
		}

	}//if a request (e.g. commands like INVITE, ACK, BYE, CANCEL, OPTIONS, REGISTER and INFO)
	//
	//Process reply
	//	(e.g. SIP/2.0 200 Ok)
	//
	else
	{
		sprintf(cData,"%.255s",memcached_get(gsMemc,cCallID,strlen(cCallID),&sizeData,&flags,&rc));
		if(rc!=MEMCACHED_SUCCESS)
		{
			sprintf(cMsg,"SIP/2.0 481 Transaction Does Not Exist\n");
			if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
			{
				if(guLogLevel>3)
				{
					sprintf(gcQuery,"reply sent to %s:%u",cSourceIP,uSourcePort);
					logfileLine("readEv",gcQuery);
				}
			}
			else
			{
				if(guLogLevel>1)
				{
					sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
					logfileLine("readEv",gcQuery);
				}
			}
			if(guLogLevel>3)
				logfileLine("readEv 481 Transaction Does Not Exist",cCallID);
		}
		if(guLogLevel>3 && cData[0])
			logfileLine("readEv reply cData",cData);
		//parse cData for forwarding information
		sscanf(cData,"cSourceIP=%[^;];uSourcePort=%u;",cDestinationIP,&uDestinationPort);
		if(guLogLevel>3)
		{
			sprintf(gcQuery,"cSourceIP:%s uSourcePort:%u",cDestinationIP,uDestinationPort);
			logfileLine("readEv",gcQuery);
		}
	}//if a reply



	//
	//Forward unmodified packet
	if(cDestinationIP[0] && uDestinationPort)
	{
		if(!iSendUDPMessage(cMessage,cDestinationIP,uDestinationPort))
		{
			if(guLogLevel>3)
			{
				sprintf(gcQuery,"message forwarded to %s:%u",cDestinationIP,uDestinationPort);
				logfileLine("readEv",gcQuery);
			}
		}
		else
		{
			if(guLogLevel>1)
			{
				sprintf(gcQuery,"forward failed to %s:%u",cDestinationIP,uDestinationPort);
				logfileLine("readEv",gcQuery);
			}
			sprintf(cMsg,"SIP/2.0 500 Forward failed\n");
			if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
			{
				if(guLogLevel>3)
				{
					sprintf(gcQuery,"reply sent to %s:%u",cSourceIP,uSourcePort);
					logfileLine("readEv",gcQuery);
				}
			}
			else
			{
				if(guLogLevel>1)
				{
					sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
					logfileLine("readEv",gcQuery);
				}
			}
			if(guLogLevel>3)
				logfileLine("readEv 500 Forward failed",cDestinationIP);
			return;
		}
	}//if(cDestinationIP[0] && uDestinationPort)
	else
	{
		sprintf(cMsg,"SIP/2.0 500 Forward failed\n");
		if(!iSendUDPMessage(cMsg,cSourceIP,uSourcePort))
		{
			if(guLogLevel>3)
			{
				sprintf(gcQuery,"reply sent to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv",gcQuery);
			}
		}
		else
		{
			if(guLogLevel>1)
			{
				sprintf(gcQuery,"reply failed to %s:%u",cSourceIP,uSourcePort);
				logfileLine("readEv",gcQuery);
			}
		}
		if(guLogLevel>3)
			logfileLine("readEv 500 Forward failed",cDestinationIP);
		return;
		if(guLogLevel>3)
			logfileLine("readEv unexpected no cDestinationIP/uDestinationPort",cCallID);
	}


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
		perror("unxsSIPProxy pid file exists");
		logfileLine("main","unxsSIPProxy may already be running");
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

		fprintf(gLfp,"%s unxsSIPProxy[%u]::%s %s.\n",cTime,pidThis,cFunction,cLogline);
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
	memcached_return rc;
	char *key= "unxsSIPProxy";
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


