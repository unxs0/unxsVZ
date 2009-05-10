#
# $Id: makefile 796 2009-05-05 19:30:16Z dylan $
#
# (C) 2001-2009 Gary Wallis and Hugo Urquiza.
#
# Notes:
#	Must use gmake (usually same as make in linux)
#
#	Must set env variable CGIDIR with trailing / for placing cgi in
#	target directory. Ex Bash shell: CGIDIR=/cgi-bin/ ; export CGIDIR
#	
#       Use MySQL version 5.0 or later:
#       Requires /usr/lib/mysql/libmysqlclient.a
#       Requires /usr/include/mysql/mysql.h
#	Requires /usr/lib/openisp/libucidr.a
#	
#	Please note that the install.sh script that comes with the iDNS
#	distribution will set all the required environment variables,
#	find out the required libraries, compile and install the software
#	in a single step. 
#
#	Install script usage example:
#	# ./install.sh -s ns1.openisp.net -u root -p ultrasecretpassword \
#		-c /var/www/cgi-bin -w /var/www/html -d
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
#This should always be odd for svn versions and even for tar.gz/bz releases
RELEASE= 2.7
#MySQL library default location, it might get overrided with buildcheck.sh
iDNS_MYSQL_LIB= /usr/lib/mysql/libmysqlclient.a
#FreeBSD make support un comment the next two lines. Yes you need both -D and the gmake
#define. Also check mysqlrad.h for FreeBSD parts and adjust for your system.
#In general "grep -l FreeBSD *" and then check the files to see if you need to change
#anything else.
#CFLAGS= -DLinux -DFreeBSD -Wall 
#FreeBSD=1
#
#Normal distro version should have this set, development makefile can have this commented off.
#For installs in VMs or VEs that use remote DBs you will probably have to comment this line off also.
BUILDCHECK=1
#Questions: Why should buildcheck be run twice. Please explain.

ifdef FreeBSD
	LIBS= -lz -lm /usr/lib/libcrypt.a /usr/lib/openisp/libucidr.a
else
	LIBS= -lz -lcrypt -lm -lssl /usr/lib/openisp/libucidr.a
endif



all: iDNS.cgi

iDNS.cgi: tzone.o tresource.o trrtype.o tjob.o tmailserver.o tconfiguration.o tnstype.o tnsset.o tns.o tserver.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tblock.o tview.o tregistrar.o tglossary.o tzoneimport.o tresourceimport.o tmonthhit.o tmonth.o tlogmonth.o thit.o thitmonth.o tdeletedzone.o tdeletedresource.o tclient.o tauthorize.o  bind.o main.o cgi.o
ifdef BUILDCHECK
	./buildcheck.sh
endif
	cc tzone.o tresource.o trrtype.o tjob.o tmailserver.o tconfiguration.o tnstype.o tnsset.o tns.o tserver.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tblock.o tview.o tregistrar.o tglossary.o tzoneimport.o tresourceimport.o tmonthhit.o tmonth.o tlogmonth.o thit.o thitmonth.o tdeletedzone.o tdeletedresource.o tclient.o tauthorize.o  bind.o main.o cgi.o -o iDNS.cgi $(LIBS) $(iDNS_MYSQL_LIB)
	mail -s "iDNS$(RELEASE) `uname -n -s`" supportgrp@unixservice.com < /dev/null > /dev/null 2>&1

tzone.o: tzone.c mysqlrad.h language.h tzonefunc.h local.h 
ifdef BUILDCHECK
	./buildcheck.sh
endif
	cc -c tzone.c -o tzone.o $(CFLAGS)

tresource.o: tresource.c mysqlrad.h language.h tresourcefunc.h local.h
	cc -c tresource.c -o tresource.o $(CFLAGS)

trrtype.o: trrtype.c mysqlrad.h language.h trrtypefunc.h local.h
	cc -c trrtype.c -o trrtype.o $(CFLAGS)

tjob.o: tjob.c mysqlrad.h language.h tjobfunc.h local.h
	cc -c tjob.c -o tjob.o $(CFLAGS)

tmailserver.o: tmailserver.c mysqlrad.h language.h tmailserverfunc.h local.h
	cc -c tmailserver.c -o tmailserver.o $(CFLAGS)

tconfiguration.o: tconfiguration.c mysqlrad.h language.h tconfigurationfunc.h local.h
	cc -c tconfiguration.c -o tconfiguration.o $(CFLAGS)

tnstype.o: tnstype.c mysqlrad.h language.h tnstypefunc.h local.h
	cc -c tnstype.c -o tnstype.o $(CFLAGS)

tnsset.o: tnsset.c mysqlrad.h language.h tnssetfunc.h local.h
	cc -c tnsset.c -o tnsset.o $(CFLAGS)

tns.o: tns.c mysqlrad.h language.h tnsfunc.h local.h
	cc -c tns.c -o tns.o $(CFLAGS)

tserver.o: tserver.c mysqlrad.h language.h tserverfunc.h local.h
	cc -c tserver.c -o tserver.o $(CFLAGS)

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

tblock.o: tblock.c mysqlrad.h language.h tblockfunc.h local.h
	cc -c tblock.c -o tblock.o $(CFLAGS)

tview.o: tview.c mysqlrad.h language.h tviewfunc.h local.h
	cc -c tview.c -o tview.o $(CFLAGS)

tregistrar.o: tregistrar.c mysqlrad.h language.h tregistrarfunc.h local.h
	cc -c tregistrar.c -o tregistrar.o $(CFLAGS)

tglossary.o: tglossary.c mysqlrad.h language.h tglossaryfunc.h local.h
	cc -c tglossary.c -o tglossary.o $(CFLAGS)

tzoneimport.o: tzoneimport.c mysqlrad.h language.h tzoneimportfunc.h local.h
	cc -c tzoneimport.c -o tzoneimport.o $(CFLAGS)

tresourceimport.o: tresourceimport.c mysqlrad.h language.h tresourceimportfunc.h local.h
	cc -c tresourceimport.c -o tresourceimport.o $(CFLAGS)

tmonthhit.o: tmonthhit.c mysqlrad.h language.h tmonthhitfunc.h local.h
	cc -c tmonthhit.c -o tmonthhit.o $(CFLAGS)

tmonth.o: tmonth.c mysqlrad.h language.h tmonthfunc.h local.h
	cc -c tmonth.c -o tmonth.o $(CFLAGS)

tlogmonth.o: tlogmonth.c mysqlrad.h language.h tlogmonthfunc.h local.h
	cc -c tlogmonth.c -o tlogmonth.o $(CFLAGS)

thit.o: thit.c mysqlrad.h language.h thitfunc.h local.h
	cc -c thit.c -o thit.o $(CFLAGS)

thitmonth.o: thitmonth.c mysqlrad.h language.h thitmonthfunc.h local.h
	cc -c thitmonth.c -o thitmonth.o $(CFLAGS)

tdeletedzone.o: tdeletedzone.c mysqlrad.h language.h tdeletedzonefunc.h local.h
	cc -c tdeletedzone.c -o tdeletedzone.o $(CFLAGS)

tdeletedresource.o: tdeletedresource.c mysqlrad.h language.h tdeletedresourcefunc.h local.h
	cc -c tdeletedresource.c -o tdeletedresource.o $(CFLAGS)

tclient.o: tclient.c mysqlrad.h language.h tclientfunc.h local.h
	cc -c tclient.c -o tclient.o $(CFLAGS)

tauthorize.o: tauthorize.c mysqlrad.h language.h tauthorizefunc.h local.h
	cc -c tauthorize.c -o tauthorize.o $(CFLAGS)


main.o: main.c mysqlrad.h mainfunc.h language.h local.h
	cc -c main.c -o main.o $(CFLAGS)

cgi.o: cgi.h cgi.c
	cc -c cgi.c -o cgi.o $(CFLAGS)

#List: bind.o
bind.o: bind.c mysqlrad.h extjobqueue.c
	cc -c bind.c -o bind.o $(CFLAGS)

clean:
	rm -f *.o iDNS.cgi

cleandist:
	rm -f *.o *.gz

install: iDNS.cgi
ifdef BUILDCHECK
	./install.sh
else
	install -s iDNS.cgi $(CGIDIR)iDNS.cgi
	rm iDNS.cgi
endif

clean-install : uninstall.sh
	./uninstall.sh

distribution:
	cd .. ;  tar czvf iDNS/iDNS$(RELEASE).tar.gz --exclude .svn\
		iDNS/*.h\
		iDNS/*.c\
		iDNS/data\
		iDNS/docs\
		iDNS/install.sh\
		iDNS/buildcheck.sh\
		iDNS/uninstall.sh\
		iDNS/testjobqueue.sh\
		iDNS/script-include.sh\
		iDNS/setup9\
		iDNS/makefile\
		iDNS/LICENSE\
		iDNS/INSTALL\
		iDNS/images\
		iDNS/interfaces/admin/*.c\
		iDNS/interfaces/admin/*.h\
		iDNS/interfaces/admin/install.sh\
		iDNS/interfaces/admin/importTemplates.sh\
		iDNS/interfaces/admin/makefile\
		iDNS/interfaces/admin/templates\
		iDNS/interfaces/admin/INSTALL\
		iDNS/interfaces/organization/install.sh\
		iDNS/interfaces/organization/importTemplates.sh\
		iDNS/interfaces/organization/*.c\
		iDNS/interfaces/organization/*.h\
		iDNS/interfaces/organization/makefile\
		iDNS/interfaces/organization/INSTALL\
		iDNS/interfaces/organization/templates\
		iDNS/interfaces/querylog/*.c\
		iDNS/interfaces/querylog/makefile\
		iDNS/interfaces/errorlog/main.c\
		iDNS/interfaces/errorlog/makefile

update : update.sh
	./update.sh

test :
	./testjobqueue.sh
