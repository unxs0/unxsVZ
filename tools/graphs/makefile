#
#FILE
#       makefile
#PURPOSE
#       Very simple gmake file to:
#       Compile aux data analysis programs that for the most part provide
#       on the fly graphic summaries using the GD/C graphics lib provided by
#       Boutell and Bruce Verderaime. Can also be used as cgi's.
#USE
#       You need to install the included gdc graphics software and modify the 
#	.c files according to your needs.
#AUTHOR
#       (C) 2006-2009 Gary Wallis for Unixservice, LLC.
#       GPL License applies and applies to all thrid party libs used herein.
#

CFLAGS= -Wall -DHAVE_LIBFREETYPE -DHAVE_JPEG

LIBS= -L/usr/lib/mysql -lmysqlclient -lz -lm -lgdc -lgd -lpng -lfreetype -ljpeg


unxsNodeGraphs: node.o
	gcc node.o -o unxsNodeGraphs $(LIBS)

node.o: node.c ../../local.h
	gcc -c node.c -o node.o $(CFLAGS)

clean:
	rm -f *.o unxsNodeGraphs

install: unxsNodeGraphs
	install -s unxsNodeGraphs /usr/sbin/unxsNodeGraphs
	@ rm unxsNodeGraphs
