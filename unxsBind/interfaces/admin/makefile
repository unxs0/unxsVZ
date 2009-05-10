#
#FILE
#	makefile
#	$Id: makefile 660 2008-12-18 18:48:48Z hus $
#AUTHOR
#	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice
#PURPOSE
#	General purpose RAD3 interface makefile
#	Must customize CGINAME 
#

CGINAME= idnsAdmin.cgi

CFLAGS= -DLinux -Wall
LIBS= /usr/lib/mysql/libmysqlclient.a /usr/lib/openisp/libtemplate.a /usr/lib/openisp/libucidr.a -lz -lcrypt -lm -lssl

interface.cgi: main.o adminuser.o customer.o customercontact.o glossary.o zone.o resource.o bulkop.o report.o \
		restorezone.o restoreresource.o dashboard.o ../../cgi.o /usr/lib/openisp/libtemplate.a /usr/lib/openisp/libucidr.a \
		/usr/include/openisp/template.h /usr/include/openisp/ucidr.h
	cc main.o adminuser.o customer.o customercontact.o glossary.o zone.o resource.o bulkop.o report.o \
	restorezone.o restoreresource.o dashboard.o ../../cgi.o -o interface.cgi $(LIBS)

main.o: main.c interface.h ../../local.h ../../mysqlrad.h /usr/include/openisp/template.h /usr/include/openisp/ucidr.h
	cc -c main.c -o main.o $(CFLAGS) 

customercontact.o: customercontact.c interface.h ../../local.h ../../mysqlrad.h
	cc -c customercontact.c -o customercontact.o $(CFLAGS) 

customer.o: customer.c interface.h ../../local.h ../../mysqlrad.h
	cc -c customer.c -o customer.o $(CFLAGS)

adminuser.o : adminuser.c interface.h ../../local.h ../../mysqlrad.h
	cc -c adminuser.c -o adminuser.o $(CFLAGS)	

glossary.o : glossary.c interface.h ../../local.h ../../mysqlrad.h
	cc -c glossary.c -o glossary.o $(CFLAGS)

zone.o : zone.c interface.h ../../local.h ../../mysqlrad.h
	cc -c zone.c -o zone.o $(CFLAGS)
	
resource.o : resource.c interface.h ../../local.h ../../mysqlrad.h
	cc -c resource.c -o resource.o $(CFLAGS) 

bulkop.o : bulkop.c interface.h ../../local.h ../../mysqlrad.h
	cc -c bulkop.c -o bulkop.o $(CFLAGS)

report.o : report.c interface.h ../../local.h ../../mysqlrad.h
	cc -c report.c -o report.o $(CFLAGS)

restorezone.o : restorezone.c interface.h ../../local.h ../../mysqlrad.h
	cc -c restorezone.c -o restorezone.o $(CFLAGS)

restoreresource.o : restoreresource.c interface.h ../../local.h ../../mysqlrad.h
	cc -c restoreresource.c -o restoreresource.o $(CFLAGS)

dashboard.o : dashboard.c interface.h ../../local.h ../../mysqlrad.h
	cc -c dashboard.c -o dashboard.o $(CFLAGS)

clean:
	rm -f *.o *.cgi
	
install: interface.cgi
	install -s interface.cgi $(CGIDIR)$(CGINAME)
	rm -f interface.cgi

