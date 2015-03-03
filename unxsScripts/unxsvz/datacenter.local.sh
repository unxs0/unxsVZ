#!/bin/bash

#FILE
#	/etc/unxsvz/datacenter.local.sh
#PURPOSE
#	To be used by node/datacenter unxsVZ scripts that need private
#	info about nodes and other related items.
#EXAMPLE
#	The distributed scripts will have a top section with default settings.
#	The scripts will source this file for localization of said settings.
#AUTHOR
#	Gary Wallis for Unixservice, LLC. Copyright 2014.
#NOTES
#	This file contains confidential info keep secure!
#	Set chmod 500
#
#	The robustness -mmin -5 code below should also be implemented in the seed first two items.


DATACENTER="YourDatacenterLabel";

#aggreagation instructions generation
DEFSECTION="DEF:node1-in=/var/lib/rrd/node1.rrd:in:MAX DEF:node1-out=/var/lib/rrd/node1.rrd:out:MAX DEF:node2-in=/var/lib/rrd/node2.rrd:in:MAX DEF:node2-out=/var/lib/rrd/node2.rrd:out:MAX";
CDEFSECTIONIN="CDEF:in=node1-in,node2-in,+";
CDEFSECTIONOUT="CDEF:out=node1-out,node2-out,+";

#note how we start 2 after
for N in $(seq 3 12 ); do
	cFile="";
	cFile=`find /var/lib/rrd/rc$N.rrd -mmin -5`;
	if [ ! "$cFile" == "" ];then
		DEFSECTION="$DEFSECTION DEF:node$N-in=/var/lib/rrd/node$N.rrd:in:MAX DEF:node$N-out=/var/lib/rrd/node$N.rrd:out:MAX";
		CDEFSECTIONIN="$CDEFSECTIONIN,node$N-in,+";
		CDEFSECTIONOUT="$CDEFSECTIONOUT,node$N-out,+";
	fi
done;

#debug only
#echo $DEFSECTION;
#echo $CDEFSECTIONIN;
#echo $CDEFSECTIONOUT;
#exit;
