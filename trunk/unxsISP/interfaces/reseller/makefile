#
#FILE
#	makefile
#	$Id: makefile 254 2008-03-06 17:23:29Z Hugo $
#AUTHOR
#	(C) 2006, Gary Wallis for Unixservice
#PURPOSE
#	General purpose RAD3 interface makefile
#	Must customize CGINAME 
#

CGINAME= ispReseller.cgi

CFLAGS= -DLinux -Wall
LIBS= /usr/lib/mysql/libmysqlclient.a /usr/lib/openisp/libtemplate.a /usr/lib/openisp/libucidr.a -lz -lcrypt -lm -lssl -luradius /usr/lib/openisp/libservermonitor.a

interface.cgi: main.o customer.o glossary.o instance.o ../../cgi.o
	cc main.o customer.o glossary.o instance.o ../../cgi.o -o interface.cgi $(LIBS)

main.o: main.c interface.h ../../local.h ../../mysqlrad.h
	cc -c main.c -o main.o $(CFLAGS) 

customer.o: customer.c interface.h ../../local.h ../../mysqlrad.h
	cc -c customer.c -o customer.o $(CFLAGS)

glossary.o : glossary.c interface.h ../../local.h ../../mysqlrad.h
	cc -c glossary.c -o glossary.o $(CFLAGS)

instance.o : instance.c interface.h ../../local.h ../../mysqlrad.h
	cc -c instance.c -o instance.o $(CFLAGS)
	
clean:
	rm -f *.o *.cgi
	
install: interface.cgi
	install -s interface.cgi $(CGIDIR)$(CGINAME)
	rm -f interface.cgi

