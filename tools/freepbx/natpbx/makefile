#
# svn ID removed
# tools/freepbx/natpbx/makefile
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

all: unxsNATPBX

unxsNATPBX: natpbx.o mysqlconnect.o
	cc natpbx.o mysqlconnect.o -o unxsNATPBX $(LIBS) 

natpbx.o: natpbx.c ../../../mysqlrad.h ../../../local.h
	cc -c natpbx.c -o natpbx.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c ../../../mysqlrad.h ../../../local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

clean:
	rm -f *.o unxsNATPBX

install: unxsNATPBX
	install -s unxsNATPBX /usr/sbin/unxsNATPBX
	rm unxsNATPBX
