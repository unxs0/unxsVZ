#
#
#
CFLAGS=-Wall
LIBS=

upload.cgi: upload.o
	cc upload.o multipart-parser-c/multipart_parser.o -o upload.cgi $(LIBS)

upload.o: upload.c

clean:
	rm -f *.o

install: upload.cgi
	install -s upload.cgi $(CGIDIR)/upload.cgi
	rm -f upload.cgi
