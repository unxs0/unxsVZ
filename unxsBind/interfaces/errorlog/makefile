#
#FILE
#	makefile
#	$Id: makefile 660 2007-06-28 22:05:36Z Gary $
#AUTHOR
#	(C) 2006-2009, Gary Wallis for Unixservice
#PURPOSE
#	Makefile for special commandline only interface for named (DNS/BIND 9.3.3) error
#	logging
#

CFLAGS= -DLinux -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lz

idns-logerror: main.o
	cc main.o -o idns-logerror $(LIBS)

main.o: main.c ../../local.h ../../mysqlrad.h
	cc -c main.c -o main.o $(CFLAGS) 

clean:
	rm -f *.o idns-logerror
	
install: idns-logerror
	install -m 700 -s idns-logerror /usr/sbin/idns-logerror
	rm -f idns-logerror
