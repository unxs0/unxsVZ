#!/bin/bash

#FILE
#	unxsVZ/tools/rrdtool/vz_year_graphs.sh
#PURPOSE
#	create year png for all
#REQUIRES
#	running unxsVZ/tools/rrdtool/vz_traffic_log.sh
#	/bin/bash
#	/usr/sbin/vzlist
#	/usr/sbin/vzctl
#	/bin/awk
#	/usr/bin/rrdtool
#	unxsVZ localized /usr/sbin/allnodescp.sh
#OS
#	Only tested on CentOS 5+ with OpenVZ
#LEGAL
#	Copyright (C) Unixservice, LLC. 2013.
#	GPLv2 license applies.
#

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

/usr/sbin/vzlist -a > /dev/null 2>&1; 
if [ $? != 0 ];then
	fLog "vzlist error";
	exit 0;
fi
/usr/sbin/vzlist > /dev/null 2>&1; 
if [ $? != 0 ];then
	#no active containers
	exit 0;
fi
for veid in `/usr/sbin/vzlist -o veid -H | sed 's/ //g'`; do

	RRDFILE="/var/lib/rrd/$veid.rrd";
	HTMLFILE="/var/www/unxs/html/traffic/$veid.html";

	if [ ! -f $HTMLFILE ];then
		echo "<img src=$veid.png><p>" >> $HTMLFILE;
		echo "<img src=$veid.week.png><p>" >> $HTMLFILE;
		echo "<img src=$veid.month.png><p>" >> $HTMLFILE;
		echo "<img src=$veid.year.png>" >> $HTMLFILE;
		/usr/sbin/allnodescp.sh $HTMLFILE > /dev/null 2>&1;
		if [ $? != 0 ];then
			fLog "allnodescp.sh $HTMLFILE error";
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
	nice /usr/sbin/allnodescp.sh $PNGFILE > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "allnodescp.sh $PNGFILE error";
		continue;
	fi

done
