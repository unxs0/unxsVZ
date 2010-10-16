#!/bin/bash

#FILE
#	PrepImportAxfrZones.sh
#
#PURPOSE
#	Prep iDNS.cgi ImportAxfrZones data.
#	Simple bash program to import zone data from dig axfr files.
#AUTHOR/LEGAL
#	(C) 2010 Unixservice, LLC. GPLv2 license applies.
#
#EXAMPLE USE
#	Create zones in iDNS backend. 
#	Create /usr/local/idns/axfr/zonelist.txt with one zone per line.
#	Edit NSs below. And maybe the views if you use more than these or
#	with different names.
#	Run this script to gather axfr data and place in correct place.
#	Check.
#	Run /var/www/unxs/cgi-bin/iDNS.cgi ImportAxfrZones.
#	Check.
#	Then do a mass update via backend to push data into BIND.
#
#NOTES
#	If you need to run /var/www/unxs/cgi-bin/iDNS.cgi ImportAxfrZones again
#	you will need to remove the RRs created unless you want dups or changed 
#	the /usr/local/idns/axfr/zonelist.txt file.
#
#	In many cases you will need to run this script from two different servers
#	to get the correct view transferred. In that case modify this script
#	for a single view and then move the data to the server you will
#	run ImportAxfrZones on. This can be tricky to get right.
#

#Set these and run from correct server to get correct view data.
cView1NS="127.0.0.1";
cView1="internal";
cView2NS="127.0.0.1";
cView2="external";

mkdir -p /usr/local/idns/axfr/$cView1;
mkdir -p /usr/local/idns/axfr/$cView2;

if [ ! -f /usr/local/idns/axfr/zonelist.txt ];then
	echo "No /usr/local/idns/axfr/zonelist.txt file found...exiting";
	exit 1;
fi

exec</usr/local/idns/axfr/zonelist.txt;
while read cZone
do
	echo "$cZone";
	dig @$cView1NS $cZone axfr > /usr/local/idns/axfr/$cView1/$cZone;
	if [ $? != 0 ];then
		echo "dig @$cView1NS $cZone error";
	fi
	dig @$cView2NS $cZone axfr > /usr/local/idns/axfr/$cView2/$cZone;
	if [ $? != 0 ];then
		echo "dig @$cView2NS $cZone error";
	fi
done

exit 0;
