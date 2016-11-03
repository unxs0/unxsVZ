#
# svn ID removed
# uSIPSwitch/makefile
#
# (C) 2012-2015 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Support for non CentOS 5,6 Linux systems is not tested or supported.
#

#We code with all kinds of gcc allowed crazyness. Show our trangressions with pedantic flag
#CFLAGS= -Wall -pedantic
CFLAGS= -Wall
LIBS= -L/usr/lib64/mysql -lmysqlclient -lssl -lm /usr/lib64/libmemcached.so /usr/lib64/libevent.so

help:
	@echo "main make options available"
	@echo "	unxsSIPHealth"
	@echo "	install"
	@echo "	install-run"

unxsSIPHealth: main.o ../mysqlconnect.o
	cc main.o ../mysqlconnect.o -o unxsSIPHealth $(LIBS) 

main.o: main.c ../sipproxy.h ../local.h endpoint.c
	cc -c main.c -o main.o $(CFLAGS)

mysqlconnect.o: ../mysqlconnect.c ../sipproxy.h ../local.h
	cc -c ../mysqlconnect.c -o ../mysqlconnect.o $(CFLAGS)

clean:
	rm -f *.o unxsSIPHealth

install: unxsSIPHealth
	install -s unxsSIPHealth /usr/sbin/unxsSIPHealth
	rm unxsSIPHealth

install-run: unxsSIPHealth
	kill -HUP `cat /var/run/unxsSIPHealth.pid`
	rm -f /var/run/unxsSIPHealth.pid
	install -s unxsSIPHealth /usr/sbin/unxsSIPHealth
	rm unxsSIPHealth
	/usr/sbin/unxsSIPHealth $(uSIPSwitchIP) 6060

