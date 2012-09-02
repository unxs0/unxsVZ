#
#FILE
#	unxsSPS/makefile
#	$Id: makefile 1858 2012-01-20 19:25:14Z Dylan $
#AUTHOR/LEGAL
#	(C) 2012-2013 Gary Wallis for Unixservice. GPLv2 license applies.
#
#NOTES
#	We only develop and test on Linux CentOS-5 and distribute via yum and rpm.
#	Feel free to repackage for your OS and let us know.
#

CFLAGS=-Wall
LIBS=-L/usr/lib/mysql -L/usr/lib/openisp -lmysqlclient -lz -lcrypt -lm -lssl -lucidr -ltemplate

all: unxsSPS.cgi

unxsSPS.cgi: tdatacenter.o tdid.o tgrouptype.o tgroup.o tgroupglue.o tclient.o tauthorize.o \
	ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tlogmonth.o tmonth.o tglossary.o \
	tjob.o tjobstatus.o tstatus.o tconfiguration.o  jobqueue.o main.o cgi.o mysqlconnect.o
	cc tdatacenter.o tdid.o tgrouptype.o tgroup.o tgroupglue.o tclient.o \
		tauthorize.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o \
		tlogmonth.o tmonth.o tglossary.o tjob.o tjobstatus.o tstatus.o tconfiguration.o \
		jobqueue.o main.o cgi.o mysqlconnect.o -o unxsSPS.cgi $(LIBS) 

mysqlconnect.o: mysqlconnect.c mysqlrad.h local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

tdatacenter.o: tdatacenter.c mysqlrad.h language.h tdatacenterfunc.h local.h
	cc -c tdatacenter.c -o tdatacenter.o $(CFLAGS)

tdid.o: tdid.c mysqlrad.h language.h tdidfunc.h local.h
	cc -c tdid.c -o tdid.o $(CFLAGS)

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

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o
	rm -f unxsSPS.cgi

install: unxsSPS.cgi
	install -s unxsSPS.cgi /var/www/unxs/cgi-bin/unxsSPS.cgi
	@ rm unxsSPS.cgi
