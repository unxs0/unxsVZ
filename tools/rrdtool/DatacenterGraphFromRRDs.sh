#!/bin/bash

#FILE
#	unxsVZ/tools/rrdtool/DatacenterGraphFromRRDs.sh
#PURPOSE
#	Create a datacenter graph by aggregating all the node rrd data.
#	To replace much less efficient and duplicated effort soon
#	to be deprecated:
#		datacenter_traffic_log.sh
#REQUIRES
#OS
#	Only tested on CentOS 5+
#LEGAL
#	Copyright (C) Unixservice, LLC. 2009-2014.
#	GPLv2 license applies.
#

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

DATACENTER="HurricaneElectric";
PNGFILE="/var/www/unxs/html/traffic/$DATACENTER.png"

#aggreagation instructions generation
DEFSECTION="DEF:rc13-in=/var/lib/rrd/rc13.rrd:in:MAX DEF:rc13-out=/var/lib/rrd/rc13.rrd:out:MAX DEF:rc14-in=/var/lib/rrd/rc14.rrd:in:MAX DEF:rc14-out=/var/lib/rrd/rc14.rrd:out:MAX";
CDEFSECTIONIN="CDEF:in=rc13-in,rc14-in,+";
CDEFSECTIONOUT="CDEF:out=rc13-out,rc14-out,+";
#note how we start 2 after
for N in $(seq 15 20;seq 45 52 ); do
	DEFSECTION="$DEFSECTION DEF:rc$N-in=/var/lib/rrd/rc$N.rrd:in:MAX DEF:rc$N-out=/var/lib/rrd/rc$N.rrd:out:MAX";
	CDEFSECTIONIN="$CDEFSECTIONIN,rc$N-in,+";
	CDEFSECTIONOUT="$CDEFSECTIONOUT,rc$N-out,+";
done;

#debug only
#echo $DEFSECTION;
#echo $CDEFSECTIONIN;
#echo $CDEFSECTIONOUT;
#exit;

/usr/bin/rrdtool graph $PNGFILE \
		--title="$DATACENTER traffic" \
		--vertical-label="bytes per second" \
		--base=1000 \
		--height=120 \
		--width=500 \
		--slope-mode \
		--font TITLE:10: \
		--font AXIS:6: \
		--font LEGEND:8: \
		--font UNIT:8: \
		$DEFSECTION \
		$CDEFSECTIONIN \
		$CDEFSECTIONOUT \
		"LINE1:in#00ff00:in" \
		"LINE1:out#0000ff:out" \
		"GPRINT:in:MAX: Max in\:%0.0lf" \
		"GPRINT:out:MAX:Max out\:%0.0lf" \
		"GPRINT:in:LAST:Last in\:%0.0lf" \
		"GPRINT:out:LAST:Last out\:%0.0lf";
#		"GPRINT:out:LAST:Last out\:%0.0lf" > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "rrdtool graph $PNGFILE error";
	exit 0;
fi
