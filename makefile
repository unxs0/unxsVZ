#
#FILE
#	unxsVZ/makefile
#	$Id$
#AUTHOR/LEGAL
#	(C) 2001-2009 Gary Wallis for Unixservice. GPLv2 license applies.
#
#NOTES
#	We only develop and test on Linux CentOS-5 and distribute via yum and rpm.
#	Feel free to repackage for your OS and let us know.
#

CFLAGS=-Wall
LIBS=-L/usr/lib64/mysql -L/usr/lib/openisp -lmysqlclient -lz -lcrypt -lm -lssl -lucidr -ltemplate

all: unxsVZ.cgi

unxsVZ.cgi: tdatacenter.o tnode.o tcontainer.o tproperty.o ttype.o tostemplate.o tnameserver.o tsearchdomain.o tconfig.o tip.o tgrouptype.o tgroup.o tgroupglue.o tclient.o tauthorize.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tlogmonth.o tmonth.o tglossary.o tjob.o tjobstatus.o tstatus.o tconfiguration.o  jobqueue.o main.o cgi.o
	cc tdatacenter.o tnode.o tcontainer.o tproperty.o ttype.o tostemplate.o tnameserver.o tsearchdomain.o tconfig.o tip.o tgrouptype.o tgroup.o tgroupglue.o tclient.o tauthorize.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tlogmonth.o tmonth.o tglossary.o tjob.o tjobstatus.o tstatus.o tconfiguration.o  jobqueue.o main.o cgi.o -o unxsVZ.cgi $(LIBS) 

tdatacenter.o: tdatacenter.c mysqlrad.h language.h tdatacenterfunc.h local.h
	cc -c tdatacenter.c -o tdatacenter.o $(CFLAGS)

tnode.o: tnode.c mysqlrad.h language.h tnodefunc.h local.h
	cc -c tnode.c -o tnode.o $(CFLAGS)

tcontainer.o: tcontainer.c mysqlrad.h language.h tcontainerfunc.h local.h
	cc -c tcontainer.c -o tcontainer.o $(CFLAGS)

tproperty.o: tproperty.c mysqlrad.h language.h tpropertyfunc.h local.h
	cc -c tproperty.c -o tproperty.o $(CFLAGS)

ttype.o: ttype.c mysqlrad.h language.h ttypefunc.h local.h
	cc -c ttype.c -o ttype.o $(CFLAGS)

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


main.o: main.c mysqlrad.h mainfunc.h language.h local.h
	cc -c main.c -o main.o $(CFLAGS)

cgi.o: cgi.h cgi.c
	cc -c cgi.c -o cgi.o $(CFLAGS)

jobqueue.o: jobqueue.c mysqlrad.h local.h
	cc -c jobqueue.c -o jobqueue.o $(CFLAGS)

clean:
	rm -f *.o

install: unxsVZ.cgi
	install -s unxsVZ.cgi /var/www/unxs/cgi-bin/unxsVZ.cgi
	rm unxsVZ.cgi
