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


DATACENTER="YourDatacenterLabel";

#aggreagation instructions generation
DEFSECTION="DEF:node13-in=/var/lib/rrd/node13.rrd:in:MAX DEF:node13-out=/var/lib/rrd/node13.rrd:out:MAX DEF:node14-in=/var/lib/rrd/node14.rrd:in:MAX DEF:node14-out=/var/lib/rrd/node14.rrd:out:MAX";
CDEFSECTIONIN="CDEF:in=node13-in,node14-in,+";
CDEFSECTIONOUT="CDEF:out=node13-out,node14-out,+";

#note how we start 2 after
for N in $(seq 1 12 ); do
	DEFSECTION="$DEFSECTION DEF:node$N-in=/var/lib/rrd/node$N.rrd:in:MAX DEF:node$N-out=/var/lib/rrd/node$N.rrd:out:MAX";
	CDEFSECTIONIN="$CDEFSECTIONIN,node$N-in,+";
	CDEFSECTIONOUT="$CDEFSECTIONOUT,node$N-out,+";
done;

#debug only
#echo $DEFSECTION;
#echo $CDEFSECTIONIN;
#echo $CDEFSECTIONOUT;
#exit;
