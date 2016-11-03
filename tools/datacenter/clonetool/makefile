#
# svn ID removed
# tools/datacenter/clonetool/makefile
#
# (C) 2012 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#	For help contact support @ openisp . net
#	Support for non CentOS 5+ linux systems is not tested or supported yet.
#

CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lm

all: unxsCloneTool

unxsCloneTool: clonetool.o mysqlconnect.o
	cc clonetool.o mysqlconnect.o -o unxsCloneTool $(LIBS) 

clonetool.o: clonetool.c ../../../mysqlrad.h ../../../local.h
	cc -c clonetool.c -o clonetool.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c ../../../mysqlrad.h ../../../local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

clean:
	rm -f *.o unxsCloneTool

install: unxsCloneTool
	install -s unxsCloneTool /usr/sbin/unxsCloneTool
	rm unxsCloneTool
