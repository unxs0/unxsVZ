#
#FILE
#	makefile
#AUTHOR/LEGAL
#	(C) 2017 Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies. See included LICENSE file.
#PURPOSE
#	Test server side w2ui json server
#

CGINAME= unxsvzW2UI.cgi

CFLAGS=-Wall
LIBS=-L/usr/lib/mysql -lmysqlclient -lz -lcrypt -lm -lssl

interface.cgi: main.o mysqlconnect.o local.h
	cc main.o mysqlconnect.o -o interface.cgi $(LIBS)

mysqlconnect.o: mysqlconnect.c interface.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS) 

main.o: main.c interface.h interface.h
	cc -c main.c -o main.o $(CFLAGS) 

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
