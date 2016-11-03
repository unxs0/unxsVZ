#
# svn ID removed
# agents/snort/makefile
#
# (C) 2001-2014 Gary Wallis.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#	For help contact support @ openisp . net
#	Support for non CentOS 5+ linux systems is not tested or supported yet.
#

CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lm

all: unxsSnort

unxsSnort: snort.o mysqlconnect.o
	cc snort.o mysqlconnect.o -o unxsSnort $(LIBS) 

snort.o: snort.c snort.h local.h
	cc -c snort.c -o snort.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c snort.h local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

clean:
	rm -f *.o unxsSnort

install: unxsSnort
	install -s unxsSnort /usr/sbin/unxsSnort
	rm unxsSnort

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

