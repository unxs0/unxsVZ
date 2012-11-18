#
# $Id$
# tools/freepbx/qos/makefile
#
# (C) 2012 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#	For help contact support @ openisp . net
#	Support for non CentOS 5+ linux systems is not tested or supported yet.
#
#	gcc -Wall test.c -lm -o /root/NodeQOS

CFLAGS= -Wall
LIBS= -lm

all: nodeQOS

nodeQOS: nodeqos.o 
	cc nodeqos.o -o nodeQOS $(LIBS) 

natpbx.o: nodeqos.c 
	cc -c nodeqos.c -o nodeqos.o $(CFLAGS)

clean:
	rm -f *.o nodeQOS

install: nodeQOS
	install -s nodeQOS /usr/sbin/nodeQOS
	rm nodeQOS
