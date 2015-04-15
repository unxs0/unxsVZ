#!/bin/bash

#FILE
#	unxsVZ/unxsScripts/unxsvz/sbin/unxsvzLogContainerTrafficForGraphs.sh
#PURPOSE
#	Example crontab bash script for providing per container mrtg style traffic
#	graphs.
#	Updated for clean unxsVZ standard bash logging to stdout.
#REQUIRES
#	/bin/bash
#	/bin/awk
#	/usr/bin/rrdtool
#	/usr/sbin/vzlist
#	/usr/sbin/vzctl
#	/usr/sbin/unxsvzAllNodeScp.sh
#OS
#	Only tested on CentOS 5+ and 6 with OpenVZ.
#LEGAL
#	Copyright (C) Unixservice, LLC. 2009-2015.
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
	fLog "vzlist error";
	exit 0;
fi
/usr/sbin/vzlist > /dev/null 2>&1; 
if [ $? != 0 ];then
	#no active containers
	exit 0;
fi
for veid in `/usr/sbin/vzlist -o veid -H | sed 's/ //g'`; do
 
	RRDFILE="/var/lib/rrd/$veid.rrd"
 
	if ! test -e $RRDFILE; then
		#1 day every sample and one day of 15 min avgs
		/usr/bin/rrdtool create $RRDFILE --start N --step 300 \
		DS:in:DERIVE:600:0:10000000 \
		DS:out:DERIVE:600:0:10000000 \
		RRA:AVERAGE:0.5:1:600 \
		RRA:AVERAGE:0.5:6:700 \
		RRA:AVERAGE:0.5:24:775 \
		RRA:AVERAGE:0.5:288:797 \
		RRA:MAX:0.5:1:600 \
		RRA:MAX:0.5:6:700 \
		RRA:MAX:0.5:24:775 \
		RRA:MAX:0.5:288:797
		if [ $? != 0 ];then
			fLog "rrdtool create $veid error";
			continue;
		fi
	fi

	#Allow for VETH based containers also 
	/usr/sbin/vzctl exec $veid "/sbin/ifconfig venet0" | grep UP > /dev/null 2>&1;
	if [ $? == 0 ];then
		eval `/usr/sbin/vzctl exec $veid "grep venet0 /proc/net/dev"  |  \
			awk -F: '{print $2}' | awk '{printf"CTIN=%-15d\nCTOUT=%-15d\n", $1, $9}'`
		if [ $? != 0 ] || [ "$CTIN" == "" ] || [ "$CTOUT" == "" ];then
			fLog "eval $veid error1";
			continue;
		fi
	else
		eval `/usr/sbin/vzctl exec $veid "grep eth0 /proc/net/dev"  |  \
			awk -F: '{print $2}' | awk '{printf"CTIN=%-15d\nCTOUT=%-15d\n", $1, $9}'`
		if [ $? != 0 ] || [ "$CTIN" == "" ] || [ "$CTOUT" == "" ];then
			fLog "eval $veid error2";
			continue;
		fi
	fi

	nice /usr/bin/rrdtool update $RRDFILE N:$CTIN:$CTOUT
	if [ $? != 0 ];then
		fLog "rrdtool update $veid error";
		continue;
	fi

	PNGFILE="/var/www/unxs/html/traffic/$veid.png"

	nice /usr/bin/rrdtool graph $PNGFILE \
			--title="$veid traffic" \
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
