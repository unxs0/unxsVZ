/*
FILE
	unxsVZ/unxsSIPProxy/libevent-test.c

COMPILE
	gcc -Wall libevent-test.c -o libevent-test /usr/lib64/libevent.so
TEST
	Run and use ctrl-c to stop. In other terminal send traffic, e.g.:

	sipsak -F -r 4950 -s sip:nobody@127.0.0.1

	Try running several instances in the background you will
	be suprised about how many messages per seconds this program will handle.
AUTHOR
	Based on public domain libevent-test.c by Brian Smith
	Other parts (C) 2012 Gary Wallis for Unixservice, LLC.
	GPLv2 License applies to applicable sections.
NOTES
	libevent kicks major ass. Our little SIP proxy will
	definetly use non blocking i/o via libevent
	that coupled with memcached should make for a tiny and
	very high speed as well as reliable little daemon.
*/
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <event.h>

unsigned guCount=0;
char gcServerIP[16]={"127.0.0.1"};
char gcClientIP[16]={""};

#include <signal.h>
void sig_handler(int signum)
{
	printf("guCount=%u\n",guCount);
	exit(0);
}

int
check_for_old_version(void)
{
    const char *v = event_get_version();
    /* This is a dumb way to do it, but it is the only thing that works
       before Libevent 2.0. */
    if (!strncmp(v, "0.", 2) ||
        !strncmp(v, "1.1", 3) ||
        !strncmp(v, "1.2", 3) ||
        !strncmp(v, "1.3", 3)) {

        printf("Your version of Libevent is very old.  If you run into bugs,"
               " consider upgrading.\n");
        return -1;
    } else {
        printf("Running with Libevent version %s\n", v);
        return 0;
    }
}	 

void readEv(int fd,short event,void* arg)
{
	ssize_t len;
	char buf[256]={""};
	//struct event *ev = arg;

	socklen_t l=sizeof(struct sockaddr);
	struct sockaddr_in cAddr;

	//printf("readEv called with %s fd: %d, event: %d\n",event_get_method(),fd,event);

	len=recvfrom(fd,(void *)buf,255,0,(struct sockaddr*)&cAddr,&l);

	if(len== -1)
	{
		perror("recvfrom()");
		return;
	}
	else if(len==0)
	{
		printf("Connection Closed\n");
		return;
	}

	//buf[len]='\0';
	//printf("READ: %s\n",buf);
	//printf(".");
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
	char cMsg[100]={"SIP/2.0 100 OK\n"};
	char cIPStr[INET_ADDRSTRLEN];
	unsigned uPort=5060;//defaul
	//Get other side IP number and create quickest socket
	inet_ntop(AF_INET,&cAddr.sin_addr,cIPStr,sizeof(cIPStr));
	inet_aton(cIPStr,&remoteServAddr.sin_addr);
	remoteServAddr.sin_family=AF_INET;
	remoteServAddr.sin_port=htons(uPort);
	sd=socket(AF_INET,SOCK_DGRAM,0);
	if(sd<0)
	{
		perror("socket()");
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
			return;
		}
	}
	//Send reponse message
	rc=sendto(sd,cMsg,strlen(cMsg),0,(struct sockaddr *)&remoteServAddr,sizeof(remoteServAddr));
	if(rc<0)
	{
		perror("sendto()");
		return;
	}
	close(sd);

	//
	//Forward


}//void readEv(int fd,short event,void* arg)


int main()
{
	check_for_old_version();

	int sock;
	int yes=1;
	int len=sizeof(struct sockaddr);
	struct sockaddr_in addr;
	if((sock=socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("socket");
		return 1;
	}
	if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) < 0)
	{
		perror("setsockopt");
		return 1;
	}
	addr.sin_family=AF_INET;
	addr.sin_port=htons(4950);
	if(gcServerIP[0])
		inet_aton(gcServerIP,&addr.sin_addr);
	else
		addr.sin_addr.s_addr=INADDR_ANY;
	bzero(&(addr.sin_zero),8);
	if(bind(sock,(struct sockaddr*)&addr, len)<0)
	{
		perror("bind");
		return 1;
	}
	signal(SIGINT,sig_handler);
	printf("Listening now...\n");

	struct event ev;
	event_init();
	event_set(&ev, sock, EV_READ | EV_PERSIST, readEv, &ev);
	event_add(&ev, NULL);
	event_dispatch();

	printf("Done!\n");
	return 1;

}//main()


