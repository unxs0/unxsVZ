#
# FILE
#       iDNS/tools/makefile
#       svn ID removed
# AUTHOR/LEGAL
# (C) 2001-2009 Gary Wallis under GPL License see www.fsf.org for details

lastmonth: lastmonth.c
	cc lastmonth.c -o lastmonth

clean:
	rm -f *.o lastmonth

install: lastmonth
	install -s lastmonth /usr/bin/lastmonth
	rm lastmonth
