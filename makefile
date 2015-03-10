#
#FILE
#	unxsVZ/makefile
#	$Id$
#AUTHOR/LEGAL
#	(C) 2001-2015 Gary Wallis for Unixservice, LLC. GPLv2 license applies.
#
#NOTES
#	We only develop and test on Linux CentOS-6 and distribute via yum and rpm.
#	Feel free to repackage for your OS and let us know.
#

CFLAGS=-Wall
LIBS=-L/usr/lib/mysql -L/usr/lib64/mysql -L/usr/lib/openisp -L/usr/lib/oath -lmysqlclient -lz -lcrypt -lm -lssl -lucidr -ltemplate -loath -lunxsvz
CGIDIR=cgi-bin
#CGIDIR=cgi-bin/alpha

help:
	@echo "main make options available"
	@echo "	install-libunxsvz (required for unxsVZ and cgi)"
	@echo "	install-unxsVZ (standalone job queue processor)"
	@echo "	install (unxsVZ.cgi)"
	@echo "	install-all (unxsVZ and unxsVZ.cgi)"

unxsVZ.cgi: tdatacenter.o tnode.o tcontainer.o tproperty.o ttype.o tperm.o tostemplate.o tnameserver.o \
	tsearchdomain.o tconfig.o tip.o tgrouptype.o tgroup.o tgroupglue.o tclient.o tauthorize.o \
	ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tlogmonth.o tmonth.o tglossary.o \
	tjob.o tjobstatus.o tstatus.o tconfiguration.o  glossary.o main.o cgi.o mysqlconnect.o
	cc tdatacenter.o tnode.o tcontainer.o tproperty.o ttype.o tperm.o tostemplate.o tnameserver.o \
		tsearchdomain.o tconfig.o tip.o tgrouptype.o tgroup.o tgroupglue.o tclient.o \
		tauthorize.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o \
		tlogmonth.o tmonth.o tglossary.o tjob.o tjobstatus.o tstatus.o tconfiguration.o \
		glossary.o main.o cgi.o mysqlconnect.o -o unxsVZ.cgi $(LIBS) 

###
#new standalone job queue processor
install-unxsVZ: unxsVZ
	install -s unxsVZ /usr/sbin/unxsVZ
	@ rm unxsVZ

unxsVZ: jobqueue.o unxsvz.o mysqlconnect.o 
	cc jobqueue.o unxsvz.o mysqlconnect.o -o unxsVZ $(LIBS)

unxsvz.o: unxsvz.c mysqlrad.h local.h
	cc -c unxsvz.c -o unxsvz.o $(CFLAGS)

#lib
libunxsvz.a: libunxsvz.o libunxsvz.h mysqlconnect.o local.h
	ar r libunxsvz.a libunxsvz.o

libunxsvz.o: libunxsvz.c libunxsvz.h mysqlconnect.o local.h
	cc -c libunxsvz.c -o libunxsvz.o $(CFLAGS)

install-libunxsvz: libunxsvz.a libunxsvz.h local.h
	mkdir -p /usr/lib/openisp
	mkdir -p /usr/include/openisp
	install libunxsvz.a /usr/lib/openisp/libunxsvz.a
	install libunxsvz.h /usr/include/openisp/unxsvz.h
	rm libunxsvz.a
###

mysqlconnect.o: mysqlconnect.c mysqlrad.h local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

tdatacenter.o: tdatacenter.c mysqlrad.h language.h tdatacenterfunc.h local.h
	cc -c tdatacenter.c -o tdatacenter.o $(CFLAGS)

tnode.o: tnode.c mysqlrad.h language.h tnodefunc.h local.h
	cc -c tnode.c -o tnode.o $(CFLAGS)

tcontainer.o: tcontainer.c mysqlrad.h language.h tcontainerfunc.h local.h tcontainerfunc-newcontainer.c
	cc -c tcontainer.c -o tcontainer.o $(CFLAGS)

tproperty.o: tproperty.c mysqlrad.h language.h tpropertyfunc.h local.h
	cc -c tproperty.c -o tproperty.o $(CFLAGS)

ttype.o: ttype.c mysqlrad.h language.h ttypefunc.h local.h
	cc -c ttype.c -o ttype.o $(CFLAGS)

tperm.o: tperm.c mysqlrad.h language.h tpermfunc.h local.h
	cc -c tperm.c -o tperm.o $(CFLAGS)

tostemplate.o: tostemplate.c mysqlrad.h language.h tostemplatefunc.h local.h
	cc -c tostemplate.c -o tostemplate.o $(CFLAGS)

tnameserver.o: tnameserver.c mysqlrad.h language.h tnameserverfunc.h local.h
	cc -c tnameserver.c -o tnameserver.o $(CFLAGS)

tsearchdomain.o: tsearchdomain.c mysqlrad.h language.h tsearchdomainfunc.h local.h
	cc -c tsearchdomain.c -o tsearchdomain.o $(CFLAGS)

tconfig.o: tconfig.c mysqlrad.h language.h tconfigfunc.h local.h
	cc -c tconfig.c -o tconfig.o $(CFLAGS)

tip.o: tip.c mysqlrad.h language.h tipfunc.h local.h
	cc -c tip.c -o tip.o $(CFLAGS)

tgrouptype.o: tgrouptype.c mysqlrad.h language.h tgrouptypefunc.h local.h
	cc -c tgrouptype.c -o tgrouptype.o $(CFLAGS)

tgroup.o: tgroup.c mysqlrad.h language.h tgroupfunc.h local.h
	cc -c tgroup.c -o tgroup.o $(CFLAGS)

tgroupglue.o: tgroupglue.c mysqlrad.h language.h tgroupgluefunc.h local.h
	cc -c tgroupglue.c -o tgroupglue.o $(CFLAGS)

tclient.o: tclient.c mysqlrad.h language.h tclientfunc.h local.h
	cc -c tclient.c -o tclient.o $(CFLAGS)

tauthorize.o: tauthorize.c mysqlrad.h language.h tauthorizefunc.h local.h
	cc -c tauthorize.c -o tauthorize.o $(CFLAGS)

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

tlogmonth.o: tlogmonth.c mysqlrad.h language.h tlogmonthfunc.h local.h
	cc -c tlogmonth.c -o tlogmonth.o $(CFLAGS)

tmonth.o: tmonth.c mysqlrad.h language.h tmonthfunc.h local.h
	cc -c tmonth.c -o tmonth.o $(CFLAGS)

tglossary.o: tglossary.c mysqlrad.h language.h tglossaryfunc.h local.h
	cc -c tglossary.c -o tglossary.o $(CFLAGS)

tjob.o: tjob.c mysqlrad.h language.h tjobfunc.h local.h
	cc -c tjob.c -o tjob.o $(CFLAGS)

tjobstatus.o: tjobstatus.c mysqlrad.h language.h tjobstatusfunc.h local.h
	cc -c tjobstatus.c -o tjobstatus.o $(CFLAGS)

tstatus.o: tstatus.c mysqlrad.h language.h tstatusfunc.h local.h
	cc -c tstatus.c -o tstatus.o $(CFLAGS)

tconfiguration.o: tconfiguration.c mysqlrad.h language.h tconfigurationfunc.h local.h
	cc -c tconfiguration.c -o tconfiguration.o $(CFLAGS)

glossary.o : glossary.c mysqlrad.h local.h
	cc -c glossary.c -o glossary.o $(CFLAGS)

main.o: main.c mysqlrad.h mainfunc.h language.h local.h
	cc -c main.c -o main.o $(CFLAGS)

cgi.o: cgi.h cgi.c
	cc -c cgi.c -o cgi.o $(CFLAGS)

jobqueue.o: jobqueue.c mysqlrad.h local.h
	cc -c jobqueue.c -o jobqueue.o $(CFLAGS)

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o

install-all: unxsVZ unxsVZ.cgi 
	install -s unxsVZ.cgi /var/www/unxs/$(CGIDIR)/unxsVZ.cgi
	install -s unxsVZ /usr/sbin/unxsVZ
	@ rm unxsVZ.cgi
	@ rm unxsVZ

install: unxsVZ.cgi
	install -s unxsVZ.cgi /var/www/unxs/$(CGIDIR)/unxsVZ.cgi
	@ rm unxsVZ.cgi
