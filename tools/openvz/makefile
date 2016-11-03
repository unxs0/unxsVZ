#
# svn ID removed
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

all: copyCloneQuota

copyCloneQuota: copyCloneQuota.o mysqlconnect.o
	cc copyCloneQuota.o mysqlconnect.o -o copyCloneQuota $(LIBS) 

copyCloneQuota.o: copyCloneQuota.c ../../mysqlrad.h ../../local.h
	cc -c copyCloneQuota.c -o copyCloneQuota.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c ../../mysqlrad.h ../../local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

clean:
	rm -f *.o copyCloneQuota

install: copyCloneQuota
	install -s copyCloneQuota /usr/sbin/copyCloneQuota
	rm copyCloneQuota
