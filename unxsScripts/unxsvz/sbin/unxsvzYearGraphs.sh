#!/bin/bash

#FILE
#	/usr/sbin/unxsvzYearGraphs.sh
#	From unxsVZ/unxsScripts/unxsvz/sbin/unxsvzYearGraphs.sh
#	$Id$
#PURPOSE
#	create year png for all containers
#	create simple html for set of graphics
#REQUIRES
#	running /usr/sbin/unxsvzLogContainerTrafficForGraphs.sh
#	/bin/bash
#	/bin/awk
#	/usr/bin/rrdtool
#	/usr/sbin/vzlist
#	/usr/sbin/vzctl
#	/usr/sbin/unxsvzAllNodeScp.sh
#OS
#	Only tested on CentOS 5 and 6 with OpenVZ
#LEGAL
#	Copyright (C) Unixservice, LLC. 2013-2015
#	GPLv2 license applies.
#

if [ -f "/etc/unxsvz/unxsvz.functions.sh" ];then
	source /etc/unxsvz/unxsvz.functions.sh;
else
	echo "no /etc/unxsvz/unxsvz.functions.sh";
	exit 1;
fi

/usr/sbin/vzlist -a > /dev/null 2>&1; 
if [ $? != 0 ];then
	fLog "vzlist error1";
	exit 0;
fi
/usr/sbin/vzlist > /dev/null 2>&1; 
if [ $? != 0 ];then
	fLog "vzlist error2";
	exit 0;
fi
for veid in `/usr/sbin/vzlist -o veid -H 2>/dev/null| sed 's/ //g'`; do

	RRDFILE="/var/lib/rrd/$veid.rrd";
	HTMLFILE="/var/www/unxs/html/traffic/$veid.html";

	if [ ! -f $HTMLFILE ];then
		echo "<img src=$veid.png><p>" >> $HTMLFILE;
		echo "<img src=$veid.week.png><p>" >> $HTMLFILE;
		echo "<img src=$veid.month.png><p>" >> $HTMLFILE;
		echo "<img src=$veid.year.png>" >> $HTMLFILE;
		/usr/sbin/unxsvzAllNodeScp.sh $HTMLFILE > /dev/null 2>&1;
		if [ $? != 0 ];then
			fLog "/usr/sbin/unxsvzAllNodeScp.sh $HTMLFILE error";
			continue;
		fi
	fi


	if [ ! -f $RRDFILE ];then
		continue;
	fi

	PNGFILE="/var/www/unxs/html/traffic/$veid.year.png";

	nice /usr/bin/rrdtool graph $PNGFILE --start end-1y\
			--title="$veid yearly traffic" \
			--vertical-label="bytes per second" \
			--base=1000 \
			--height=120 \
			--width=500 \
			--slope-mode \
			--font TITLE:10: \
			--font AXIS:6: \
			--font LEGEND:8: \
			--font UNIT:8: \
			"DEF:in=$RRDFILE:in:MAX" \
			"DEF:out=$RRDFILE:out:MAX" \
			"LINE1:in#00ff00:in" \
			"LINE1:out#0000ff:out" \
			"GPRINT:in:MAX:       Max in\:%0.0lf" \
			"GPRINT:out:MAX:Max out\:%0.0lf" \
			"GPRINT:in:LAST:Last in\:%0.0lf" \
			"GPRINT:out:LAST:Last out\:%0.0lf" > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "rrdtool graph $PNGFILE error";
		continue;
	fi

	#Here you need to copy all graphics to all nodes that provide possible unxsVZ admin interfaces.
	nice /usr/sbin/unxsvzAllNodeScp.sh $PNGFILE > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "unxsvzAllNodeScp.sh $PNGFILE error";
		continue;
	fi

done
