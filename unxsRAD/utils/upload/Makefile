#
#FILE
#	Makefile
#AUTHOR/LEGAL
#	(C) 2018-2019 Gary Wallis for Unixservice, LLC.
# 	LGPL License applies
#PURPOSE
#	Create and install static template library for uploading images
#

CFLAGS=-Wall

LIBS= /usr/lib/openisp/libupload.a

all: libupload.a

libupload.a: upload.o upload.h
	ar r libupload.a upload.o

upload.o: upload.c upload.h
	cc -c upload.c -o upload.o $(CFLAGS)

clean:
	rm -f *.o *.a test
	rm -f /var/www/unxs/html/upload.cgi
	rm -f /var/www/unxs/html/test.html

install: libupload.a upload.h
	mkdir -p /usr/lib/openisp
	mkdir -p /usr/include/openisp
	install libupload.a /usr/lib/openisp/libupload.a
	install upload.h /usr/include/openisp/upload.h
	rm libupload.a

test: test.o
	cc test.o -o test $(CFLAGS) $(LIBS)

test.o: test.c
	cc -c test.c -o test.o $(CFLAGS)

install-test: test
	install -s test /var/www/unxs/html/upload.cgi
	rm -f test
	cp test.html  /var/www/unxs/html/
