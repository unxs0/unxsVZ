#
# svn ID removed
# tools/oathtool/makefile
#
# (C) 2013 Gary Wallis for Unixservice, LLC.
#
#

CFLAGS= -Wall
//LIBS= -L/usr/lib/oath -loath
LIBS= -loath

all: oathtool

oathtool: oathtool.o
	cc oathtool.o -o oathtool $(LIBS) 

oathtool.o: oathtool.c /usr/include/liboath/oath.h
	cc -c oathtool.c -o oathtool.o $(CFLAGS)

clean:
	rm -f *.o oathtool

install: oathtool
	install -s oathtool /usr/sbin/oathtool
	rm oathtool
