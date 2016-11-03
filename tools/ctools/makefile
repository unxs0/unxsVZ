#
# svn ID removed
# tools/ctools/makefile
#
# (C) 2011-2012 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#	For help contact support @ unixservice . com
#	Support for non CentOS 5+ linux systems is not provided yet.
#

CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lm

all: unxsvz-checkconf

unxsvz-checkconf: unxsvz-checkconf.o mysqlconnect.o
	cc unxsvz-checkconf.o mysqlconnect.o -o unxsvz-checkconf $(LIBS) 

unxsvz-checkconf.o: unxsvz-checkconf.c ../../mysqlrad.h ../../local.h
	cc -c unxsvz-checkconf.c -o unxsvz-checkconf.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c ../../mysqlrad.h ../../local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

clean:
	rm -f *.o unxsvz-checkconf

install: unxsvz-checkconf
	install -s unxsvz-checkconf /usr/sbin/unxsvz-checkconf
	rm unxsvz-checkconf
