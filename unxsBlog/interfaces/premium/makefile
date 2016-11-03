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

CGINAME= PremiumContent.cgi

CFLAGS= -DLinux -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient /usr/lib/openisp/libtemplate.a /usr/lib/openisp/libucidr.a -lz -lcrypt -lm -lssl

interface.cgi: main.o content.o ../../cgi.o
	cc main.o content.o ../../cgi.o -o interface.cgi $(LIBS)

main.o: main.c interface.h ../../local.h ../../mysqlrad.h
	cc -c main.c -o main.o $(CFLAGS) 

content.o : content.c interface.h ../../local.h ../../mysqlrad.h
	cc -c content.c -o content.o $(CFLAGS)
	
clean:
	rm -f *.o *.cgi
	
install: interface.cgi
	install -s interface.cgi /var/www/unxs/cgi-bin/$(CGINAME)
	rm -f interface.cgi

