#!/bin/bash

#FILE
#	/usr/sbin/unxsvzLogNodeTrafficForGraphs.sh
#	From unxsVZ/unxsScripts/unxsvz/sbin/unxsvzLogNodeTrafficForGraphs.sh
#PURPOSE
#	Graph all node container (or node see DEV) traffic
#	Updated for clean unxsVZ standard bash logging to stdout.
#	We changed to use hostname -a (the short hostname) it should be in the
#	/etc/hosts file (for jobqueue.c use in some cases?)
#REQUIRES
#	hostname (/etc/hosts should be configured correctly if using -a)
#	/bin/bash
#	/usr/sbin/vzlist
#	/usr/sbin/vzctl
#	/bin/awk
#	/usr/bin/rrdtool
#	/usr/sbin/unxsvzAllNodeScp.sh
#OS
#	Only tested on CentOS 5 and 6 with OpenVZ
#LEGAL
#	Copyright (C) Unixservice, LLC. 2009-2015
#	GPLv2 license applies.
#

if [ -f "/etc/unxsvz/unxsvz.functions.sh" ];then
	source /etc/unxsvz/unxsvz.functions.sh;
else
	echo "no /etc/unxsvz/unxsvz.functions.sh";
	exit 1;
fi

#This may need to be adjusted here on at your cluster /etc/hosts files for example
HOSTNAME=`hostname -s`; 
if [ "$HOSTNAME" == "" ];then
	fLog "hostname error";
	exit 0;
fi
RRDFILE="/var/lib/rrd/$HOSTNAME.rrd";
#Prefer to graph only container traffic. 
#ISP should have node traffic graphics available via switch etc.
#DEV="eth0";
DEV="venet0";
 
if ! test -e $RRDFILE; then
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
		fLog "rrdtool create $RRDFILE error";
		exit 0;
	fi
fi

#only gather data if vz is running, this should avoid nasty spikes that are very hard to eliminate correctly.
/sbin/service vz status | grep run > /dev/null 2>&1;
if [ $? != 0 ];then
	
	fLog "vz not running will not collect, add or graph data";
	exit 1;
fi

 
eval `grep $DEV /proc/net/dev  | awk -F: '{print $2}' | awk '{printf"CTIN=%-15d\nCTOUT=%-15d\n", $1, $9}'`
if [ $? == 0 ] && [ "$CTIN" != "" ] && [ "$CTOUT" != "" ];then

	#debug only
	#echo "$RRDFILE N:$CTOUT:$CTIN"
	#note reversal 
	nice /usr/bin/rrdtool update $RRDFILE N:$CTOUT:$CTIN
	if [ $? != 0 ];then
		fLog "rrdtool update $RRDFILE error";
		exit 0;
	fi

	PNGFILE="/var/www/unxs/html/traffic/$HOSTNAME.png"

	nice /usr/bin/rrdtool graph $PNGFILE \
		--title="$HOSTNAME node traffic" \
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
		"GPRINT:in:MAX: Max in\:%0.0lf" \
		"GPRINT:out:MAX:Max out\:%0.0lf" \
		"GPRINT:in:LAST:Last in\:%0.0lf" \
		"GPRINT:out:LAST:Last out\:%0.0lf" > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "rrdtool graph $PNGFILE error";
		exit 0;
	fi

	#copy images to all nodes that may run unxsVZ GUI
	nice /usr/sbin/unxsvzAllNodeScp.sh $PNGFILE > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "unxsvzAllNodeScp.sh $PNGFILE error";
		exit 0;
	fi
fi
