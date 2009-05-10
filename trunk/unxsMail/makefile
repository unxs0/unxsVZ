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
	LIBS= /usr/local/lib/mysql/libmysqlclient.a -lz -lm /usr/lib/libcrypt.a /usr/lib/openisp/libtemplate.a
else
	LIBS= /usr/lib/mysql/libmysqlclient.a -lz -lcrypt -lm -lssl /usr/lib/openisp/libtemplate.a
endif



all: mysqlMail2.cgi

mysqlMail2.cgi: tuser.o talias.o tdomain.o tvut.o tvutentries.o taccess.o trelay.o tlocal.o tusertype.o tuserconfig.o tserverconfig.o tconfigspec.o tclient.o tauthorize.o tstatus.o tjobstatus.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tlogmonth.o tmonth.o tglossary.o tjob.o tserver.o tservergroup.o tservergroupglue.o tconfiguration.o tholduser.o trequest.o twhitelist.o tblacklist.o tvacation.o mail.o  main.o cgi.o
	cc tuser.o talias.o tdomain.o tvut.o tvutentries.o taccess.o trelay.o tlocal.o tusertype.o tuserconfig.o tserverconfig.o tconfigspec.o tclient.o tauthorize.o tstatus.o tjobstatus.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tlogmonth.o tmonth.o tglossary.o tjob.o tserver.o tservergroup.o tservergroupglue.o tconfiguration.o tholduser.o trequest.o twhitelist.o tblacklist.o tvacation.o mail.o  main.o cgi.o -o mysqlMail2.cgi $(LIBS) 
	#mail -s "mysqlMail2$(RELEASE) `uname -n -s`" support@openisp.net < /dev/null > /dev/null 2>&1

tuser.o: tuser.c mysqlrad.h language.h tuserfunc.h local.h
	cc -c tuser.c -o tuser.o $(CFLAGS)

talias.o: talias.c mysqlrad.h language.h taliasfunc.h local.h
	cc -c talias.c -o talias.o $(CFLAGS)

tdomain.o: tdomain.c mysqlrad.h language.h tdomainfunc.h local.h
	cc -c tdomain.c -o tdomain.o $(CFLAGS)

tvut.o: tvut.c mysqlrad.h language.h tvutfunc.h local.h
	cc -c tvut.c -o tvut.o $(CFLAGS)

tvutentries.o: tvutentries.c mysqlrad.h language.h tvutentriesfunc.h local.h
	cc -c tvutentries.c -o tvutentries.o $(CFLAGS)

taccess.o: taccess.c mysqlrad.h language.h taccessfunc.h local.h
	cc -c taccess.c -o taccess.o $(CFLAGS)

trelay.o: trelay.c mysqlrad.h language.h trelayfunc.h local.h
	cc -c trelay.c -o trelay.o $(CFLAGS)

tlocal.o: tlocal.c mysqlrad.h language.h tlocalfunc.h local.h
	cc -c tlocal.c -o tlocal.o $(CFLAGS)

tusertype.o: tusertype.c mysqlrad.h language.h tusertypefunc.h local.h
	cc -c tusertype.c -o tusertype.o $(CFLAGS)

tuserconfig.o: tuserconfig.c mysqlrad.h language.h tuserconfigfunc.h local.h
	cc -c tuserconfig.c -o tuserconfig.o $(CFLAGS)

tserverconfig.o: tserverconfig.c mysqlrad.h language.h tserverconfigfunc.h local.h
	cc -c tserverconfig.c -o tserverconfig.o $(CFLAGS)

tconfigspec.o: tconfigspec.c mysqlrad.h language.h tconfigspecfunc.h local.h
	cc -c tconfigspec.c -o tconfigspec.o $(CFLAGS)

tclient.o: tclient.c mysqlrad.h language.h tclientfunc.h local.h
	cc -c tclient.c -o tclient.o $(CFLAGS)

tauthorize.o: tauthorize.c mysqlrad.h language.h tauthorizefunc.h local.h
	cc -c tauthorize.c -o tauthorize.o $(CFLAGS)

tstatus.o: tstatus.c mysqlrad.h language.h tstatusfunc.h local.h
	cc -c tstatus.c -o tstatus.o $(CFLAGS)

tjobstatus.o: tjobstatus.c mysqlrad.h language.h tjobstatusfunc.h local.h
	cc -c tjobstatus.c -o tjobstatus.o $(CFLAGS)

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

tservergroup.o: tservergroup.c mysqlrad.h language.h tservergroupfunc.h local.h
	cc -c tservergroup.c -o tservergroup.o $(CFLAGS)

tservergroupglue.o: tservergroupglue.c mysqlrad.h language.h tservergroupgluefunc.h local.h
	cc -c tservergroupglue.c -o tservergroupglue.o $(CFLAGS)

tconfiguration.o: tconfiguration.c mysqlrad.h language.h tconfigurationfunc.h local.h
	cc -c tconfiguration.c -o tconfiguration.o $(CFLAGS)

tholduser.o: tholduser.c mysqlrad.h language.h tholduserfunc.h local.h
	cc -c tholduser.c -o tholduser.o $(CFLAGS)

trequest.o: trequest.c mysqlrad.h language.h trequestfunc.h local.h
	cc -c trequest.c -o trequest.o $(CFLAGS)

twhitelist.o: twhitelist.c mysqlrad.h language.h twhitelistfunc.h local.h
	cc -c twhitelist.c -o twhitelist.o $(CFLAGS)

tblacklist.o: tblacklist.c mysqlrad.h language.h tblacklistfunc.h local.h
	cc -c tblacklist.c -o tblacklist.o $(CFLAGS)

tvacation.o: tvacation.c mysqlrad.h language.h tvacationfunc.h local.h
	cc -c tvacation.c -o tvacation.o $(CFLAGS)

main.o: main.c mysqlrad.h mainfunc.h language.h local.h
	cc -c main.c -o main.o $(CFLAGS)

cgi.o: cgi.h cgi.c
	cc -c cgi.c -o cgi.o $(CFLAGS)

mail.o : mail.c  mysqlrad.h mainfunc.h language.h local.h
	cc -c mail.c -o mail.o $(CFLAGS)
	

clean:
	rm -f *.o

cleandist:
	rm -f *.o *.gz

install: mysqlMail2.cgi
	install -s mysqlMail2.cgi $(CGIDIR)mysqlMail2.cgi
	rm mysqlMail2.cgi

distribution:
	cd ..;  tar czvf mysqlMail2/mysqlMail2$(RELEASE).tar.gz --exclude .svn\
		mysqlMail2/*.h\
		mysqlMail2/*.c\
		mysqlMail2/data\
		mysqlMail2/docs\
		mysqlMail2/makefile\
		mysqlMail2/CHANGES\
		mysqlMail2/LICENSE\
		mysqlMail2/INSTALL
