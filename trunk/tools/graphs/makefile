#
#FILE
#       unxsVZ/tools/graphs/makefile
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

all: unxsCpuCheck.cgi unxsPrivmem.cgi unxsRamUtil.cgi unxsDiskUtil.cgi

unxsCpuCheck.cgi: cpucheck.o
	gcc cpucheck.o -o unxsCpuCheck.cgi $(LIBS)

cpucheck.o: cpucheck.c ../../local.h
	gcc -c cpucheck.c -o cpucheck.o $(CFLAGS)

unxsPrivmem.cgi: privmem.o
	gcc privmem.o -o unxsPrivmem.cgi $(LIBS)

privmem.o: privmem.c ../../local.h
	gcc -c privmem.c -o privmem.o $(CFLAGS)

unxsRamUtil.cgi: ramutil.o
	gcc ramutil.o -o unxsRamUtil.cgi $(LIBS)

ramutil.o: ramutil.c ../../local.h
	gcc -c ramutil.c -o ramutil.o $(CFLAGS)

unxsDiskUtil.cgi: diskutil.o
	gcc diskutil.o -o unxsDiskUtil.cgi $(LIBS)

diskutil.o: diskutil.c ../../local.h
	gcc -c diskutil.c -o diskutil.o $(CFLAGS)

clean:
	rm -f *.o *.cgi

install: unxsCpuCheck.cgi unxsPrivmem.cgi unxsRamUtil.cgi unxsDiskUtil.cgi
	install -s unxsCpuCheck.cgi /var/www/unxs/cgi-bin/unxsCpuCheck.cgi
	@ rm unxsCpuCheck.cgi
	install -s unxsPrivmem.cgi /var/www/unxs/cgi-bin/unxsPrivmem.cgi
	@ rm unxsPrivmem.cgi
	install -s unxsRamUtil.cgi /var/www/unxs/cgi-bin/unxsRamUtil.cgi
	@ rm unxsRamUtil.cgi
	install -s unxsDiskUtil.cgi /var/www/unxs/cgi-bin/unxsDiskUtil.cgi
	@ rm unxsDiskUtil.cgi
