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

if [ "$1" == "" ];then
	echo "usage: $0 create";
	exit 0;
fi

fLog "start";

if [ -f "/etc/unxsvz/datacenter.local.sh" ];then
	source /etc/unxsvz/datacenter.local.sh;
else
	echo "no /etc/unxsvz/datacenter.local.sh";
	exit 1;
fi

if [ "$DATACENTER" == "" ];then
	echo "no DATACENTER";
	exit 1;
fi

if [ "$DEFSECTION" == "" ];then
	echo "no DEFSECTION";
	exit 1;
fi

if [ "$CDEFSECTIONIN" == "" ];then
	echo "no CDEFSECTIONIN";
	exit 1;
fi

if [ "$CDEFSECTIONOUT" == "" ];then
	echo "no CDEFSECTIONOUT";
	exit 1;
fi

PNGFILE="/var/www/unxs/html/traffic/$DATACENTER.png"


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
		"GPRINT:out:LAST:Last out\:%0.0lf" > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "rrdtool graph $PNGFILE error";
	exit 0;
fi

/usr/sbin/allnodescp.sh $PNGFILE --remotedatacenter > /dev/null 2>&1;
if [ $? != 0 ];then
        fLog "allnodescp.sh $PNGFILE error";
        exit 1;
fi

fLog "end";
