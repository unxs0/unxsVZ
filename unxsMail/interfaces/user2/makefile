#
#FILE
#	makefile
#	svn ID removed
#AUTHOR
#	(C) 2006-2010 Gary Wallis and Hugo Urquiza for Unixservice, LLC.
#PURPOSE
#	unxsVZ CentOS 5+ rpm/yum style only.
#

CGINAME= mailUser.cgi
CGIDIR= /var/www/unxs/cgi-bin/
CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -L/usr/lib/openisp -lmysqlclient -lz -lcrypt -lm -lssl -ltemplate

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

