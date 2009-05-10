#
#FILE
#	makefile
#	$Id: makefile 609 2007-06-05 15:20:50Z ggw $
#AUTHOR
#	(C) 2006-2007, Gary Wallis for Unixservice
#PURPOSE
#	Makefile for special commandline only interface for named (DNS/BIND 9.3.3) query
#	logging
#

CFLAGS= -DLinux -Wall
LIBS= /usr/lib/mysql/libmysqlclient.a -lssl -lz

idns-logquery: main.o
	cc main.o -o idns-logquery $(LIBS)

main.o: main.c ../../local.h ../../mysqlrad.h
	cc -c main.c -o main.o $(CFLAGS) 

idns-aggregator: aggregator.o
	cc aggregator.o -o idns-aggregator $(LIBS)

aggregator.o: aggregator.c ../../local.h ../../mysqlrad.h
	cc -c aggregator.c -o aggregator.o $(CFLAGS) 

clean:
	rm -f *.o idns-logquery
	
install: idns-logquery
	install -s idns-logquery /usr/sbin/idns-logquery
	rm -f idns-logquery

install-collector: collector.sh
	cp -i collector.sh /usr/sbin/
	chmod 500 /usr/sbin/collector.sh

install-aggregator.sh: aggregator.sh
	cp -i aggregator.sh /usr/sbin/
	chmod 500 /usr/sbin/aggregator.sh

install-aggregator: idns-aggregator
	install -s idns-aggregator /usr/sbin/idns-aggregator
	rm -f idns-aggregator
