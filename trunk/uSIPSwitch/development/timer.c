//gcc timer.c -o timer -lm /usr/lib64/libevent.so


#include <stdio.h>
#include <sys/time.h>
#include <event.h>

#define uTIMERA_SECS 2

void vTimerA(int fd, short event, void *arg)
{

	//do something
	printf("Timer A triggered\n");

	//do it again later
	struct event *ev = arg;
	struct timeval tvTimerA;
	tvTimerA.tv_sec = uTIMERA_SECS;
	tvTimerA.tv_usec = 0;
	evtimer_add(ev,&tvTimerA);
}


int main(int argc, const char* argv[])
{
	struct event evTimerA;
	struct timeval tvTimerA;

	tvTimerA.tv_sec = uTIMERA_SECS;
	tvTimerA.tv_usec = 0;

	event_init();

	evtimer_set(&evTimerA,vTimerA,&evTimerA);
	evtimer_add(&evTimerA,&tvTimerA);
	event_dispatch();

	return 0;
}
