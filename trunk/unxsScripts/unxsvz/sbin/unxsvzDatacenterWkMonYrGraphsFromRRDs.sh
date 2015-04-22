#!/bin/bash

#FILE
#	/usr/sbin/DatacenterWkMonYrGraphsFromRRDs.sh
#PURPOSE
#	Create datacenter week/month/year graphs by aggregating all the node rrd data.
#REQUIRES
#OS
#	Only tested on CentOS 5 and 6
#LEGAL
#	Copyright (C) Gary Wallis for Unixservice, LLC. 2009-2015.
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
	fLog "no /etc/unxsvz/datacenter.local.sh";
	exit 1;
fi

if [ "$DATACENTER" == "" ];then
	fLog "no DATACENTER";
	exit 1;
fi

if [ "$DEFSECTION" == "" ];then
	fLog "no DEFSECTION";
	exit 1;
fi

if [ "$CDEFSECTIONIN" == "" ];then
	fLog "no CDEFSECTIONIN";
	exit 1;
fi

if [ "$CDEFSECTIONOUT" == "" ];then
	fLog "no CDEFSECTIONOUT";
	exit 1;
fi

PNGFILE="/var/www/unxs/html/traffic/$DATACENTER.week.png"
/usr/bin/rrdtool graph $PNGFILE --start end-1w\
		--title="$DATACENTER last week traffic" \
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
/usr/sbin/unxsvzAllNodeScp.sh $PNGFILE --remotedatacenter > /dev/null 2>&1;
if [ $? != 0 ];then
        fLog "unxsvzAllNodeScp.sh $PNGFILE error";
        exit 1;
fi

PNGFILE="/var/www/unxs/html/traffic/$DATACENTER.month.png"
/usr/bin/rrdtool graph $PNGFILE --start end-1m\
		--title="$DATACENTER last month traffic" \
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
/usr/sbin/unxsvzAllNodeScp.sh $PNGFILE --remotedatacenter > /dev/null 2>&1;
if [ $? != 0 ];then
        fLog "unxsvzAllNodeScp.sh $PNGFILE error";
        exit 1;
fi

PNGFILE="/var/www/unxs/html/traffic/$DATACENTER.year.png"
/usr/bin/rrdtool graph $PNGFILE --start end-1y\
		--title="$DATACENTER last year traffic" \
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
/usr/sbin/unxsvzAllNodeScp.sh $PNGFILE --remotedatacenter > /dev/null 2>&1;
if [ $? != 0 ];then
        fLog "unxsvzAllNodeScp.sh $PNGFILE error";
        exit 1;
fi

fLog "end";
