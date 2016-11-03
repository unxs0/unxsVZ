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

LIBS= -L/usr/lib/mysql -lmysqlclient -lz -lm -lcrypt /usr/lib/openisp/libtemplate.a



all: unxsApache.cgi

unxsApache.cgi: tsite.o tsiteuser.o tmysql.o tmysqluser.o tip.o tsslcert.o tclient.o ttemplate.o ttemplateset.o ttemplatetype.o tjob.o tlog.o tlogtype.o tlogmonth.o tmonth.o tconfiguration.o tserver.o tstatus.o tjobstatus.o tauthorize.o tglossary.o  apache.o main.o cgi.o
	cc tsite.o tsiteuser.o tmysql.o tmysqluser.o tip.o tsslcert.o tclient.o ttemplate.o ttemplateset.o ttemplatetype.o tjob.o tlog.o tlogtype.o tlogmonth.o tmonth.o tconfiguration.o tserver.o tstatus.o tjobstatus.o tauthorize.o tglossary.o  apache.o main.o cgi.o -o unxsApache.cgi $(LIBS) 
	#mail -s "unxsApache$(RELEASE) `uname -n -s`" support@openisp.net < /dev/null > /dev/null 2>&1

tsite.o: tsite.c mysqlrad.h language.h tsitefunc.h local.h
	cc -c tsite.c -o tsite.o $(CFLAGS)

tsiteuser.o: tsiteuser.c mysqlrad.h language.h tsiteuserfunc.h local.h
	cc -c tsiteuser.c -o tsiteuser.o $(CFLAGS)

tmysql.o: tmysql.c mysqlrad.h language.h tmysqlfunc.h local.h
	cc -c tmysql.c -o tmysql.o $(CFLAGS)

tmysqluser.o: tmysqluser.c mysqlrad.h language.h tmysqluserfunc.h local.h
	cc -c tmysqluser.c -o tmysqluser.o $(CFLAGS)

tip.o: tip.c mysqlrad.h language.h tipfunc.h local.h
	cc -c tip.c -o tip.o $(CFLAGS)

tsslcert.o: tsslcert.c mysqlrad.h language.h tsslcertfunc.h local.h
	cc -c tsslcert.c -o tsslcert.o $(CFLAGS)

tclient.o: tclient.c mysqlrad.h language.h tclientfunc.h local.h
	cc -c tclient.c -o tclient.o $(CFLAGS)

ttemplate.o: ttemplate.c mysqlrad.h language.h ttemplatefunc.h local.h
	cc -c ttemplate.c -o ttemplate.o $(CFLAGS)

ttemplateset.o: ttemplateset.c mysqlrad.h language.h ttemplatesetfunc.h local.h
	cc -c ttemplateset.c -o ttemplateset.o $(CFLAGS)

ttemplatetype.o: ttemplatetype.c mysqlrad.h language.h ttemplatetypefunc.h local.h
	cc -c ttemplatetype.c -o ttemplatetype.o $(CFLAGS)

tjob.o: tjob.c mysqlrad.h language.h tjobfunc.h local.h
	cc -c tjob.c -o tjob.o $(CFLAGS)

tlog.o: tlog.c mysqlrad.h language.h tlogfunc.h local.h
	cc -c tlog.c -o tlog.o $(CFLAGS)

tlogtype.o: tlogtype.c mysqlrad.h language.h tlogtypefunc.h local.h
	cc -c tlogtype.c -o tlogtype.o $(CFLAGS)

tlogmonth.o: tlogmonth.c mysqlrad.h language.h tlogmonthfunc.h local.h
	cc -c tlogmonth.c -o tlogmonth.o $(CFLAGS)

tmonth.o: tmonth.c mysqlrad.h language.h tmonthfunc.h local.h
	cc -c tmonth.c -o tmonth.o $(CFLAGS)

tconfiguration.o: tconfiguration.c mysqlrad.h language.h tconfigurationfunc.h local.h
	cc -c tconfiguration.c -o tconfiguration.o $(CFLAGS)

tserver.o: tserver.c mysqlrad.h language.h tserverfunc.h local.h
	cc -c tserver.c -o tserver.o $(CFLAGS)

tstatus.o: tstatus.c mysqlrad.h language.h tstatusfunc.h local.h
	cc -c tstatus.c -o tstatus.o $(CFLAGS)

tjobstatus.o: tjobstatus.c mysqlrad.h language.h tjobstatusfunc.h local.h
	cc -c tjobstatus.c -o tjobstatus.o $(CFLAGS)

tauthorize.o: tauthorize.c mysqlrad.h language.h tauthorizefunc.h local.h
	cc -c tauthorize.c -o tauthorize.o $(CFLAGS)

tglossary.o: tglossary.c mysqlrad.h language.h tglossaryfunc.h local.h
	cc -c tglossary.c -o tglossary.o $(CFLAGS)


main.o: main.c mysqlrad.h mainfunc.h language.h local.h
	cc -c main.c -o main.o $(CFLAGS)

cgi.o: cgi.h cgi.c
	cc -c cgi.c -o cgi.o $(CFLAGS)

#List: apache.o
apache.o: apache.c mysqlrad.h local.h
	cc -c apache.c -o apache.o $(CFLAGS)

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o

cleandist:
	rm -f *.o *.gz

install: unxsApache.cgi
	install -s unxsApache.cgi /var/www/unxs/cgi-bin/unxsApache.cgi
	rm unxsApache.cgi

distribution:
	cd ..;  tar czvf unxsApache/unxsApache$(RELEASE).tar.gz --exclude .svn\
		unxsApache/*.h\
		unxsApache/*.c\
		unxsApache/data\
		unxsApache/docs\
		unxsApache/makefile\
		unxsApache/CHANGES\
		unxsApache/LICENSE\
		unxsApache/INSTALL
