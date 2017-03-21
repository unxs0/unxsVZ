#
#FILE
#       {{cProject}}/Makefile
#       From unxsRAD/templates/default/RAD4/Makefile template.
#AUTHOR/LEGAL
#       Template (C) 2001-2017 Gary Wallis for Unixservice. GPLv2 license applies.
#NOTES
#	Only tested on modern CentOS and Ubuntu
#

#CFLAGS=-O -DLinux -Wall -pedantic
GIT_VERSION := $(shell git describe --dirty --always --tags 2> /dev/null || echo "noGit")
CFLAGS= -DLinux -Wall -DdsGitVersion=\"$(GIT_VERSION)\"
RELEASE= 0.1
#If using lib64 make sure you have adjusted if any errors emerge
LIBS= -L/usr/lib/openisp -L/usr/lib64/mysql -lmysqlclient -lz -lcrypt -lm -lssl -ltemplate
CGIDIR=/var/www/unxs/cgi-bin/

all: {{cProject}}.cgi {{cProject}}

#New standalone CLI job queue model
{{cProject}}: {{cProjectLC}}.o mysqlconnect.o
	cc {{cProjectLC}}.o mysqlconnect.o -o {{cProject}} $(LIBS)

{{cProjectLC}}.o: {{cProjectLC}}.c mysqlrad.h local.h mysqlconnect.c
	cc -c {{cProjectLC}}.c -o {{cProjectLC}}.o $(CFLAGS)

#unxsRAD comes with a sample standalone program rename
{{cProjectLC}}.c: unxsrad.c
	@ if [ ! -f {{cProjectLC}}.c ];then cp -i unxsrad.c {{cProjectLC}}.c; fi

#included standard RAD4 objects
{{cProject}}.cgi: {{funcMakefileObjects}} mysqlconnect.o main.o cgi.o
	cc {{funcMakefileObjects}} mysqlconnect.o main.o cgi.o -o {{cProject}}.cgi $(LIBS) 

{{funcMakefileRules}}

#standard RAD4 
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
