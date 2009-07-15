#
#FILE
#	makefile
#	$Id$
#AUTHOR
#	(C) 2006-2009, Gary Wallis for Unixservice USA.
#

CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lz

idns-logerror: main.o
	cc main.o -o unxsvz-autonomics $(LIBS)

main.o: main.c ../../local.h ../../mysqlrad.h
	cc -c main.c -o main.o $(CFLAGS) 

clean:
	rm -f *.o unxsvz-autonomics
	
install: idns-logerror
	install -m 700 -s unxsvz-autonomics /usr/sbin/unxsvz-autonomics
	rm -f idns-logerror
