#
# svn ID removed
# agents/nodeqos/makefile
#
# (C) 2012-2013 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#	For help contact support @ openisp . net
#	Support for non CentOS 5+ linux systems is not tested or supported yet.
#

CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lm

all: PbxQOS

PbxQOS: nodeqos.o mysqlconnect.o
	cc nodeqos.o mysqlconnect.o -o PbxQOS $(LIBS) 

nodeqos.o: nodeqos.c ../../mysqlrad.h ../../local.h local.h
	cc -c nodeqos.c -o nodeqos.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c ../../mysqlrad.h ../../local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

clean:
	rm -f *.o PbxQOS

install: PbxQOS
	install -s PbxQOS /usr/sbin/PbxQOS
	rm PbxQOS

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

