#
#FILE
#	makefile
#	$Id: makefile 1182 2006-11-01 15:16:26Z ggw $
#AUTHOR/LEGAL
#	(C) 2006-2009 Gary Wallis for Unixservice, LLC
#	This program is distributed under the terms of the 
#	GNU Lesser General Public License. 
#	See included LICENSE file.
#PURPOSE
#	Create and install libucidr a very small
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
	cc example.o -o example /usr/lib/openisp/libucidr.a
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
