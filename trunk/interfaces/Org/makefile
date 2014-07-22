#
#FILE
#	makefile
#	$Id: makefile 1456 2010-05-16 22:32:30Z Gary $
#AUTHOR/LEGAL
#	(C) 2006-2009 Gary Wallis and Hugo Urquiza for Unixservice, LLC.
#	(C) 2010 Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies. See included LICENSE file.
#PURPOSE
#	General purpose RAD3 interface makefile
#	Must customize CGINAME 
#

CGINAME= unxsvzOrg.cgi

CFLAGS=-Wall
//LIBS=-L/usr/lib/mysql -L/usr/lib/openisp -lmysqlclient -ltemplate -lucidr -lz -lcrypt -lm -lssl -lldap
LIBS=-L/usr/lib/mysql -L/usr/lib/openisp -L/usr/lib/oath -lmysqlclient -ltemplate -lucidr -lz -lcrypt -lm -lssl -loath

interface.cgi: main.o glossary.o container.o creatednsjob.o user.o ldap.o ../../cgi.o mysqlconnect.o firewallcontrol.o
	cc main.o glossary.o container.o creatednsjob.o user.o ldap.o ../../cgi.o mysqlconnect.o firewallcontrol.o -o interface.cgi $(LIBS)

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

cgi.o: ../../cgi.c  ../../cgi.h
	cc -o ../../cgi.o -c ../../cgi.c $(CFLAGS)

clean:
	rm -f *.o *.cgi
	
install-test: interface.cgi ./templates/Footer ./templates/ZLogin.Body
	install -m 510 -g apache -s interface.cgi /var/www/unxs/cgi-bin/test/$(CGINAME)
	rm -f interface.cgi

install: interface.cgi ./templates/Footer ./templates/ZLogin.Body
	install -m 510 -g apache -s interface.cgi /var/www/unxs/cgi-bin/$(CGINAME)
	rm -f interface.cgi

./templates/Footer: ./templates/Footer.default
	@ if [ ! -f ./templates/Footer ];then cp -i ./templates/Footer.default ./templates/Footer; fi

./templates/ZLogin.Body: ./templates/ZLogin.Body.default
	@ if [ ! -f ./templates/ZLogin.Body ];then cp -i ./templates/ZLogin.Body.default ./templates/ZLogin.Body; fi
