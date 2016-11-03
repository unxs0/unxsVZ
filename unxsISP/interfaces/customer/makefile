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

CGINAME= ispClient.cgi

CFLAGS= -DLinux -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient /usr/lib/openisp/libtemplate-branch.a /usr/lib/openisp/libucidr.a -lz -lcrypt -lm -lssl

interface.cgi: main.o customer.o glossary.o invoice.o product.o https.o payment.o registration.o purchase.o ../../cgi.o
	cc main.o customer.o glossary.o invoice.o product.o https.o payment.o registration.o purchase.o ../../cgi.o -o interface.cgi $(LIBS)

main.o: main.c interface.h ../../local.h ../../mysqlrad.h
	cc -c main.c -o main.o $(CFLAGS) 

customer.o: customer.c interface.h ../../local.h ../../mysqlrad.h
	cc -c customer.c -o customer.o $(CFLAGS)

glossary.o : glossary.c interface.h ../../local.h ../../mysqlrad.h
	cc -c glossary.c -o glossary.o $(CFLAGS)

invoice.o: invoice.c interface.h ../../local.h ../../mysqlrad.h
	cc -c invoice.c -o invoice.o $(CFLAGS)

product.o : product.c interface.h ../../local.h ../../mysqlrad.h
	cc -c product.c -o product.o $(CFLAGS)

https.o : https.c interface.h ../../local.h ../../mysqlrad.h
	cc -c https.c -o https.o $(CFLAGS)

payment.o: payment.c interface.h ../../local.h ../../mysqlrad.h
	cc  -c payment.c -o payment.o $(CFLAGS)

registration.o: registration.c interface.h ../../local.h ../../mysqlrad.h
	cc -c registration.c -o registration.o $(CFLAGS)

purchase.o : purchase.c interface.h ../../local.h ../../mysqlrad.h
	cc -c purchase.c -o purchase.o $(CFLAGS)

clean:
	rm -f *.o *.cgi
	
install: interface.cgi
	install -s interface.cgi /var/www/unxs/cgi-bin/$(CGINAME)
	rm -f interface.cgi

