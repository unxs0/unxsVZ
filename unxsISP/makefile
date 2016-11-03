#
# svn ID removed
#
# (C) 2001-2009 Gary Wallis and Hugo Urquiza.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#
#	Must set env variable CGIDIR with trailing / for placing cgi in
#	target directory. Ex Bash shell: CGIDIR=/cgi-bin/ ; export CGIDIR
#
#       Use mySQL version 3.23.39 or later:
#       Requires /usr/lib/mysql/libmysqlclient.a
#       Requires /usr/include/mysql/mysql.h
#
#	These mysqlRAD generated files have only been tested extensively
#	on Linux x86. Please share your ports.
#
#	For help contact support @ openisp . net
#
#	Please remove the mail line below after first make.
#

#Change these for your system: Ex. -DSolaris instead of -DLinux
#CFLAGS=-O -DLinux -Wall
CFLAGS= -DLinux -Wall
RELEASE= 0.1

#FreeBSD make support un comment the next two lines. Yes you need both -D and the gmake
#define. Also check mysqlrad.h for FreeBSD parts and adjust for your system.
#In general "grep -l FreeBSD *" and then check the files to see if you need to change
#anything else.
#CFLAGS= -DLinux -DFreeBSD -Wall 
#FreeBSD=1

LIBS= -L/usr/lib/mysql -lmysqlclient /usr/lib/openisp/libtemplate.a -lz -lcrypt -lm -lssl

all: unxsISP.cgi

unxsISP.cgi: tclient.o tproduct.o tservice.o tinstance.o tjob.o tclientconfig.o tconfig.o tparameter.o tparamtype.o tperiod.o tstatus.o tauthorize.o tserviceglue.o tinvoice.o tinvoicemonth.o tpaid.o tinvoiceitems.o tinvoicestatus.o ttemplate.o tconfiguration.o tpayment.o taccounttype.o tshipping.o tproducttype.o tserver.o tglossary.o tlog.o tlogtype.o tlogmonth.o tmonth.o ttemplateset.o tjobstatus.o ttemplatetype.o tmotd.o  unxsisp.o main.o cgi.o
	cc tclient.o tproduct.o tservice.o tinstance.o tjob.o tclientconfig.o tconfig.o tparameter.o tparamtype.o tperiod.o tstatus.o tauthorize.o tserviceglue.o tinvoice.o tinvoicemonth.o tpaid.o tinvoiceitems.o tinvoicestatus.o ttemplate.o tconfiguration.o tpayment.o taccounttype.o tshipping.o tproducttype.o tserver.o tglossary.o tlog.o tlogtype.o tlogmonth.o tmonth.o ttemplateset.o tjobstatus.o ttemplatetype.o tmotd.o  unxsisp.o main.o cgi.o -o unxsISP.cgi $(LIBS) 
	#mail -s "unxsISP$(RELEASE) `uname -n -s`" support@openisp.net < /dev/null > /dev/null 2>&1

tclient.o: tclient.c mysqlrad.h language.h tclientfunc.h local.h
	cc -c tclient.c -o tclient.o $(CFLAGS)

tproduct.o: tproduct.c mysqlrad.h language.h tproductfunc.h local.h
	cc -c tproduct.c -o tproduct.o $(CFLAGS)

tservice.o: tservice.c mysqlrad.h language.h tservicefunc.h local.h
	cc -c tservice.c -o tservice.o $(CFLAGS)

tinstance.o: tinstance.c mysqlrad.h language.h tinstancefunc.h local.h
	cc -c tinstance.c -o tinstance.o $(CFLAGS)

tjob.o: tjob.c mysqlrad.h language.h tjobfunc.h local.h
	cc -c tjob.c -o tjob.o $(CFLAGS)

tclientconfig.o: tclientconfig.c mysqlrad.h language.h tclientconfigfunc.h local.h
	cc -c tclientconfig.c -o tclientconfig.o $(CFLAGS)

tconfig.o: tconfig.c mysqlrad.h language.h tconfigfunc.h local.h
	cc -c tconfig.c -o tconfig.o $(CFLAGS)

tparameter.o: tparameter.c mysqlrad.h language.h tparameterfunc.h local.h
	cc -c tparameter.c -o tparameter.o $(CFLAGS)

tparamtype.o: tparamtype.c mysqlrad.h language.h tparamtypefunc.h local.h
	cc -c tparamtype.c -o tparamtype.o $(CFLAGS)

tperiod.o: tperiod.c mysqlrad.h language.h tperiodfunc.h local.h
	cc -c tperiod.c -o tperiod.o $(CFLAGS)

tstatus.o: tstatus.c mysqlrad.h language.h tstatusfunc.h local.h
	cc -c tstatus.c -o tstatus.o $(CFLAGS)

tauthorize.o: tauthorize.c mysqlrad.h language.h tauthorizefunc.h local.h
	cc -c tauthorize.c -o tauthorize.o $(CFLAGS)

tserviceglue.o: tserviceglue.c mysqlrad.h language.h tservicegluefunc.h local.h
	cc -c tserviceglue.c -o tserviceglue.o $(CFLAGS)

tinvoice.o: tinvoice.c mysqlrad.h language.h tinvoicefunc.h local.h
	cc -c tinvoice.c -o tinvoice.o $(CFLAGS)

tinvoicemonth.o: tinvoicemonth.c mysqlrad.h language.h tinvoicemonthfunc.h local.h
	cc -c tinvoicemonth.c -o tinvoicemonth.o $(CFLAGS)

tpaid.o: tpaid.c mysqlrad.h language.h tpaidfunc.h local.h
	cc -c tpaid.c -o tpaid.o $(CFLAGS)

tinvoiceitems.o: tinvoiceitems.c mysqlrad.h language.h tinvoiceitemsfunc.h local.h
	cc -c tinvoiceitems.c -o tinvoiceitems.o $(CFLAGS)

tinvoicestatus.o: tinvoicestatus.c mysqlrad.h language.h tinvoicestatusfunc.h local.h
	cc -c tinvoicestatus.c -o tinvoicestatus.o $(CFLAGS)

ttemplate.o: ttemplate.c mysqlrad.h language.h ttemplatefunc.h local.h
	cc -c ttemplate.c -o ttemplate.o $(CFLAGS)

tconfiguration.o: tconfiguration.c mysqlrad.h language.h tconfigurationfunc.h local.h
	cc -c tconfiguration.c -o tconfiguration.o $(CFLAGS)

tpayment.o: tpayment.c mysqlrad.h language.h tpaymentfunc.h local.h
	cc -c tpayment.c -o tpayment.o $(CFLAGS)

taccounttype.o: taccounttype.c mysqlrad.h language.h taccounttypefunc.h local.h
	cc -c taccounttype.c -o taccounttype.o $(CFLAGS)

tshipping.o: tshipping.c mysqlrad.h language.h tshippingfunc.h local.h
	cc -c tshipping.c -o tshipping.o $(CFLAGS)

tproducttype.o: tproducttype.c mysqlrad.h language.h tproducttypefunc.h local.h
	cc -c tproducttype.c -o tproducttype.o $(CFLAGS)

tserver.o: tserver.c mysqlrad.h language.h tserverfunc.h local.h
	cc -c tserver.c -o tserver.o $(CFLAGS)

tglossary.o: tglossary.c mysqlrad.h language.h tglossaryfunc.h local.h
	cc -c tglossary.c -o tglossary.o $(CFLAGS)

tlog.o: tlog.c mysqlrad.h language.h tlogfunc.h local.h
	cc -c tlog.c -o tlog.o $(CFLAGS)

tlogtype.o: tlogtype.c mysqlrad.h language.h tlogtypefunc.h local.h
	cc -c tlogtype.c -o tlogtype.o $(CFLAGS)

tlogmonth.o: tlogmonth.c mysqlrad.h language.h tlogmonthfunc.h local.h
	cc -c tlogmonth.c -o tlogmonth.o $(CFLAGS)

tmonth.o: tmonth.c mysqlrad.h language.h tmonthfunc.h local.h
	cc -c tmonth.c -o tmonth.o $(CFLAGS)

ttemplateset.o: ttemplateset.c mysqlrad.h language.h ttemplatesetfunc.h local.h
	cc -c ttemplateset.c -o ttemplateset.o $(CFLAGS)

tjobstatus.o: tjobstatus.c mysqlrad.h language.h tjobstatusfunc.h local.h
	cc -c tjobstatus.c -o tjobstatus.o $(CFLAGS)

ttemplatetype.o: ttemplatetype.c mysqlrad.h language.h ttemplatetypefunc.h local.h
	cc -c ttemplatetype.c -o ttemplatetype.o $(CFLAGS)

tmotd.o: tmotd.c mysqlrad.h language.h tmotdfunc.h local.h
	cc -c tmotd.c -o tmotd.o $(CFLAGS)


main.o: main.c mysqlrad.h mainfunc.h language.h local.h
	cc -c main.c -o main.o $(CFLAGS)

cgi.o: cgi.h cgi.c
	cc -c cgi.c -o cgi.o $(CFLAGS)

#List: unxsisp.c.o
unxsisp.o: unxsisp.c mysqlrad.h
	cc -c unxsisp.c -o unxsisp.o $(CFLAGS)

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o

cleandist:
	rm -f *.o *.gz

install: unxsISP.cgi
	install -s unxsISP.cgi /var/www/unxs/cgi-bin/unxsISP.cgi
	rm unxsISP.cgi

distribution:
	cd ..;  tar czvf unxsISP/unxsISP$(RELEASE).tar.gz --exclude .svn\
		unxsISP/*.h\
		unxsISP/*.c\
		unxsISP/data\
		unxsISP/docs\
		unxsISP/makefile\
		unxsISP/CHANGES\
		unxsISP/LICENSE\
		unxsISP/INSTALL
