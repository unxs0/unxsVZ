## unxsRAD 

Very preliminary version being built based on old MySQL C app code generator
that was laying around.

Will provide major provider (Amazon/Google/DigitalOcean) VM cloud based Docker DevOps and cloud SQL server support.

### MySQL Data Driven Rapid Application Developer

Paste in your MySQL schema, type in some other details and create your app code base.

Multi-tenant async job queue builds source code directories and ready to run apps
directly from unxsRAD data.

The job queue handler can be on same or any one of many remote servers.

You can paste in your MySQL CREATE TABLE code for quick start.

You can start coding right away in the per table {{table-name}}func.c
app files. 

Simple/fast: One shot compiled C cgi executable, for no memory issues
and interpreted language requirement/version/lib headaches.

You apps can be in node.js/python/go/ruby etc by extending the template
code accordingly to handle the output file extensions.

### Backend Nomenclature and Quick Notes

#### Backend schema prefixes for easy SQL program reading.

* *t* prefix is for a table.

	E.g. tAuthorize.

* *u* prefix is for an unsigned int or long int.

	Or the FK of a table (may show up as the FK.cLabel char string, or a unix timestamp (shown as a date).
	E.g. uAuthorize.

* *c* prefix is for a char string.

	E.g. cLabel as for example tTemplate.cLabel.

#### Main tab provides access to all schema.

### unxsRAD Installation

See unxsRad/data/README.md for sample data install.

Simple things you need to take care of:

* An httpd server that will run .cgi executables (like Apache).
* Install unxsVZ/unxsAdmin js/css/images dirs in your html of httpd server above.
* MySQL server.
* Edit local.h and your Makefile a tad for install paths.

Then:

```
[root@hosting1] cd ~/unxsVZ/unxsRAD

[root@hosting1 unxsRAD]# cd ../unxsTemplateLib/

[root@hosting1 unxsTemplateLib]# make install
cc -c template.c -o template.o -Wall
ar r libtemplate.a template.o
ar: creating libtemplate.a
mkdir -p /usr/lib/openisp
mkdir -p /usr/include/openisp
install libtemplate.a /usr/lib/openisp/libtemplate.a
install template.h /usr/include/openisp/template.h
rm libtemplate.a

[root@hosting1 unxsTemplateLib]# cd ../unxsRAD

[root@hosting1 unxsRAD]# make clean
rm -f *.o
rm -f unxsRAD.cgi
[root@hosting1 unxsRAD]# make install
cc -c tproject.c -o tproject.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c ttable.c -o ttable.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tfield.c -o tfield.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c ttemplate.c -o ttemplate.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c ttemplateset.c -o ttemplateset.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c ttemplatetype.c -o ttemplatetype.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tprojectstatus.c -o tprojectstatus.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tfieldtype.c -o tfieldtype.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tindextype.c -o tindextype.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tclient.c -o tclient.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tauthorize.c -o tauthorize.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tstatus.c -o tstatus.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tlog.c -o tlog.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tlogtype.c -o tlogtype.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tlogmonth.c -o tlogmonth.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tglossary.c -o tglossary.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tjob.c -o tjob.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tmonth.c -o tmonth.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tjobstatus.c -o tjobstatus.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tconfiguration.c -o tconfiguration.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c tserver.c -o tserver.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c mysqlconnect.c -o mysqlconnect.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c main.c -o main.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc -c cgi.c -o cgi.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc tproject.o ttable.o tfield.o ttemplate.o ttemplateset.o ttemplatetype.o tprojectstatus.o\
         tfieldtype.o tindextype.o tclient.o tauthorize.o tstatus.o tlog.o tlogtype.o tlogmonth.o\
         tglossary.o tjob.o tmonth.o tjobstatus.o tconfiguration.o tserver.o mysqlconnect.o  main.o cgi.o -o unxsRAD.cgi -L/usr/lib/openisp -L/usr/lib64/mysql -L/usr/lib/mysql -lz -lcrypt -lm -lssl -ltemplate -lmysqlclient
cc -c unxsrad.c -o unxsrad.o -DLinux -Wall -DdsGitVersion=\"1b43815\"
cc unxsrad.o mysqlconnect.o -o unxsRAD -L/usr/lib/openisp -L/usr/lib64/mysql -L/usr/lib/mysql -lz -lcrypt -lm -lssl -ltemplate -lmysqlclient
install -s unxsRAD.cgi /var/www/cgi-bin/unxsRAD.cgi
install -s unxsRAD /usr/sbin/unxsRAD
rm -f unxsRAD.cgi
rm -f unxsRAD

#run the GUI click on the tProject tab; select/load the sample project unxsSalud; Click on the "Make Source Code"
#click on the tJob tab; make sure a job exists for your local server (you may need to change tServer data); then...

[root@hosting1 unxsRAD]# /usr/sbin/unxsRAD
usage: /usr/sbin/unxsRAD ProcessJobQueue|ProcessJobQueueDebug
        gcHostname: hosting1.unixservice.com
        git build info: 1b43815
[root@hosting1 unxsRAD]# /usr/sbin/unxsRAD ProcessJobQueue
[root@hosting1 unxsRAD]# cd /var/local/unxsRAD/apps/unxsSalud/
[root@hosting1 unxsSalud]# make clean
rm -f *.o
[root@hosting1 unxsSalud]# make
cc -c tpaciente.c -o tpaciente.o -DLinux -Wall
cc -c tconsulta.c -o tconsulta.o -DLinux -Wall
cc -c tinterconsulta.c -o tinterconsulta.o -DLinux -Wall
cc -c tmotivo.c -o tmotivo.o -DLinux -Wall
cc -c tcentro.c -o tcentro.o -DLinux -Wall
cc -c tantecedente.c -o tantecedente.o -DLinux -Wall
cc -c tinternacion.c -o tinternacion.o -DLinux -Wall
cc -c tmedico.c -o tmedico.o -DLinux -Wall
cc -c tmedicacion.c -o tmedicacion.o -DLinux -Wall
cc -c tinmunizacion.c -o tinmunizacion.o -DLinux -Wall
cc -c tclient.c -o tclient.o -DLinux -Wall
cc -c tauthorize.c -o tauthorize.o -DLinux -Wall
cc -c tserver.c -o tserver.o -DLinux -Wall
cc -c tconfiguration.c -o tconfiguration.o -DLinux -Wall
cc -c tstatus.c -o tstatus.o -DLinux -Wall
cc -c tjobstatus.c -o tjobstatus.o -DLinux -Wall
cc -c tjob.c -o tjob.o -DLinux -Wall
cc -c tglossary.c -o tglossary.o -DLinux -Wall
cc -c ttemplate.c -o ttemplate.o -DLinux -Wall
cc -c ttemplateset.c -o ttemplateset.o -DLinux -Wall
cc -c ttemplatetype.c -o ttemplatetype.o -DLinux -Wall
cc -c tlog.c -o tlog.o -DLinux -Wall
cc -c tlogtype.c -o tlogtype.o -DLinux -Wall
cc -c tlogmonth.c -o tlogmonth.o -DLinux -Wall
cc -c tmonth.c -o tmonth.o -DLinux -Wall
cc -c mysqlconnect.c -o mysqlconnect.o -DLinux -Wall
cc -c main.c -o main.o -DLinux -Wall
cc -c cgi.c -o cgi.o -DLinux -Wall
cc tpaciente.o tconsulta.o tinterconsulta.o tmotivo.o tcentro.o tantecedente.o tinternacion.o tmedico.o tmedicacion.o tinmunizacion.o tclient.o tauthorize.o tserver.o tconfiguration.o tstatus.o tjobstatus.o tjob.o tglossary.o ttemplate.o ttemplateset.o ttemplatetype.o tlog.o tlogtype.o tlogmonth.o tmonth.o  mysqlconnect.o main.o cgi.o -o unxsSalud.cgi -L/usr/lib/openisp -L/usr/lib64/mysql -lmysqlclient -lz -lcrypt -lm -lssl -ltemplate
cc -c unxssalud.c -o unxssalud.o -DLinux -Wall
cc unxssalud.o mysqlconnect.o -o unxsSalud -L/usr/lib/openisp -L/usr/lib64/mysql -lmysqlclient -lz -lcrypt -lm -lssl -ltemplate
```

#### Non standard required libs

mysql-devel: libmysqlclient.a
unxsTemplateLib: libtemplate.a

#### Straightforward standalone C

```
make
vi local.h #set MySQL server items
vi Makefile #set cgi-bin and html paths
make install
```

#### httpd content notes

You must use unxsVZ/unxsAdmin images/css and js content
in your html dir

### User Guide

Nothing yet.

