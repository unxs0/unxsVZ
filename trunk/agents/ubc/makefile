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
LIBS= -L/usr/lib64/mysql -L/usr/lib/mysql -lmysqlclient -lssl -lm

all: unxsUBC

unxsUBC: ubc.o mysqlconnect.o
	cc ubc.o mysqlconnect.o -o unxsUBC $(LIBS) 

ubc.o: ubc.c ../../mysqlrad.h ../../local.h
	cc -c ubc.c -o ubc.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c ../../mysqlrad.h ../../local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

clean:
	rm -f *.o unxsUBC

install: unxsUBC
	install -s unxsUBC /usr/sbin/unxsUBC
	rm unxsUBC
