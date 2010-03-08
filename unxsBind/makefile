#
#File
#	makefile
#	$Id$
#
#Author/Legal
#	(C) 2001-2010 Gary Wallis and Hugo Urquiza for Unixservice, LLC.
#	This software is distributed under the GPLv3 license.
#
#Notes:
#	This is a CentOS5 rpm release version only (use CFLAG -pedantic to cleanup.)
#	
#	For help contact support @ openisp . net
#
CFLAGS= -Wall

LIBS= -L/usr/lib/mysql -L/usr/lib/openisp -lz -lcrypt -lm -lssl -lucidr -lmysqlclient

all: iDNS.cgi

iDNS.cgi: tzone.o tresource.o trrtype.o tjob.o tmailserver.o tconfiguration.o tnstype.o tnsset.o tns.o tserver.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tblock.o tview.o tregistrar.o tglossary.o tzoneimport.o tresourceimport.o tmonthhit.o tmonth.o tlogmonth.o thit.o thitmonth.o tdeletedzone.o tdeletedresource.o tclient.o tauthorize.o  bind.o main.o import.o extjobqueue.o cgi.o
	cc tzone.o tresource.o trrtype.o tjob.o tmailserver.o tconfiguration.o tnstype.o tnsset.o tns.o tserver.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tblock.o tview.o tregistrar.o tglossary.o tzoneimport.o tresourceimport.o tmonthhit.o tmonth.o tlogmonth.o thit.o thitmonth.o tdeletedzone.o tdeletedresource.o tclient.o tauthorize.o  bind.o main.o import.o extjobqueue.o cgi.o -o iDNS.cgi $(LIBS)

tzone.o: tzone.c mysqlrad.h language.h tzonefunc.h local.h 
	cc -c tzone.c -o tzone.o $(CFLAGS)

tresource.o: tresource.c mysqlrad.h language.h tresourcefunc.h local.h
	cc -c tresource.c -o tresource.o $(CFLAGS)

trrtype.o: trrtype.c mysqlrad.h language.h trrtypefunc.h local.h
	cc -c trrtype.c -o trrtype.o $(CFLAGS)

tjob.o: tjob.c mysqlrad.h language.h tjobfunc.h local.h
	cc -c tjob.c -o tjob.o $(CFLAGS)

tmailserver.o: tmailserver.c mysqlrad.h language.h tmailserverfunc.h local.h
	cc -c tmailserver.c -o tmailserver.o $(CFLAGS)

tconfiguration.o: tconfiguration.c mysqlrad.h language.h tconfigurationfunc.h local.h
	cc -c tconfiguration.c -o tconfiguration.o $(CFLAGS)

tnstype.o: tnstype.c mysqlrad.h language.h tnstypefunc.h local.h
	cc -c tnstype.c -o tnstype.o $(CFLAGS)

tnsset.o: tnsset.c mysqlrad.h language.h tnssetfunc.h local.h
	cc -c tnsset.c -o tnsset.o $(CFLAGS)

tns.o: tns.c mysqlrad.h language.h tnsfunc.h local.h
	cc -c tns.c -o tns.o $(CFLAGS)

tserver.o: tserver.c mysqlrad.h language.h tserverfunc.h local.h
	cc -c tserver.c -o tserver.o $(CFLAGS)

ttemplate.o: ttemplate.c mysqlrad.h language.h ttemplatefunc.h local.h
	cc -c ttemplate.c -o ttemplate.o $(CFLAGS)

ttemplateset.o: ttemplateset.c mysqlrad.h language.h ttemplatesetfunc.h local.h
	cc -c ttemplateset.c -o ttemplateset.o $(CFLAGS)

ttemplatetype.o: ttemplatetype.c mysqlrad.h language.h ttemplatetypefunc.h local.h
	cc -c ttemplatetype.c -o ttemplatetype.o $(CFLAGS)

tlog.o: tlog.c mysqlrad.h language.h tlogfunc.h local.h
	cc -c tlog.c -o tlog.o $(CFLAGS)

tlogtype.o: tlogtype.c mysqlrad.h language.h tlogtypefunc.h local.h
	cc -c tlogtype.c -o tlogtype.o $(CFLAGS)

tblock.o: tblock.c mysqlrad.h language.h tblockfunc.h local.h
	cc -c tblock.c -o tblock.o $(CFLAGS)

tview.o: tview.c mysqlrad.h language.h tviewfunc.h local.h
	cc -c tview.c -o tview.o $(CFLAGS)

tregistrar.o: tregistrar.c mysqlrad.h language.h tregistrarfunc.h local.h
	cc -c tregistrar.c -o tregistrar.o $(CFLAGS)

tglossary.o: tglossary.c mysqlrad.h language.h tglossaryfunc.h local.h
	cc -c tglossary.c -o tglossary.o $(CFLAGS)

tzoneimport.o: tzoneimport.c mysqlrad.h language.h tzoneimportfunc.h local.h
	cc -c tzoneimport.c -o tzoneimport.o $(CFLAGS)

tresourceimport.o: tresourceimport.c mysqlrad.h language.h tresourceimportfunc.h local.h
	cc -c tresourceimport.c -o tresourceimport.o $(CFLAGS)

tmonthhit.o: tmonthhit.c mysqlrad.h language.h tmonthhitfunc.h local.h
	cc -c tmonthhit.c -o tmonthhit.o $(CFLAGS)

tmonth.o: tmonth.c mysqlrad.h language.h tmonthfunc.h local.h
	cc -c tmonth.c -o tmonth.o $(CFLAGS)

tlogmonth.o: tlogmonth.c mysqlrad.h language.h tlogmonthfunc.h local.h
	cc -c tlogmonth.c -o tlogmonth.o $(CFLAGS)

thit.o: thit.c mysqlrad.h language.h thitfunc.h local.h
	cc -c thit.c -o thit.o $(CFLAGS)

thitmonth.o: thitmonth.c mysqlrad.h language.h thitmonthfunc.h local.h
	cc -c thitmonth.c -o thitmonth.o $(CFLAGS)

tdeletedzone.o: tdeletedzone.c mysqlrad.h language.h tdeletedzonefunc.h local.h
	cc -c tdeletedzone.c -o tdeletedzone.o $(CFLAGS)

tdeletedresource.o: tdeletedresource.c mysqlrad.h language.h tdeletedresourcefunc.h local.h
	cc -c tdeletedresource.c -o tdeletedresource.o $(CFLAGS)

tclient.o: tclient.c mysqlrad.h language.h tclientfunc.h local.h
	cc -c tclient.c -o tclient.o $(CFLAGS)

tauthorize.o: tauthorize.c mysqlrad.h language.h tauthorizefunc.h local.h
	cc -c tauthorize.c -o tauthorize.o $(CFLAGS)


main.o: main.c mysqlrad.h mainfunc.h language.h local.h
	cc -c main.c -o main.o $(CFLAGS)

cgi.o: cgi.h cgi.c
	cc -c cgi.c -o cgi.o $(CFLAGS)

bind.o: bind.c mysqlrad.h
	cc -c bind.c -o bind.o $(CFLAGS)

import.o: import.c mysqlrad.h
	cc -c import.c -o import.o $(CFLAGS)

extjobqueue.o: mysqlrad.h extjobqueue.c
	cc -c extjobqueue.c -o extjobqueue.o $(CFLAGS)


local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o iDNS.cgi

install: iDNS.cgi
	install -s iDNS.cgi /var/www/unxs/cgi-bin/iDNS.cgi
	rm iDNS.cgi
