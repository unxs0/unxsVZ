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

	We need to keep track of SIP "conversations" that we call dialogues.

	We use the SIP protocol to maintain dailogue entries.

INITIAL ARCHITECTURE

	Create N listen children on startup.
	
	Build DNS based gateway, server IP, port, preference table. A signal can rebuild this
	table during run time. The table can also have non DNS based static IP and port and preference
	tuples if no noted in the source gateway tables by using IPv4 instead of hostnames.
	
	Every valid dialogue get's it's own child process.

	The table is a libmemcache construct.
	
AUTHOR/LEGAL
	(C) 2012 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
	Includes public domain beej.us networking source code.
OTHER
	Only tested on CentOS 5.
FREE HELP
	support @ openisp . net
	supportgrp @ unixservice . com
	Join mailing list: https://lists.openisp.net/mailman/listinfo/unxsvz
*/

#include "sipproxy.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <libmemcached/memcached.h>

MYSQL gMysql;
static FILE *gLfp=NULL;
char gcQuery[1024];

//toc
int main(void);
void logfileLine(const char *cFunction,const char *cLogline);
void daemonize(void);
void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
in_port_t get_in_port(struct sockaddr *sa);

#define cudefMYPORT "4950"	// the port users will be connecting to
#define udefMAXBUFLEN 100


typedef struct {
	char cGateway[100];
	char cIPv4[16];
	unsigned uPort;
	unsigned uPrio;
} structTypeGateway;


typedef struct {
	structTypeGateway *structInitiatorGateway;
	structTypeGateway *structDestinationGateway;
	unsigned uCounter;//decide when to close if not via sip protocol
	unsigned uStatus;//current status of call
} structTypeDialogue;


int main(void)
{
	int iSockFD;
	struct addrinfo addrinfoHints, *addrinfoServer, *addrinfoPtr;
	int iRv;
	int iNumbytes;

	//This if for unxs default logging function.
	if((gLfp=fopen(cLOGFILE,"a"))==NULL)
	{
		logfileLine("main","fopen logfile failed");
		exit(1);
	}


	//
	//Setup our lookup table section

	//Setup some sample test destination GWs
	structTypeGateway structGateway[16];
	sprintf(structGateway[0].cGateway,"localhost");
	sprintf(structGateway[0].cIPv4,"127.0.0.1");
	structGateway[0].uPort=4590;
	structGateway[0].uPrio=0;//Highest no other entries

	//We also need some storage for the incoming gateway info
	structTypeGateway structInitiatorGateway[16];

	//End of setup our lookup table section
	//


	//
	//Server socket setup section

	//Setup hints for our needs
	//In this case bind to all system IPs but only udp IPv4
	memset(&addrinfoHints, 0, sizeof addrinfoHints);
	addrinfoHints.ai_family = AF_INET; // set to AF_INET to force IPv4
	addrinfoHints.ai_socktype = SOCK_DGRAM;
	addrinfoHints.ai_flags = AI_PASSIVE; // use my IP

	//We can bind to many here depending on our hints and other things
	//Populate a list of addrinfo's
	if ((iRv=getaddrinfo(NULL,cudefMYPORT,&addrinfoHints,&addrinfoServer))!=0)
	{
		sprintf(gcQuery,"getaddrinfo: %.99s",gai_strerror(iRv));
		logfileLine("main",gcQuery);
		return(1);
	}

	//Loop through all the results and bind to the first we can
	for(addrinfoPtr=addrinfoServer;addrinfoPtr!=NULL;addrinfoPtr=addrinfoPtr->ai_next)
	{
		if((iSockFD=socket(addrinfoPtr->ai_family,addrinfoPtr->ai_socktype,addrinfoPtr->ai_protocol))== -1)
		{
			logfileLine("main","socket error");
			continue;
		}

		if(bind(iSockFD,addrinfoPtr->ai_addr,addrinfoPtr->ai_addrlen)== -1)
		{
			close(iSockFD);
			logfileLine("main","bind error");
			continue;
		}
		break;
	}//For each addrinfo member

	//Could not bind to anything bail!
	if(addrinfoPtr==NULL)
	{
		logfileLine("main","failed to bind socket");
		return(2);
	}

	//We do not require this list anymore so we free it
	freeaddrinfo(addrinfoServer);

	//End of server socket setup section
	//


	//
	//We have enough to start basic daemon so let's do it
	daemonize();
	//


	//
	//This section is basically the waiting for recvfrom loop with
	//child creation for handling incoming and outgoing sip messages.

	//handle wait via signals to avoid wait() blocking.
	struct sigaction sa;
	sa.sa_handler=sigchld_handler; //reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=SA_RESTART;
	if(sigaction(SIGCHLD,&sa,NULL)== -1)
	{
		logfileLine("main","sigaction error exit");
		exit(1);
	}
	logfileLine("main","started");

	//main recvfrom loop
	struct sockaddr_storage sockaddrstorageOtherSide;
	char cBuf[udefMAXBUFLEN];
	socklen_t socklentypeAddrlen;
	char cIPStr[INET_ADDRSTRLEN];
	while(1)
	{
		socklentypeAddrlen=sizeof sockaddrstorageOtherSide;
		if ((iNumbytes=recvfrom(iSockFD,cBuf,udefMAXBUFLEN-1,0,(struct sockaddr *)&sockaddrstorageOtherSide,&socklentypeAddrlen))== -1)
		{
			logfileLine("main","recvfrom error");
			return(3);
		}
		else
		{
			//These are key architecture concerns that will make the app robust and quick.
			//Fork on every message recieved that comes in OR
			//	only fork after determingin if traffic is from a valid source and destination and has valid sip traffic
			//Probably an initial pool of forked listeners that will have i/o handled via libevent or similar
			//is the best solution.
			//For now we will keep it as simple as possible to learn how to handle sip traffic and integrate with
			//memcached DNS and DNS SRV based building of our routing table.
			if(!fork())
			{
				register int i;
				char cGateway[100]={""};
				char cIPv4[16]={""};
				unsigned uPort=0;

				sprintf(gcQuery,"%s:%d",
					inet_ntop(sockaddrstorageOtherSide.ss_family,get_in_addr((struct sockaddr *)&sockaddrstorageOtherSide),
								cIPStr,sizeof cIPStr),
					ntohs(get_in_port((struct sockaddr *)&sockaddrstorageOtherSide)));
				logfileLine("main",gcQuery);
				cBuf[iNumbytes]=0;
				//printf("listener: packet contains \"%s\"\n", cBuf);
				logfileLine("main",cBuf);


				//Single test case sip packet INVITE
				//	We will need to pass other packets also

				//Parse the gateway from the INVITE data
				//INVITE sip:13103566265@localhost SIP/2.0
				if(sscanf(cBuf,"INVITE sip:%*[^@\n]@%[^ ]",cGateway)==1)
				{
					logfileLine("main destination gw",cGateway);

					//find the gateway in our lookup table
					//temp test bigO n lookup no prio
					for(i=0;i<16;i++)
					{
						if(!strcmp(cGateway,structGateway[i].cGateway))
						{
							sprintf(cIPv4,"%.15s",structGateway[i].cIPv4);
							uPort=structGateway[i].uPort;
							break;
						}
					}

					if(!uPort)
					{
						//Return SIP error to dialogue initiator
						//Send a one time sip message then close
						//	and exit this child.
						logfileLine("main","gw not found");
					}
					else
					{
						//Check for existing dialogue entry
						//	else create new dialogue.
	
						//Forward UDP packet as is to dialgue destination gateway
						//Send a message but keep the socket available for future
						// messages.
						sprintf(gcQuery,"%s:%u",cIPv4,uPort);
						logfileLine("main destination ip:port",gcQuery);
						//Wait around for SIP protocol answers and return them to initiator
						//If no answers return SIP error to initiator and then close sockets
						// and exit this child.
						//Exit after timeout counter expires close sockets and exit this child.
					}
				}//if cGateway

				//this is the child process and it is no longer required
				//not sure about this at all review your forking skills
				if(gLfp!=NULL)
					fclose(gLfp);
				close(iSockFD);
				exit(0);
			}
		}

	}//while(1) end of main recfrom outer loop

	close(iSockFD);

	return(0);

}//int main()


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
	switch(fork())
	{
		default:
			_exit(0);

		case -1:
			fprintf(stderr,"fork failed\n");
			_exit(1);

		case 0:
		break;
	}

	if(setsid()<0)
	{
		fprintf(stderr,"setsid failed\n");
		_exit(1);
	}

}//void daemonize(void)


//Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family==AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}//void *get_in_addr(struct sockaddr *sa)


//Get port, IPv4 or IPv6:
in_port_t get_in_port(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET)
		return (((struct sockaddr_in*)sa)->sin_port);
	return (((struct sockaddr_in6*)sa)->sin6_port);
}//in_port_t get_in_port(struct sockaddr *sa)


//Very important snippet of code
void sigchld_handler(int s)
{
	while(waitpid(-1,NULL,WNOHANG)>0);
	//logfileLine("sigchld_handler","ok");
}//void sigchld_handler(int s)
