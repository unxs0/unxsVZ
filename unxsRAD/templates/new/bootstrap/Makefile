#
#FILE
#	unxsRAD/interfaces/bootstrap/Makefile
#	templates/new
#AUTHOR/LEGAL
#	(C) 2017-2018 Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies. See included LICENSE file.
#PURPOSE
#	General purpose RAD4 bootstrap interface Makefile
#	Must customize CGINAME 
#

GIT_VERSION := $(shell git describe --dirty --always --tags)
CGINAME=mobile.cgi

CFLAGS= -Wall -DdsGitVersion=\"$(GIT_VERSION)\"
LIBS=-L/usr/lib64/mysql -L/usr/lib/openisp -L/usr/lib/oath -L/usr/lib64 -lmysqlclient -ltemplate -lz -lcrypt -lm -lssl -loath

help:
	@echo "These are the main make options available"
	@echo "make interface.cgi"
	@echo "make install"
	@echo "make install-test"
	@echo "make install-bootstrap"

interface.cgi: {{funcMakefileObjects}} calendar.o user.o main.o cgi.o mysqlconnect.o
	cc {{funcMakefileObjects}} calendar.o user.o main.o cgi.o mysqlconnect.o -o interface.cgi $(LIBS)

{{funcMakefileRules}}

main.o: main.c interface.h local.h ../mysqlrad.h
	cc -c main.c -o main.o $(CFLAGS) 

user.o: user.c interface.h local.h ../mysqlrad.h
	cc -c user.c -o user.o $(CFLAGS)

calendar.o: calendar.c interface.h local.h ../mysqlrad.h
	cc -c calendar.c -o calendar.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c  local.h interface.h
	cc -o mysqlconnect.o -c mysqlconnect.c $(CFLAGS)

cgi.o: cgi.c cgi.h
	cc -o cgi.o -c cgi.c $(CFLAGS)

clean:
	rm -f *.o *.cgi
	
install-test: interface.cgi
	install -m 510 -g apache -s interface.cgi /var/www/unxs/html/{{cProject}}/test/$(CGINAME)
	rm -f interface.cgi

install: interface.cgi
	install -m 510 -g apache -s interface.cgi /var/www/unxs/html/{{cProject}}/$(CGINAME)
	rm -f interface.cgi

install-bootstrap: 
	mkdir -p /var/www/unxs/html/bs/css/
	mkdir -p /var/www/unxs/html/bs/js/
	mkdir -p /var/www/unxs/html/bs/fonts/
	cp css/*.css /var/www/unxs/html/bs/css/
	cp js/*.js /var/www/unxs/html/bs/js/
	cp fonts/* /var/www/unxs/html/bs/fonts/

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

