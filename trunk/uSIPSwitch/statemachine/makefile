#
# $Id$
# uSIPSwitch/statemachine/makefile
#
# (C) 2013 Gary Wallis for Unixservice, LLC.
#
#

#We code with all kinds of gcc allowed crazyness. Show our trangressions with pedantic flag
#CFLAGS= -Wall -pedantic
CFLAGS= -Wall

all: sm

sm: main.o
	cc main.o -o sm

clean:
	rm -f sm;rm -f *.o;
