#
#FILE
#       unxsSPS/makefile
#       $Id: makefile.template 2979 2011-12-11 15:24:44Z Gary $
#AUTHOR/LEGAL
#       (C) 2001-2012 Gary Wallis for Unixservice. GPLv2 license applies.
#
#NOTES
#       We only develop and test on Linux CentOS-5 and distribute via yum and rpm.
#       Feel free to repackage for your OS and let us know.
#

#CFLAGS=-O -DLinux -Wall -pedantic
CFLAGS= -DLinux -Wall
RELEASE= 0.1
#If using lib64 make sure you have adjusted if any errors emerge
LIBS= -L/usr/lib/openisp -L/usr/lib64/mysql -lz -lcrypt -lm -lssl -ltemplate -lmysqlclient
CGIDIR=/var/www/unxs/cgi-bin/

all: unxsSPS.cgi unxsSPS

#New standalone CLI job queue model
unxsSPS: unxssps.o mysqlconnect.o
	cc unxssps.o mysqlconnect.o -o unxsSPS $(LIBS)

unxssps.o: unxssps.c mysqlrad.h local.h mysqlconnect.c
	cc -c unxssps.c -o unxssps.o $(CFLAGS)

#unxsRAD comes with a sample standalone program rename
unxssps.c: unxsrad.c
	@ if [ ! -f unxssps.c ];then cp -i unxsrad.c unxssps.c; fi

#included standard RAD4 objects
unxsSPS.cgi: tpbx.o tdid.o tgateway.o trule.o tprefix.o tcarrier.o tcdr.o taddress.o tcluster.o tgatewaytype.o ttimeinterval.o tgroup.o tgroupglue.o tgrouptype.o tclient.o tauthorize.o tserver.o tstatus.o tconfiguration.o tjob.o tjobstatus.o tglossary.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tlogmonth.o tmonth.o  mysqlconnect.o main.o cgi.o
	cc tpbx.o tdid.o tgateway.o trule.o tprefix.o tcarrier.o tcdr.o taddress.o tcluster.o tgatewaytype.o ttimeinterval.o tgroup.o tgroupglue.o tgrouptype.o tclient.o tauthorize.o tserver.o tstatus.o tconfiguration.o tjob.o tjobstatus.o tglossary.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tlogmonth.o tmonth.o  mysqlconnect.o main.o cgi.o -o unxsSPS.cgi $(LIBS) 

tpbx.o: tpbx.c mysqlrad.h language.h tpbxfunc.h local.h
	cc -c tpbx.c -o tpbx.o $(CFLAGS)

tdid.o: tdid.c mysqlrad.h language.h tdidfunc.h local.h
	cc -c tdid.c -o tdid.o $(CFLAGS)

tgateway.o: tgateway.c mysqlrad.h language.h tgatewayfunc.h local.h
	cc -c tgateway.c -o tgateway.o $(CFLAGS)

trule.o: trule.c mysqlrad.h language.h trulefunc.h local.h
	cc -c trule.c -o trule.o $(CFLAGS)

tprefix.o: tprefix.c mysqlrad.h language.h tprefixfunc.h local.h
	cc -c tprefix.c -o tprefix.o $(CFLAGS)

tcarrier.o: tcarrier.c mysqlrad.h language.h tcarrierfunc.h local.h
	cc -c tcarrier.c -o tcarrier.o $(CFLAGS)

tcdr.o: tcdr.c mysqlrad.h language.h tcdrfunc.h local.h
	cc -c tcdr.c -o tcdr.o $(CFLAGS)

taddress.o: taddress.c mysqlrad.h language.h taddressfunc.h local.h
	cc -c taddress.c -o taddress.o $(CFLAGS)

tcluster.o: tcluster.c mysqlrad.h language.h tclusterfunc.h local.h
	cc -c tcluster.c -o tcluster.o $(CFLAGS)

tgatewaytype.o: tgatewaytype.c mysqlrad.h language.h tgatewaytypefunc.h local.h
	cc -c tgatewaytype.c -o tgatewaytype.o $(CFLAGS)

ttimeinterval.o: ttimeinterval.c mysqlrad.h language.h ttimeintervalfunc.h local.h
	cc -c ttimeinterval.c -o ttimeinterval.o $(CFLAGS)

tgroup.o: tgroup.c mysqlrad.h language.h tgroupfunc.h local.h
	cc -c tgroup.c -o tgroup.o $(CFLAGS)

tgroupglue.o: tgroupglue.c mysqlrad.h language.h tgroupgluefunc.h local.h
	cc -c tgroupglue.c -o tgroupglue.o $(CFLAGS)

tgrouptype.o: tgrouptype.c mysqlrad.h language.h tgrouptypefunc.h local.h
	cc -c tgrouptype.c -o tgrouptype.o $(CFLAGS)

tclient.o: tclient.c mysqlrad.h language.h tclientfunc.h local.h
	cc -c tclient.c -o tclient.o $(CFLAGS)

tauthorize.o: tauthorize.c mysqlrad.h language.h tauthorizefunc.h local.h
	cc -c tauthorize.c -o tauthorize.o $(CFLAGS)

tserver.o: tserver.c mysqlrad.h language.h tserverfunc.h local.h
	cc -c tserver.c -o tserver.o $(CFLAGS)

tstatus.o: tstatus.c mysqlrad.h language.h tstatusfunc.h local.h
	cc -c tstatus.c -o tstatus.o $(CFLAGS)

tconfiguration.o: tconfiguration.c mysqlrad.h language.h tconfigurationfunc.h local.h
	cc -c tconfiguration.c -o tconfiguration.o $(CFLAGS)

tjob.o: tjob.c mysqlrad.h language.h tjobfunc.h local.h
	cc -c tjob.c -o tjob.o $(CFLAGS)

tjobstatus.o: tjobstatus.c mysqlrad.h language.h tjobstatusfunc.h local.h
	cc -c tjobstatus.c -o tjobstatus.o $(CFLAGS)

tglossary.o: tglossary.c mysqlrad.h language.h tglossaryfunc.h local.h
	cc -c tglossary.c -o tglossary.o $(CFLAGS)

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



#standard RAD4 
main.o: main.c mysqlrad.h mainfunc.h language.h local.h
	cc -c main.c -o main.o $(CFLAGS)

cgi.o: cgi.h cgi.c
	cc -c cgi.c -o cgi.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c mysqlrad.h local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o

cleandist:
	rm -f *.o *.gz

install: unxsSPS.cgi unxsSPS
	install -s unxsSPS.cgi $(CGIDIR)unxsSPS.cgi
	install -s unxsSPS /usr/sbin/unxsSPS
	rm unxsSPS.cgi
	rm unxsSPS

distribution:
	cd ..;  tar czvf unxsSPS/unxsSPS$(RELEASE).tar.gz --exclude .svn\
		unxsSPS/*.h\
		unxsSPS/*.c\
		unxsSPS/data\
		unxsSPS/docs\
		unxsSPS/makefile\
		unxsSPS/CHANGES\
		unxsSPS/LICENSE\
		unxsSPS/INSTALL
