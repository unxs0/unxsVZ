#
#FILE
#	unxsVZ/tools/mysql-server/repcluster-health/makefile
#	svn ID removed
#AUTHOR/LEGAL
#	(C) 2001-2010 Gary Wallis for Unixservice. GPLv2 license applies.
#NOTES
#

#CFLAGS=-Wall -pedantic
CFLAGS=-Wall

repcluster-health: main.c
	gcc main.c -o repcluster-health -L/usr/lib/mysql -lmysqlclient $(CFLAGS)

clean:
	rm -f repcluster-health *.o

install: repcluster-health
	install -m 500 -s repcluster-health /usr/sbin/repcluster-health
	rm -f repcluster-health
