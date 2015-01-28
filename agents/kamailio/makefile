#
# $Id$
# agents/kamailio/makefile
#
# (C) 2015 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#	For help contact support @ openisp . net
#	Support for non CentOS 5+ linux systems is not tested or supported yet.
#

CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lm

all: unxsSIPS

unxsSIPS: sips.o mysqlconnect.o
	cc sips.o mysqlconnect.o -o unxsSIPS $(LIBS) 

sips.o: sips.c ../../mysqlrad.h ../../local.h local.h
	cc -c sips.c -o sips.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c ../../mysqlrad.h ../../local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o unxsSIPS

install: unxsSIPS
	install -s unxsSIPS /usr/sbin/unxsSIPS
	rm unxsSIPS
