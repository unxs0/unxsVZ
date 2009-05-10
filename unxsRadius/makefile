#
# $Id: makefile.template 693 2006-07-27 22:58:07Z ggw $
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

ifdef FreeBSD
	LIBS= /usr/local/lib/mysql/libmysqlclient.a -lz -lm /usr/lib/libcrypt.a
else
	LIBS= /usr/lib/mysql/libmysqlclient.a -lz -lcrypt -lm -lssl
endif



all: mysqlRadius2.cgi

mysqlRadius2.cgi: tuser.o tprofile.o tprofilename.o tserver.o tnas.o tjob.o tjobstatus.o tconfiguration.o tservergroup.o tnasgroup.o tlog.o tlogtype.o tmonth.o tlogmonth.o tglossary.o ttemplate.o ttemplateset.o ttemplatetype.o tclient.o tauthorize.o  radius.o main.o cgi.o
	cc tuser.o tprofile.o tprofilename.o tserver.o tnas.o tjob.o tjobstatus.o tconfiguration.o tservergroup.o tnasgroup.o tlog.o tlogtype.o tmonth.o tlogmonth.o tglossary.o ttemplate.o ttemplateset.o ttemplatetype.o tclient.o tauthorize.o  radius.o main.o cgi.o -o mysqlRadius2.cgi $(LIBS) 
	#mail -s "mysqlRadius2$(RELEASE) `uname -n -s`" support@openisp.net < /dev/null > /dev/null 2>&1

tuser.o: tuser.c mysqlrad.h language.h tuserfunc.h local.h
	cc -c tuser.c -o tuser.o $(CFLAGS)

tprofile.o: tprofile.c mysqlrad.h language.h tprofilefunc.h local.h
	cc -c tprofile.c -o tprofile.o $(CFLAGS)

tprofilename.o: tprofilename.c mysqlrad.h language.h tprofilenamefunc.h local.h
	cc -c tprofilename.c -o tprofilename.o $(CFLAGS)

tserver.o: tserver.c mysqlrad.h language.h tserverfunc.h local.h
	cc -c tserver.c -o tserver.o $(CFLAGS)

tnas.o: tnas.c mysqlrad.h language.h tnasfunc.h local.h
	cc -c tnas.c -o tnas.o $(CFLAGS)

tjob.o: tjob.c mysqlrad.h language.h tjobfunc.h local.h
	cc -c tjob.c -o tjob.o $(CFLAGS)

tjobstatus.o: tjobstatus.c mysqlrad.h language.h tjobstatusfunc.h local.h
	cc -c tjobstatus.c -o tjobstatus.o $(CFLAGS)

tconfiguration.o: tconfiguration.c mysqlrad.h language.h tconfigurationfunc.h local.h
	cc -c tconfiguration.c -o tconfiguration.o $(CFLAGS)

tservergroup.o: tservergroup.c mysqlrad.h language.h tservergroupfunc.h local.h
	cc -c tservergroup.c -o tservergroup.o $(CFLAGS)

tnasgroup.o: tnasgroup.c mysqlrad.h language.h tnasgroupfunc.h local.h
	cc -c tnasgroup.c -o tnasgroup.o $(CFLAGS)

tlog.o: tlog.c mysqlrad.h language.h tlogfunc.h local.h
	cc -c tlog.c -o tlog.o $(CFLAGS)

tlogtype.o: tlogtype.c mysqlrad.h language.h tlogtypefunc.h local.h
	cc -c tlogtype.c -o tlogtype.o $(CFLAGS)

tmonth.o: tmonth.c mysqlrad.h language.h tmonthfunc.h local.h
	cc -c tmonth.c -o tmonth.o $(CFLAGS)

tlogmonth.o: tlogmonth.c mysqlrad.h language.h tlogmonthfunc.h local.h
	cc -c tlogmonth.c -o tlogmonth.o $(CFLAGS)

tglossary.o: tglossary.c mysqlrad.h language.h tglossaryfunc.h local.h
	cc -c tglossary.c -o tglossary.o $(CFLAGS)

ttemplate.o: ttemplate.c mysqlrad.h language.h ttemplatefunc.h local.h
	cc -c ttemplate.c -o ttemplate.o $(CFLAGS)

ttemplateset.o: ttemplateset.c mysqlrad.h language.h ttemplatesetfunc.h local.h
	cc -c ttemplateset.c -o ttemplateset.o $(CFLAGS)

ttemplatetype.o: ttemplatetype.c mysqlrad.h language.h ttemplatetypefunc.h local.h
	cc -c ttemplatetype.c -o ttemplatetype.o $(CFLAGS)

tclient.o: tclient.c mysqlrad.h language.h tclientfunc.h local.h
	cc -c tclient.c -o tclient.o $(CFLAGS)

tauthorize.o: tauthorize.c mysqlrad.h language.h tauthorizefunc.h local.h
	cc -c tauthorize.c -o tauthorize.o $(CFLAGS)


main.o: main.c mysqlrad.h mainfunc.h language.h local.h
	cc -c main.c -o main.o $(CFLAGS)

cgi.o: cgi.h cgi.c
	cc -c cgi.c -o cgi.o $(CFLAGS)

#List: radius.o
radius.o: radius.c mysqlrad.h
	cc -c radius.c -o radius.o $(CFLAGS)

clean:
	rm -f *.o

cleandist:
	rm -f *.o *.gz

install: mysqlRadius2.cgi
	install -s mysqlRadius2.cgi $(CGIDIR)mysqlRadius2.cgi
	rm mysqlRadius2.cgi

distribution:
	cd ..;  tar czvf mysqlRadius2/mysqlRadius2$(RELEASE).tar.gz --exclude .svn\
		mysqlRadius2/*.h\
		mysqlRadius2/*.c\
		mysqlRadius2/data\
		mysqlRadius2/docs\
		mysqlRadius2/makefile\
		mysqlRadius2/CHANGES\
		mysqlRadius2/LICENSE\
		mysqlRadius2/INSTALL
