#
# $Id$
# agents/openvz/makefile
#
# (C) 2001-2014 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#	For help contact support @ openisp . net
#	Support for non CentOS 5+ linux systems is not tested.
#

CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lm

all: unxsOVZ

unxsOVZ: openvz.o mysqlconnect.o
	cc openvz.o mysqlconnect.o -o unxsOVZ $(LIBS) 

openvz.o: openvz.c openvz.h local.h
	cc -c openvz.c -o openvz.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c openvz.h local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

clean:
	rm -f *.o unxsOVZ

install: unxsOVZ
	install -s unxsOVZ /usr/sbin/unxsOVZ
	rm unxsOVZ

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

