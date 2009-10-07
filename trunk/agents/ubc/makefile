#
# $Id$
# agents/ubc/makefile
#
# (C) 2001-2009 Gary Wallis.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#	For help contact support @ openisp . net
#	Support for non CentOS 5+ linux systems is not tested or supported yet.
#

CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lm

all: unxsUBC

unxsUBC: ubc.o 
	cc ubc.o -o unxsUBC $(LIBS) 

ubc.o: ubc.c ../../mysqlrad.h ../../local.h
	cc -c ubc.c -o ubc.o $(CFLAGS)

clean:
	rm -f *.o unxsUBC

install: unxsUBC
	install -s unxsUBC /usr/sbin/unxsUBC
	rm unxsUBC
