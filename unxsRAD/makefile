#
#FILE
#       unxsRAD/makefile
#       svn ID removed
#AUTHOR/LEGAL
#       (C) 2001-2012 Gary Wallis for Unixservice. GPLv2 license applies.
#
#NOTES
#       We only develop and test on Linux CentOS-5 and distribute via yum and rpm.
#       Feel free to repackage for your OS and let us know.
#

#Change these for your system: Ex. -DSolaris instead of -DLinux
#CFLAGS=-O -DLinux -Wall -pedantic
GIT_VERSION := $(shell git describe --dirty --always --tags)
CFLAGS= -DLinux -Wall -DdsGitVersion=\"$(GIT_VERSION)\"
RELEASE= 0.1
LIBS= -L/usr/lib/openisp -L/usr/lib64/mysql -L/usr/lib/mysql -lz -lcrypt -lm -lssl -ltemplate -lmysqlclient
CGIDIR=/var/www/cgi-bin/
IMGDIR=/var/www/html/pami/images/


all: unxsRAD.cgi unxsRAD

#New standalone CLI job queue model
unxsRAD: unxsrad.o mysqlconnect.o
	cc unxsrad.o mysqlconnect.o -o unxsRAD $(LIBS)

unxsrad.o: unxsrad.h mysqlrad.h local.h
	cc -c unxsrad.c -o unxsrad.o $(CFLAGS)

unxsRAD.cgi: tproject.o ttable.o tfield.o ttemplate.o ttemplateset.o ttemplatetype.o tprojectstatus.o tfieldtype.o tindextype.o tclient.o tauthorize.o tstatus.o tlog.o tlogtype.o tlogmonth.o tglossary.o tjob.o tmonth.o tjobstatus.o tconfiguration.o tserver.o mysqlconnect.o  main.o cgi.o
	cc tproject.o ttable.o tfield.o ttemplate.o ttemplateset.o ttemplatetype.o tprojectstatus.o tfieldtype.o tindextype.o tclient.o tauthorize.o tstatus.o tlog.o tlogtype.o tlogmonth.o tglossary.o tjob.o tmonth.o tjobstatus.o tconfiguration.o tserver.o mysqlconnect.o  main.o cgi.o -o unxsRAD.cgi $(LIBS) 

tproject.o: tproject.c mysqlrad.h language.h tprojectfunc.c local.h
	cc -c tproject.c -o tproject.o $(CFLAGS)

ttable.o: ttable.c mysqlrad.h language.h ttablefunc.h local.h
	cc -c ttable.c -o ttable.o $(CFLAGS)

tfield.o: tfield.c mysqlrad.h language.h tfieldfunc.h local.h
	cc -c tfield.c -o tfield.o $(CFLAGS)

ttemplate.o: ttemplate.c mysqlrad.h language.h ttemplatefunc.h local.h
	cc -c ttemplate.c -o ttemplate.o $(CFLAGS)

ttemplateset.o: ttemplateset.c mysqlrad.h language.h ttemplatesetfunc.h local.h
	cc -c ttemplateset.c -o ttemplateset.o $(CFLAGS)

ttemplatetype.o: ttemplatetype.c mysqlrad.h language.h ttemplatetypefunc.h local.h
	cc -c ttemplatetype.c -o ttemplatetype.o $(CFLAGS)

tprojectstatus.o: tprojectstatus.c mysqlrad.h language.h tprojectstatusfunc.h local.h
	cc -c tprojectstatus.c -o tprojectstatus.o $(CFLAGS)

tfieldtype.o: tfieldtype.c mysqlrad.h language.h tfieldtypefunc.h local.h
	cc -c tfieldtype.c -o tfieldtype.o $(CFLAGS)

tindextype.o: tindextype.c mysqlrad.h language.h tindextypefunc.h local.h
	cc -c tindextype.c -o tindextype.o $(CFLAGS)

tclient.o: tclient.c mysqlrad.h language.h tclientfunc.h local.h
	cc -c tclient.c -o tclient.o $(CFLAGS)

tauthorize.o: tauthorize.c mysqlrad.h language.h tauthorizefunc.h local.h
	cc -c tauthorize.c -o tauthorize.o $(CFLAGS)

tstatus.o: tstatus.c mysqlrad.h language.h tstatusfunc.h local.h
	cc -c tstatus.c -o tstatus.o $(CFLAGS)

tlog.o: tlog.c mysqlrad.h language.h tlogfunc.h local.h
	cc -c tlog.c -o tlog.o $(CFLAGS)

tlogtype.o: tlogtype.c mysqlrad.h language.h tlogtypefunc.h local.h
	cc -c tlogtype.c -o tlogtype.o $(CFLAGS)

tlogmonth.o: tlogmonth.c mysqlrad.h language.h tlogmonthfunc.h local.h
	cc -c tlogmonth.c -o tlogmonth.o $(CFLAGS)

tglossary.o: tglossary.c mysqlrad.h language.h tglossaryfunc.h local.h
	cc -c tglossary.c -o tglossary.o $(CFLAGS)

tjob.o: tjob.c mysqlrad.h language.h tjobfunc.h local.h
	cc -c tjob.c -o tjob.o $(CFLAGS)

tmonth.o: tmonth.c mysqlrad.h language.h tmonthfunc.h local.h
	cc -c tmonth.c -o tmonth.o $(CFLAGS)

tjobstatus.o: tjobstatus.c mysqlrad.h language.h tjobstatusfunc.h local.h
	cc -c tjobstatus.c -o tjobstatus.o $(CFLAGS)

tconfiguration.o: tconfiguration.c mysqlrad.h language.h tconfigurationfunc.h local.h
	cc -c tconfiguration.c -o tconfiguration.o $(CFLAGS)

tserver.o: tserver.c mysqlrad.h language.h tserverfunc.h local.h
	cc -c tserver.c -o tserver.o $(CFLAGS)


main.o: main.c mysqlrad.h mainfunc.c language.h local.h
	cc -c main.c -o main.o $(CFLAGS)

cgi.o: cgi.h cgi.c
	cc -c cgi.c -o cgi.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c mysqlrad.h local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

local.h: local.h.default
	@ if [ ! -f local.h ];then cp -i local.h.default local.h; fi

clean:
	rm -f *.o
	rm -f unxsRAD.cgi

install: unxsRAD.cgi unxsRAD
	install -s unxsRAD.cgi $(CGIDIR)unxsRAD.cgi
	install -s unxsRAD /usr/sbin/unxsRAD
	rm -f unxsRAD.cgi
	rm -f unxsRAD

install-img: 
	cp images/*.gif $(IMGDIR)
