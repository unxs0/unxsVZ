#
#FILE
#	makefile
#	svn ID removed
#AUTHOR
#	(C) 2006, Gary Wallis for Unixservice
#PURPOSE
#	General purpose RAD3 interface makefile
#	Must customize CGINAME 
#

CGINAME= iRadius.cgi

CFLAGS= -DLinux -Wall
LIBS= /usr/lib64/mysql/libmysqlclient.a /usr/lib/openisp/libtemplate.a /usr/lib/openisp/libucidr.a -lz -lcrypt -lm -lssl

interface.cgi: main.o myaccount.o glossary.o ../../cgi.o
	cc main.o myaccount.o glossary.o ../../cgi.o -o interface.cgi $(LIBS)

main.o: main.c interface.h ../../local.h ../../mysqlrad.h
	cc -c main.c -o main.o $(CFLAGS) 

myaccount.o: myaccount.c interface.h ../../local.h ../../mysqlrad.h
	cc -c myaccount.c -o myaccount.o $(CFLAGS) 

glossary.o : glossary.c interface.h ../../local.h ../../mysqlrad.h
	cc -c glossary.c -o glossary.o $(CFLAGS)

clean:
	rm -f *.o *.cgi
	
install: interface.cgi
	install -s interface.cgi $(CGIDIR)$(CGINAME)
	rm -f interface.cgi

