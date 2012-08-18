/*
FILE
	unxsVZ/unxsSIPProxy/libevent-test.c

COMPILE
	gcc -Wall libevent-test.c -o libevent-test /usr/lib64/libevent.so
TEST
	Run and use ctrl-c to stop. In other terminal send traffic, e.g.:

	sipsak -F -r 4950 -s sip:nobody@127.0.0.1

	Try running several instances in the background you will
	be suprised on the messages per seconds this program will handle.
AUTHOR
	Based on public domain libevent-test.c by Brian Smith
	Other parts (C) 2012 Gary Wallis for Unixservice, LLC.
	GPLv2 License applies to applicable sections.
NOTES
	libevent kicks major ass. Our little SIP proxy will
	definetly use non blocking i/o and threads via libevent
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

#include <signal.h>
void sig_handler(int signum)
{
	printf("guCount=%u\n",guCount);
	exit(0);
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

}//void readEv(int fd,short event,void* arg)


int main()
{
	int sock;
	int yes = 1;
	int len = sizeof(struct sockaddr);
	struct sockaddr_in addr;
	if((sock = socket(AF_INET, SOCK_DGRAM, 0))<0)
	{
		perror("socket");
		return 1;
	}
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
	{
		perror("setsockopt");
		return 1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4950);
	addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(addr.sin_zero), 8);
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


