#
#FILE
#	svn ID removed
#	Makefile
#PURPOSE
#	Create a small uClibC capable libradius.a
#	With a simple API see PLAN file in this dir
#	That will allow -for example- NoDog.cgi captive portal
#	and other software the ability to AAA with standard RADIUS
#	servers.
#NOTES
#	We are starting out by cleaning up and out Cistron src dir
#	and using radclient as the base for our interface.
#
#

CC	= gcc
#CFLAGS	= -Wall -g
CFLAGS	= -Wall
LDFLAGS	=

#Fine for standard Linux and uClibC buildroot Linux
LIBS = -lcrypt

LIB_OBJS      = dict.o util.o md5.o attrprint.o radius.o log.o pair.o encrattr.o radauth.o radacct.o
INCLUDES       = radius.h conf.h

all: liburadius.a

liburadius.a: $(LIB_OBJS)
	ar -r liburadius.a $(LIB_OBJS)

radacct.o: radacct.c $(INCLUDES)
	$(CC) $(CFLAGS) -c radacct.c

radauth.o: radauth.c $(INCLUDES)
	$(CC) $(CFLAGS) -c radauth.c

dict.o: dict.c $(INCLUDES)
	$(CC) $(CFLAGS) -c dict.c

util.o: util.c $(INCLUDES)
	$(CC) $(CFLAGS) -c util.c

md5.o: md5.c md5.h
	$(CC) $(CFLAGS) -c md5.c

attrprint.o: attrprint.c $(INCLUDES)
	$(CC) $(CFLAGS) $(MYSQL) -c attrprint.c

radius.o: radius.c $(INCLUDES)
	$(CC) $(CFLAGS) -c radius.c

log.o: log.c $(INCLUDES)
	$(CC) $(CFLAGS) $(MYSQL) -c log.c

pair.o: pair.c $(INCLUDES)
	$(CC) $(CFLAGS) $(PAM) -c pair.c

encrattr.o: encrattr.c $(INCLUDES)
	$(CC) $(CFLAGS) -c encrattr.c

install: liburadius.a uradius.h dictionary
	mkdir -p /usr/lib/openisp
	mkdir -p /usr/include/openisp
	mkdir -p /usr/local/etc/raddb
	install liburadius.a /usr/lib/openisp/liburadius.a
	install uradius.h /usr/include/openisp/uradius.h
	install dictionary /usr/local/etc/raddb/dictionary
	rm liburadius.a

clean:
	rm -f *.o liburadius.a
