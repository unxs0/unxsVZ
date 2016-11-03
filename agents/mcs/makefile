#
# svn ID removed
# agents/mcs/makefile
#
# (C) 2011 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#	For help contact support @ openisp . net
#	Support for non CentOS 5+ linux systems is not tested or supported yet.
#

CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lm

all: unxsMCS

unxsMCS: mcs.o mysqlconnect.o
	cc mcs.o mysqlconnect.o -o unxsMCS $(LIBS) 

mcs.o: mcs.c ../../mysqlrad.h ../../local.h local.h
	cc -c mcs.c -o mcs.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c ../../mysqlrad.h ../../local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o unxsMCS

install: unxsMCS
	install -s unxsMCS /usr/sbin/unxsMCS
	rm unxsMCS
