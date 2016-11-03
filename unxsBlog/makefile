#
# svn ID removed
#
# (C) 2001-2004 Gary Wallis.
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

LIBS= -L/usr/lib/mysql -lmysqlclient -lz -lcrypt -lm -lssl



all: unxsBlog.cgi

unxsBlog.cgi: tcontent.o tcontenttype.o tclient.o tauthorize.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tlogmonth.o tmonth.o tglossary.o tjob.o tserver.o tjobstatus.o tstatus.o tconfiguration.o   main.o cgi.o
	cc tcontent.o tcontenttype.o tclient.o tauthorize.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tlogmonth.o tmonth.o tglossary.o tjob.o tserver.o tjobstatus.o tstatus.o tconfiguration.o   main.o cgi.o -o unxsBlog.cgi $(LIBS) 
	#mail -s "unxsBlog$(RELEASE) `uname -n -s`" support@openisp.net < /dev/null > /dev/null 2>&1

tcontent.o: tcontent.c mysqlrad.h language.h tcontentfunc.h local.h
	cc -c tcontent.c -o tcontent.o $(CFLAGS)

tcontenttype.o: tcontenttype.c mysqlrad.h language.h tcontenttypefunc.h local.h
	cc -c tcontenttype.c -o tcontenttype.o $(CFLAGS)

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

tserver.o: tserver.c mysqlrad.h language.h tserverfunc.h local.h
	cc -c tserver.c -o tserver.o $(CFLAGS)

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



clean:
	rm -f *.o

cleandist:
	rm -f *.o *.gz

install: unxsBlog.cgi
	install -s unxsBlog.cgi /var/www/unxs/cgi-bin/unxsBlog.cgi
	rm unxsBlog.cgi

distribution:
	cd ..;  tar czvf unxsBlog/unxsBlog$(RELEASE).tar.gz --exclude .svn\
		unxsBlog/*.h\
		unxsBlog/*.c\
		unxsBlog/data\
		unxsBlog/docs\
		unxsBlog/makefile\
		unxsBlog/CHANGES\
		unxsBlog/LICENSE\
		unxsBlog/INSTALL
