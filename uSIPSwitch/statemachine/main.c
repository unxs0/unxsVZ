/*
FILE
	$Id$
	uSIPSwitch/statemachine/main.c
AUTHOR
	(C) 2013 Gary Wallis for Unixservice, LLC.
NOTES
	Learn about lookup table based state machine programming
*/

#include <stdlib.h>
#include <stdio.h>

enum states { STATE_0, STATE_1, STATE_2, MAX_STATES };
enum events {  EVENT_0, EVENT_1, EVENT_2, MAX_EVENTS };

void action_s0_e0(void);
void action_s0_e1(void);
void action_s0_e2(void);
void action_s1_e0(void);
void action_s1_e1(void);
void action_s1_e2(void);
void action_s2_e0(void);
void action_s2_e1(void);
void action_s2_e2(void);
enum events get_new_event(void);

//lookup table data structure of action functions
void (*const state_table [MAX_STATES][MAX_EVENTS])(void) = {

    { action_s0_e0, action_s0_e1, action_s0_e2 },
    { action_s1_e0, action_s1_e1, action_s1_e2 },
    { action_s2_e0, action_s2_e1, action_s2_e2 }
};

static enum states current_state=STATE_0;
static enum events new_event=EVENT_0;

int main(void)
{
	while(current_state<STATE_2)
	{
		//printf("pre get_new_event() %u\n",new_event);
		new_event=get_new_event();
		//printf("post get_new_event() %u\n",new_event);

		//protect state_table boundaries
		if (((new_event >= 0) && (new_event < MAX_EVENTS))
    			&& ((current_state >= 0) && (current_state < MAX_STATES)))
		{
			state_table [current_state][new_event] ();
		}
		else
		{
			printf("invalid event/state %u/%u\n",new_event,current_state);
			exit(0);
		}
    	}
	printf("state2 normal exit %u %u\n",current_state,new_event);
	exit(0);
}//main()


void action_s0_e0(void)
{
	printf("action_s0_e0() %u %u\n",current_state,new_event);
	current_state = STATE_1;
}

void action_s0_e1(void)
{
	printf("action_s0_e1() %u %u\n",current_state,new_event);
	current_state = STATE_1;
}


void action_s0_e2(void)
{
	printf("action_s0_e2() %u %u\n",current_state,new_event);
	current_state = STATE_1;
}


void action_s1_e0(void)
{
	printf("action_s1_e0() %u %u\n",current_state,new_event);
	current_state = STATE_2;
}


void action_s1_e1(void)
{
	printf("action_s1_e1() %u %u\n",current_state,new_event);
	current_state = STATE_2;
}


void action_s1_e2(void)
{
	printf("action_s1_e2() %u %u\n",current_state,new_event);
	current_state = STATE_2;
}


void action_s2_e0(void)
{
	printf("action_s2_e0() %u %u\n",current_state,new_event);
	current_state = STATE_1;
}


void action_s2_e1(void)
{
	printf("action_s2_e1() %u %u\n",current_state,new_event);
	current_state = STATE_1;
}


void action_s2_e2(void)
{
	printf("action_s2_e2() %u %u\n",current_state,new_event);
	current_state = STATE_2;
}


enum events get_new_event (void)
{
    return(++new_event);
}
