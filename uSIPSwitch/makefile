#
# $Id$
# uSIPSwitch/makefile
#
# (C) 2012 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Support for non CentOS 5+ linux systems is not tested or supported yet.
#

#We code with all kinds of gcc allowed crazyness. Show our trangressions with pedantic flag
#CFLAGS= -Wall -pedantic
CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lm /usr/lib64/libmemcached.so /usr/lib64/libevent.so
LIBS2= -L/usr/lib/mysql -lmysqlclient -lssl -lm /usr/lib64/libmemcached.so /usr/lib/openisp/libdns.a

all: uSIPSwitch

uSIPSwitch: main.o mysqlconnect.o
	cc main.o mysqlconnect.o -o uSIPSwitch $(LIBS) 

unxsLoadNatPBX: loadnatpbx.o mysqlconnect.o
	cc loadnatpbx.o mysqlconnect.o -o unxsLoadNatPBX $(LIBS2) 

unxsLoadFromSPS: loadfromsps.o mysqlconnect.o
	cc loadfromsps.o mysqlconnect.o -o unxsLoadFromSPS $(LIBS2) 

main.o: main.c sipproxy.h local.h module/parsemessage.c module/process.c module/postparsecheck.c
	cc -c main.c -o main.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c sipproxy.h local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

loadnatpbx.o: loadnatpbx.c mysqlconnect.c sipproxy.h local.h
	cc -c loadnatpbx.c -o loadnatpbx.o $(CFLAGS)

loadfromsps.o: loadfromsps.c mysqlconnect.c sipproxy.h local.h
	cc -c loadfromsps.c -o loadfromsps.o $(CFLAGS)

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o uSIPSwitch

install-unxsLoadFromSPS: unxsLoadFromSPS
	install -s unxsLoadFromSPS /usr/sbin/unxsLoadFromSPS
	rm unxsLoadFromSPS

install: uSIPSwitch
	install -s uSIPSwitch /usr/sbin/uSIPSwitch
	rm uSIPSwitch

install-run: uSIPSwitch
	/usr/sbin/stopsipproxy.sh
	install -s uSIPSwitch /usr/sbin/uSIPSwitch
	rm uSIPSwitch
	/usr/sbin/uSIPSwitch $(uSIPSwitchIP) $(uSIPSwitchPort)

install-test: uSIPSwitch
	/usr/sbin/stopsipproxy.sh
	install -s uSIPSwitch /usr/sbin/uSIPSwitch
	rm uSIPSwitch
	/usr/sbin/uSIPSwitch
	./test.sh


libctest: ctest.c
	cc -Wall -fPIC -c ctest.c
	cc -shared -Wl,-soname,libctest.so.1 -o libctest.so.1.0 ctest.o
	mv libctest.so.1.0 /var/lib
	ln -sf /var/lib/libctest.so.1.0 /var/lib/libctest.so
	ln -sf /var/lib/libctest.so.1.0 /var/lib/libctest.so.1

dltest: dltest.c
	gcc -rdynamic -o dltest dltest.c -ldl

libs: libdns.a

libdns.a: dns.o dns.h
	ar r libdns.a dns.o

dns.o: dns.c dns.h
	cc -c dns.c -o dns.o $(CFLAGS)

install-libs: libdns.a dns.h
	mkdir -p /usr/lib/openisp
	mkdir -p /usr/include/openisp
	install libdns.a /usr/lib/openisp/libdns.a
	install dns.h /usr/include/openisp/dns.h
	rm libdns.a
