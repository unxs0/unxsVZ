#
#FILE
#       {{cProject}}/makefile
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
LIBS= -L/usr/lib/openisp /usr/lib/mysql/libmysqlclient.a -lz -lcrypt -lm -lssl -ltemplate
CGIDIR=/var/www/unxs/cgi-bin/

all: {{cProject}}.cgi {{cProject}}

#New standalone CLI job queue model
{{cProject}}: {{cProjectLC}}.o mysqlconnect.o
	cc {{cProjectLC}}.o mysqlconnect.o -o {{cProject}} $(LIBS)

{{cProjectLC}}.o: {{cProjectLC}}.c {{cProjectLC}}.h mysqlrad.h local.h mysqlconnect.c
	cc -c {{cProjectLC}}.c -o {{cProjectLC}}.o $(CFLAGS)

#included standard RAD4 objects
{{cProject}}.cgi: {{funcMakefileObjects}} ttemplate.o ttemplateset.o ttemplatetype.o tclient.o tauthorize.o tstatus.o tlog.o tlogtype.o tlogmonth.o tglossary.o tjob.o tmonth.o tjobstatus.o tconfiguration.o tserver.o  main.o cgi.o
	cc {{funcMakefileObjects}} ttemplate.o ttemplateset.o ttemplatetype.o tclient.o tauthorize.o tstatus.o tlog.o tlogtype.o tlogmonth.o tglossary.o tjob.o tmonth.o tjobstatus.o tconfiguration.o tserver.o main.o cgi.o -o {{cProject}}.cgi $(LIBS) 

{{funcMakefileRules}}

#standard RAD4 tables
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

install: {{cProject}}.cgi {{cProject}}
	install -s {{cProject}}.cgi $(CGIDIR){{cProject}}.cgi
	install -s {{cProject}} /usr/sbin/{{cProject}}
	rm {{cProject}}.cgi
	rm {{cProject}}

distribution:
	cd ..;  tar czvf {{cProject}}/{{cProject}}$(RELEASE).tar.gz --exclude .svn\
		{{cProject}}/*.h\
		{{cProject}}/*.c\
		{{cProject}}/data\
		{{cProject}}/docs\
		{{cProject}}/makefile\
		{{cProject}}/CHANGES\
		{{cProject}}/LICENSE\
		{{cProject}}/INSTALL
