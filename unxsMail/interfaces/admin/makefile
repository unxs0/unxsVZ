#
#FILE
#	makefile
#	svn ID removed
#AUTHOR
#	(C) 2006-2009, Gary Wallis and Hugo Urquiza for Unixservice
#PURPOSE
#	General purpose RAD3 interface makefile
#	Must customize CGINAME 
#

CGINAME= mail2Admin.cgi

CFLAGS= -DLinux -Wall
LIBS= /usr/lib/mysql/libmysqlclient.a /usr/lib/openisp/libtemplate.a -lz -lcrypt -lm -lssl

interface.cgi: main.o user.o domain.o vut.o vutentries.o alias.o glossary.o dashboard.o \
		../../cgi.o /usr/lib/openisp/libtemplate.a \
		/usr/include/openisp/template.h 
	cc main.o user.o domain.o vut.o vutentries.o alias.o glossary.o dashboard.o ../../cgi.o -o interface.cgi $(LIBS)

main.o: main.c interface.h ../../local.h ../../mysqlrad.h /usr/include/openisp/template.h 
	cc -c main.c -o main.o $(CFLAGS) 

user.o: user.c interface.h ../../local.h ../../mysqlrad.h
	cc -c user.c -o user.o $(CFLAGS)

domain.o: domain.c interface.h ../../local.h ../../mysqlrad.h
	cc -c domain.c -o domain.o $(CFLAGS)

vut.o : vut.c interface.h ../../local.h ../../mysqlrad.h
	cc -c vut.c -o vut.o $(CFLAGS)

vutentries.o: vutentries.c interface.h ../../local.h ../../mysqlrad.h
	cc -c vutentries.c -o vutentries.o $(CFLAGS)
	
alias.o : alias.c interface.h ../../local.h ../../mysqlrad.h
	cc -c alias.c -o alias.o $(CFLAGS)

glossary.o : glossary.c interface.h ../../local.h ../../mysqlrad.h
	cc -c glossary.c -o glossary.o $(CFLAGS)

dashboard.o : dashboard.c interface.h ../../local.h ../../mysqlrad.h
	cc -c dashboard.c -o dashboard.o $(CFLAGS)

clean:
	rm -f *.o *.cgi
	
install: interface.cgi
	install -s interface.cgi $(CGIDIR)$(CGINAME)
	rm -f interface.cgi

