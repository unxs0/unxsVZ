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

CGINAME= ispHelp.cgi

CFLAGS= -DLinux -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient /usr/lib/openisp/libtemplate-branch.a /usr/lib/openisp/libucidr.a -lz -lcrypt -lm -lssl

interface.cgi: main.o glossary.o ticket.o  register.o ../../cgi.o
	cc main.o glossary.o ticket.o register.o ../../cgi.o -o interface.cgi $(LIBS)

main.o: main.c interface.h ../../local.h ../../mysqlrad.h
	cc -c main.c -o main.o $(CFLAGS) 

glossary.o : glossary.c interface.h ../../local.h ../../mysqlrad.h
	cc -c glossary.c -o glossary.o $(CFLAGS)

ticket.o: ticket.c interface.h ../../local.h ../../mysqlrad.h
	cc -c ticket.c -o ticket.o $(CFLAGS)

register.o: register.c interface.h ../../local.h ../../mysqlrad.h
	cc -c register.c -o register.o $(CFLAGS)

clean:
	rm -f *.o *.cgi
	
install: interface.cgi
	install -s interface.cgi /var/www/unxs/cgi-bin/$(CGINAME)
	rm -f interface.cgi

