#
# FILE
# 	iDNS/tools/makefile
# 	$Id$
# AUTHOR/LEGAL
# (C) 2001-2007 Gary Wallis under GPL License see www.fsf.org for details

CFLAGS= -DLinux -Wall
LIBS= /usr/lib/mysql/libmysqlclient.a -lz -lcrypt -lm /usr/lib/openisp/libucidr.a

all: idns-importzones

idns-importzones: idns-importzones.o 
	cc idns-importzones.o -o idns-importzones $(LIBS)

idns-importzones.o: idns-importzones.c ../../local.h ../../mysqlrad.h
	cc -c idns-importzones.c -o idns-importzones.o $(CFLAGS)

install: idns-importzones
	install -s idns-importzones /usr/sbin/idns-importzones
	rm -f idns-importzones
clean:
	rm -f *.o

