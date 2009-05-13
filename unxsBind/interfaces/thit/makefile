#
#FILE
#	makefile
#	$Id: makefile 662 2008-12-18 18:52:33Z hus $
#AUTHOR
#	(C) 2006-2009, Gary Wallis for Unixservice
#PURPOSE
#	Makefile for special commandline only interface for named (DNS/BIND 9.3.3+) 
#	per zone named.stats logging
#

CFLAGS= -DLinux -Wall
LIBS= /usr/lib/mysql/libmysqlclient.a -lssl -lz -lm

tHitCollector: main.o
	cc main.o -o tHitCollector $(LIBS)

main.o: main.c ../../local.h ../../mysqlrad.h
	cc -c main.c -o main.o $(CFLAGS) 

clean:
	rm -f *.o tHitCollector
	
install: tHitCollector
	install -s tHitCollector /usr/sbin/tHitCollector
	chmod 700 /usr/sbin/tHitCollector
	rm -f tHitCollector
