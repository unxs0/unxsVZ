#
# $Id$
# agents/ubc/makefile
#
# (C) 2001-2009 Gary Wallis.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#	For help contact support @ openisp . net
#	Support for non linux systems is not tested or supported yet.
#

#Change these for your system: Ex. -DSolaris instead of -DLinux
#CFLAGS=-O -DLinux -Wall
CFLAGS= -DLinux -Wall
RELEASE= 0.1

ifdef FreeBSD
	LIBS= /usr/local/lib/mysql/libmysqlclient.a -lz -lm /usr/lib/libcrypt.a
else
	#LIBS= /usr/lib/mysql/libmysqlclient.a -lz -lcrypt -lm -lssl
	LIBS= /usr/lib/mysql/libmysqlclient.a -lssl -lm
endif


all: unxsUBC

unxsUBC: ubc.o 
	cc ubc.o -o unxsUBC $(LIBS) 

ubc.o: ubc.c ../../mysqlrad.h
	cc -c ubc.c -o ubc.o $(CFLAGS)

clean:
	rm -f *.o unxsUBC

install: unxsUBC
	install -s unxsUBC /usr/sbin/unxsUBC
	rm unxsUBC
