#
#FILE
#	makefile
#	$Id: makefile 1397 2007-08-09 14:32:57Z Gary $
#AUTHOR/LEGAL
#	(C) 2006-2008 Gary Wallis for Unixservice.
# 	LGPL License applies
#PURPOSE
#	Create and install openisp RAD3 .a template library
#

CFLAGS= -Wall -DLinux

ifdef FreeBSD
	LIBS= /usr/local/lib/mysql/libmysqlclient.a -lz -lssl
else
	LIBS= /usr/lib/mysql/libmysqlclient.a -lz -lssl
endif



all: lfclibtemplate.a

lfclibtemplate.a: template.o template.h
	ar r lfclibtemplate.a template.o

template.o: template.c template.h
	cc -c template.c -o template.o $(CFLAGS)

clean:
	rm -f *.o *.a

install: lfclibtemplate.a template.h
	mkdir -p /usr/lib/openisp
	mkdir -p /usr/include/openisp
	install lfclibtemplate.a /usr/lib/openisp/lfclibtemplate.a
	install template.h /usr/include/openisp/lfctemplate.h
	rm lfclibtemplate.a
