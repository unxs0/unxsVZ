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
LIBS= -L/usr/lib/mysql -lmysqlclient -lz -lm -lgdc -lgd -lpng -lfreetype -ljpeg -lssl

all: unxsCpuCheck.cgi unxsPrivmem.cgi unxsRamUtil.cgi unxsDiskUtil.cgi unxsPerNode.cgi

unxsCpuCheck.cgi: cpucheck.o mysqlconnect.o
	gcc cpucheck.o mysqlconnect.o -o unxsCpuCheck.cgi $(LIBS)

cpucheck.o: cpucheck.c ../../local.h mysqlconnect.h
	gcc -c cpucheck.c -o cpucheck.o $(CFLAGS)

unxsPerNode.cgi: pernode.o mysqlconnect.o
	gcc pernode.o mysqlconnect.o -o unxsPerNode.cgi $(LIBS)

pernode.o: pernode.c ../../local.h mysqlconnect.h
	gcc -c pernode.c -o pernode.o $(CFLAGS)

unxsPrivmem.cgi: privmem.o mysqlconnect.o
	gcc privmem.o mysqlconnect.o -o unxsPrivmem.cgi $(LIBS)

privmem.o: privmem.c ../../local.h mysqlconnect.h
	gcc -c privmem.c -o privmem.o $(CFLAGS)

unxsRamUtil.cgi: ramutil.o mysqlconnect.o
	gcc ramutil.o mysqlconnect.o -o unxsRamUtil.cgi $(LIBS)

ramutil.o: ramutil.c ../../local.h mysqlconnect.h
	gcc -c ramutil.c -o ramutil.o $(CFLAGS)

unxsDiskUtil.cgi: diskutil.o mysqlconnect.o
	gcc diskutil.o mysqlconnect.o -o unxsDiskUtil.cgi $(LIBS)

diskutil.o: diskutil.c mysqlconnect.c ../../local.h mysqlconnect.h
	gcc -c diskutil.c -o diskutil.o $(CFLAGS)

mysqlconnect.o: mysqlconnect.c ../../mysqlrad.h ../../local.h
	cc -c mysqlconnect.c -o mysqlconnect.o $(CFLAGS)

clean:
	rm -f *.o *.cgi

install: unxsCpuCheck.cgi unxsPrivmem.cgi unxsRamUtil.cgi unxsDiskUtil.cgi unxsPerNode.cgi
	install -s unxsCpuCheck.cgi /var/www/unxs/cgi-bin/unxsCpuCheck.cgi
	@ rm unxsCpuCheck.cgi
	install -s unxsPrivmem.cgi /var/www/unxs/cgi-bin/unxsPrivmem.cgi
	@ rm unxsPrivmem.cgi
	install -s unxsRamUtil.cgi /var/www/unxs/cgi-bin/unxsRamUtil.cgi
	@ rm unxsRamUtil.cgi
	install -s unxsDiskUtil.cgi /var/www/unxs/cgi-bin/unxsDiskUtil.cgi
	@ rm unxsDiskUtil.cgi
	install -s unxsPerNode.cgi /var/www/unxs/cgi-bin/unxsPerNode.cgi
	@ rm unxsPerNode.cgi
