#
# svn ID removed
# uSIPSwitch/makefile
#
# (C) 2012 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Support for non CentOS 5+ linux systems is not tested or supported yet.
#

#We code with all kinds of gcc allowed crazyness. Show our trangressions with pedantic flag
#CFLAGS= -Wall -pedantic
CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -L/usr/lib/openisp -lmysqlclient -lssl -lm -ldns

all: unxsContainerResolver

unxsContainerResolver: container-resolv.o mysqlconnect.o container-resolv.h
	cc container-resolv.o mysqlconnect.o -o unxsContainerResolver $(LIBS) 

mysqlconnect.o: mysqlconnect.c local.h 
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o unxsContainerResolver

install-unxsContainerResolver: unxsContainerResolver
	install -s unxsContainerResolver /usr/sbin/unxsContainerResolver
	rm unxsContainerResolver
