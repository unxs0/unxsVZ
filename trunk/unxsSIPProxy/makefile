#
# $Id$
# unxsSIPProxy/makefile
#
# (C) 2012 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Support for non CentOS 5+ linux systems is not tested or supported yet.
#

CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lm

all: unxsSIPProxy

unxsSIPProxy: main.o mysqlconnect.o
	cc main.o mysqlconnect.o -o unxsSIPProxy $(LIBS) 

main.o: main.c sipproxy.h local.h
	cc -c main.c -o main.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c sipproxy.h local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o unxsSIPProxy

install: unxsSIPProxy
	install -s unxsSIPProxy /usr/sbin/unxsSIPProxy
	rm unxsSIPProxy

install-run: unxsSIPProxy
	killall unxsSIPProxy
	install -s unxsSIPProxy /usr/sbin/unxsSIPProxy
	rm unxsSIPProxy
	/usr/sbin/unxsSIPProxy
