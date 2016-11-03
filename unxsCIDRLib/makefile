#
#FILE
#	makefile
#	svn ID removed
#AUTHOR/LEGAL
# (C) 2006 Gary Wallis.
#PURPOSE
#	Create and install openisp libucidr a very small
#	basic cdir math lib developed initially for
#	checking to see if a given ipv4 is in a
#	given arbitrary ipv4 CIDR block such as 10.0.0.0/24
#

CFLAGS= -Wall

all: libucidr.a

libucidr.a: ucidr.o ucidr.h
	ar r libucidr.a ucidr.o

ucidr.o: ucidr.c ucidr.h
	cc -c ucidr.c -o ucidr.o $(CFLAGS)

example: example.o
	cc example.o -o example /usr/lib/openisp/libucidr.a -lm
	rm -f example.o

example.o: example.c ucidr.h
	cc -c example.c -o example.o $(CFLAGS)

clean:
	rm -f *.o *.a example

install: libucidr.a ucidr.h
	mkdir -p /usr/lib/openisp
	mkdir -p /usr/include/openisp
	install libucidr.a /usr/lib/openisp/libucidr.a
	install ucidr.h /usr/include/openisp/ucidr.h
	rm libucidr.a
