#
#FILE
#	makefile
#	$Id: makefile 663 2008-12-18 19:00:37Z hus $
#AUTHOR
#	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice
#PURPOSE
#	General purpose RAD3 interface makefile
#	Must customize CGINAME 
#

CGINAME= idnsOrg.cgi

CFLAGS= -DLinux -Wall
iDNS_MYSQL_LIB= /usr/lib/mysql/libmysqlclient.a
LIBS= $(iDNS_MYSQL_LIB) /usr/lib/openisp/libtemplate.a /usr/lib/openisp/libucidr.a -lz -lcrypt -lm -lssl

interface.cgi: main.o zone.o resource.o glossary.o bulkop.o ../../cgi.o /usr/lib/openisp/libtemplate.a /usr/lib/openisp/libucidr.a
	cc main.o zone.o resource.o glossary.o bulkop.o ../../cgi.o -o interface.cgi $(LIBS)

main.o: main.c interface.h ../../local.h ../../mysqlrad.h /usr/include/openisp/template.h /usr/include/openisp/ucidr.h
	cc -c main.c -o main.o $(CFLAGS) 

zone.o: zone.c interface.h ../../local.h ../../mysqlrad.h
	cc -c zone.c -o zone.o $(CFLAGS) 

resource.o: resource.c interface.h ../../local.h ../../mysqlrad.h
	cc -c resource.c -o resource.o $(CFLAGS) 

glossary.o : glossary.c interface.h ../../local.h ../../mysqlrad.h
	cc -c glossary.c -o glossary.o $(CFLAGS)

bulkop.o : bulkop.c interface.h ../../local.h ../../mysqlrad.h
	cc -o bulkop.o -c bulkop.c $(CFLAGS)

clean:
	rm -f *.o *.cgi
	
install: interface.cgi
	install -s interface.cgi $(CGIDIR)$(CGINAME)
	rm -f interface.cgi

