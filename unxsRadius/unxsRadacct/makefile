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

LIBS= -L/usr/lib/mysql/ -lmysqlclient -lz -lcrypt -lm -lssl



all: unxsRadacct.cgi

unxsRadacct.cgi: tradacct.o tmonth.o tradacctmonth.o tconfiguration.o tlog.o tlogtype.o tlogmonth.o tglossary.o tserver.o tmonthlog.o tauthorize.o tclient.o   main.o cgi.o
	cc tradacct.o tmonth.o tradacctmonth.o tconfiguration.o tlog.o tlogtype.o tlogmonth.o tglossary.o tserver.o tmonthlog.o tauthorize.o tclient.o   main.o cgi.o -o unxsRadacct.cgi $(LIBS) 
	#mail -s "unxsRadacct$(RELEASE) `uname -n -s`" support@openisp.net < /dev/null > /dev/null 2>&1

tradacct.o: tradacct.c mysqlrad.h language.h tradacctfunc.h local.h
	cc -c tradacct.c -o tradacct.o $(CFLAGS)

tmonth.o: tmonth.c mysqlrad.h language.h tmonthfunc.h local.h
	cc -c tmonth.c -o tmonth.o $(CFLAGS)

tradacctmonth.o: tradacctmonth.c mysqlrad.h language.h tradacctmonthfunc.h local.h
	cc -c tradacctmonth.c -o tradacctmonth.o $(CFLAGS)

tconfiguration.o: tconfiguration.c mysqlrad.h language.h tconfigurationfunc.h local.h
	cc -c tconfiguration.c -o tconfiguration.o $(CFLAGS)

tlog.o: tlog.c mysqlrad.h language.h tlogfunc.h local.h
	cc -c tlog.c -o tlog.o $(CFLAGS)

tlogtype.o: tlogtype.c mysqlrad.h language.h tlogtypefunc.h local.h
	cc -c tlogtype.c -o tlogtype.o $(CFLAGS)

tlogmonth.o: tlogmonth.c mysqlrad.h language.h tlogmonthfunc.h local.h
	cc -c tlogmonth.c -o tlogmonth.o $(CFLAGS)

tglossary.o: tglossary.c mysqlrad.h language.h tglossaryfunc.h local.h
	cc -c tglossary.c -o tglossary.o $(CFLAGS)

tserver.o: tserver.c mysqlrad.h language.h tserverfunc.h local.h
	cc -c tserver.c -o tserver.o $(CFLAGS)

tmonthlog.o: tmonthlog.c mysqlrad.h language.h tmonthlogfunc.h local.h
	cc -c tmonthlog.c -o tmonthlog.o $(CFLAGS)

tauthorize.o: tauthorize.c mysqlrad.h language.h tauthorizefunc.h local.h
	cc -c tauthorize.c -o tauthorize.o $(CFLAGS)

tclient.o: tclient.c mysqlrad.h language.h tclientfunc.h local.h
	cc -c tclient.c -o tclient.o $(CFLAGS)


main.o: main.c mysqlrad.h mainfunc.h language.h local.h
	cc -c main.c -o main.o $(CFLAGS)

cgi.o: cgi.h cgi.c
	cc -c cgi.c -o cgi.o $(CFLAGS)

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi


clean:
	rm -f *.o

cleandist:
	rm -f *.o *.gz

install: unxsRadacct.cgi
	install -s unxsRadacct.cgi /var/www/unxs/cgi-bin/unxsRadacct.cgi
	rm unxsRadacct.cgi

distribution:
	cd ..;  tar czvf unxsRadacct/unxsRadacct$(RELEASE).tar.gz --exclude .svn\
		unxsRadacct/*.h\
		unxsRadacct/*.c\
		unxsRadacct/data\
		unxsRadacct/docs\
		unxsRadacct/makefile\
		unxsRadacct/CHANGES\
		unxsRadacct/LICENSE\
		unxsRadacct/INSTALL
