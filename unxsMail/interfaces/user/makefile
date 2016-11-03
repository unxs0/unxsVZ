#
#FILE
#	makefile
#	svn ID removed
#AUTHOR
#	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice
#PURPOSE
#	General purpose RAD3 interface makefile
#	Must customize CGINAME 
#

CGINAME= unxsMailUser.cgi

CFLAGS= -DLinux -Wall
LIBS= /usr/lib/mysql/libmysqlclient.a /usr/lib/openisp/libtemplate.a -lz -lcrypt -lm -lssl

interface.cgi: main.o myaccount.o glossary.o spam.o ../../cgi.o
	cc main.o myaccount.o glossary.o spam.o ../../cgi.o -o interface.cgi $(LIBS)

main.o: main.c interface.h ../../local.h ../../mysqlrad.h
	cc -c main.c -o main.o $(CFLAGS) 

myaccount.o: myaccount.c interface.h ../../local.h ../../mysqlrad.h
	cc -c myaccount.c -o myaccount.o $(CFLAGS) 

spam.o: spam.c interface.h ../../local.h ../../mysqlrad.h
	cc -c spam.c -o spam.o $(CFLAGS)

glossary.o : glossary.c interface.h ../../local.h ../../mysqlrad.h
	cc -c glossary.c -o glossary.o $(CFLAGS)

clean:
	rm -f *.o *.cgi
	
install: interface.cgi
	install -s interface.cgi /var/www/unxs/cgi-bin/$(CGINAME)
	rm -f interface.cgi

