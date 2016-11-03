#
#FILE
#	makefile
#	svn ID removed
#AUTHOR/LEGAL
#	(C) 2006-2009 Gary Wallis for Unixservice, LLC.
# 	LGPL License applies
#PURPOSE
#	Create and install openisp static template library
#	This library allows for very fast {{cVarName}} to cVarValue pair
#	substitution including special multiline nested (recursive) function cVarName's.
#

CFLAGS=-Wall

LIBS=-L/usr/lib/mysql -lmysqlclient -lz -lssl

all: libtemplate.a

libtemplate.a: template.o template.h
	ar r libtemplate.a template.o

template.o: template.c template.h
	cc -c template.c -o template.o $(CFLAGS)

clean:
	rm -f *.o *.a

install: libtemplate.a template.h
	mkdir -p /usr/lib/openisp
	mkdir -p /usr/include/openisp
	install libtemplate.a /usr/lib/openisp/libtemplate.a
	install template.h /usr/include/openisp/template.h
	rm libtemplate.a
