#
#FILE
#	makefile
#	svn ID removed
#AUTHOR
#	(C) 2009, Gary Wallis for Unixservice USA.
#

CFLAGS= -Wall
LIBS= -L/usr/lib/mysql -lmysqlclient -lssl -lz

unxsvz-autonomics: main.o autonomics.h datacenter.o node.o container.o
	cc main.o datacenter.o node.o container.o -o unxsvz-autonomics $(LIBS)

main.o: main.c ../../local.h ../../mysqlrad.h autonomics.h
	cc -c main.c -o main.o $(CFLAGS) 

datacenter.o: datacenter.c autonomics.h
	cc -c datacenter.c -o datacenter.o $(CFLAGS) 

node.o: node.c autonomics.h
	cc -c node.c -o node.o $(CFLAGS) 

container.o: container.c autonomics.h
	cc -c container.c -o container.o $(CFLAGS) 

clean:
	rm -f *.o unxsvz-autonomics
	
install: unxsvz-autonomics
	install -m 700 -s unxsvz-autonomics /usr/sbin/unxsvz-autonomics
	rm -f unxsvz-autonomics
