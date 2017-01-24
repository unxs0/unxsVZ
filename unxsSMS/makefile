#
# $Id$
# interfaces/sms/makefile
#
# (C) 2010 Gary Wallis for Unixservice, LLC.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#	For help contact support @ openisp . net
#	Support for non CentOS 5+ linux systems is not tested or supported yet.
#

CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lm -lcurl
LIBS2= -lssl -lm -lcurl

all: unxsSMS unxsDOApi

unxsDOApi: digitalocean.api.o
	cc digitalocean.api.o -o unxsDOApi $(LIBS2) 

digitalocean.api.o: digitalocean.api.c local.h
	cc -c digitalocean.api.c -o digitalocean.api.o $(CFLAGS)

unxsGCApi: gcloud.api.o
	cc gcloud.api.o -o unxsGCApi $(LIBS2) 

gcloud.api.o: gcloud.api.c local.h
	cc -c gcloud.api.c -o gcloud.api.o $(CFLAGS)

unxsSMS: sms.o mysqlconnect.o
	cc sms.o mysqlconnect.o -o unxsSMS $(LIBS) 

sms.o: sms.c mysqlrad.h local.h
	cc -c sms.c -o sms.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c mysqlrad.h local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o unxsSMS

install: unxsSMS
	install -s unxsSMS /usr/sbin/unxsSMS
	rm unxsSMS


