#
#FILE
#	makefile
#AUTHOR/LEGAL
#	(C) 2017 Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies. See included LICENSE file.
#PURPOSE
#	Test server side FooTable json server
#

CGINAME= unxsvzFT.cgi

CFLAGS=-Wall
LIBS=-L/usr/lib/mysql -L/usr/lib64/mysql -lmysqlclient -lz -lcrypt -lm -lssl

interface.cgi: main.o mysqlconnect.o cgi.o local.h
	cc main.o mysqlconnect.o cgi.o -o interface.cgi $(LIBS)

mysqlconnect.o: mysqlconnect.c interface.h local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS) 

main.o: main.c interface.h local.h
	cc -c main.c -o main.o $(CFLAGS) 

cgi.o: cgi.c cgi.h interface.h
	cc -c cgi.c -o cgi.o $(CFLAGS) 

clean:
	rm -f *.o *.cgi
	
install-test: interface.cgi
	install -m 510 -g apache -s interface.cgi /var/www/cgi-bin/test/$(CGINAME)
	rm -f interface.cgi

install: interface.cgi
	install -m 510 -g apache -s interface.cgi /var/www/cgi-bin/$(CGINAME)
	rm -f interface.cgi

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

install-www:
	@ mkdir -p /var/www/html/FT /var/www/html/FT/css /var/www/html/FT/js;
	@ cp -a html/*.html /var/www/html/FT/; cp -a css/*.css /var/www/html/FT/css; cp -a js/*.js /var/www/html/FT/js;
