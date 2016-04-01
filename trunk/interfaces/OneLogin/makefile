#
#FILE
#	makefile
#	$Id: makefile 1456 2010-05-16 22:32:30Z Gary $
#AUTHOR/LEGAL
#	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice, LLC.
#	(C) 2010-2015 Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies. See included LICENSE file.
#PURPOSE
#	General purpose RAD3 interface makefile
#	Must customize CGINAME 
#

CGINAME= OneLogin.cgi

CFLAGS=-Wall
//LIBS=-L/usr/lib/mysql -L/usr/lib/openisp -lmysqlclient -ltemplate -lucidr -lz -lcrypt -lm -lssl -lldap
LIBS=-L/usr/lib64/mysql -L/usr/lib/openisp -L/usr/lib/oath -L/usr/lib64 -lmysqlclient -ltemplate -lucidr -lz -lcrypt -lm -lssl -loath -lcurl

help:
	@echo "These are the main make options available"
	@echo "make interface.cgi"
	@echo "make install"
	@echo "make install-test"
	@echo "make install-bootstrap"
	@echo "Note! The template.default dir contents is initially copied to your local working directory ./templates."
	@echo " This means that if you are developing templates you should edit template.default files and then"
	@echo " use the new ./importDefaultTemplates.sh script"

interface.cgi: main.o glossary.o container.o creatednsjob.o user.o ldap.o ../../cgi.o mysqlconnect.o firewallcontrol.o vitelity.o
	cc main.o glossary.o container.o creatednsjob.o user.o ldap.o ../../cgi.o mysqlconnect.o firewallcontrol.o vitelity.o -o interface.cgi $(LIBS)

main.o: main.c interface.h ../../local.h ../../mysqlrad.h
	cc -c main.c -o main.o $(CFLAGS) 

glossary.o: glossary.c interface.h ../../local.h ../../mysqlrad.h
	cc -c glossary.c -o glossary.o $(CFLAGS)

container.o: container.c interface.h ../../local.h ../../mysqlrad.h
	cc -c container.c -o container.o $(CFLAGS)

creatednsjob.o: creatednsjob.c interface.h ../../local.h ../../mysqlrad.h
	cc -c creatednsjob.c -o creatednsjob.o $(CFLAGS)

user.o: user.c interface.h ../../local.h ../../mysqlrad.h
	cc -c user.c -o user.o $(CFLAGS)

ldap.o: ldap.c  interface.h ../../local.h ../../mysqlrad.h
	cc -o ldap.o -c ldap.c $(CFLAGS)

mysqlconnect.o: mysqlconnect.c  ../../local.h interface.h
	cc -o mysqlconnect.o -c mysqlconnect.c $(CFLAGS)

firewallcontrol.o: firewallcontrol.c  ../../local.h interface.h
	cc -o firewallcontrol.o -c firewallcontrol.c $(CFLAGS)

vitelity.o: vitelity.c  interface.h
	cc -o vitelity.o -c vitelity.c $(CFLAGS)

cgi.o: ../../cgi.c  ../../cgi.h
	cc -o ../../cgi.o -c ../../cgi.c $(CFLAGS)

clean:
	rm -f *.o *.cgi
	
install-test: interface.cgi ./templates/Footer 
	install -m 510 -g apache -s interface.cgi /var/www/unxs/html/test/$(CGINAME)
	rm -f interface.cgi

install: interface.cgi ./templates/Footer
	install -m 510 -g apache -s interface.cgi /var/www/unxs/html/$(CGINAME)
	rm -f interface.cgi

./templates/Footer: ./templates.default/Footer
	@ mkdir -p ./templates
	@ cp -ai ./templates.default/* ./templates/;

install-bootstrap: 
	cp css/*.css /var/www/unxs/html/bs/css/
	cp js/*.js /var/www/unxs/html/bs/js/
	cp fonts/* /var/www/unxs/html/bs/fonts/
