#!/bin/sh
#
#FILE
#	/usr/local/sbin/datacenter_traffic_log.sh
#PURPOSE
#	Graph all node container (or nodes see DEV) for complete datacenter traffic
#	Updated for clean unxsVZ standard bash logging to stdout.
#REQUIRES
#	/bin/bash
#	/usr/sbin/ssh
#	/usr/sbin/vzlist
#	/usr/sbin/vzctl
#	/bin/awk
#	/usr/bin/rrdtool
#	unxsVZ localized /usr/sbin/allnodescp.sh
#	Localization for node naming scheme and ssh port used
#	Localization of datacenter name
#OS
#	Only tested on CentOS 5+ with OpenVZ
#LEGAL
#	Copyright (C) Unixservice, LLC. 2009.
#	GPLv2 license applies.
#

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }


#DATACENTER must not have spaces and must be set in tConfiguration with .png
DATACENTER="Wilshire1"; 
RRDFILE="/var/lib/rrd/$DATACENTER.rrd"
DEV="venet0";
 
if ! test -e $RRDFILE; then
	/usr/bin/rrdtool create $RRDFILE --start N --step 300 \
	DS:in:COUNTER:600:U:U \
	DS:out:COUNTER:600:U:U \
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
		exit 1;
	fi
fi

#for each datacenter node then add them , set this same as allnodecmd.sh
#cNameSuffix="lax-f2";
cNameSuffix="vm";
cNamePrefix="node";
uPort="22";
#Set the seq range for your datacenter
for N in $(seq 1 2 ); do
	eval `nice /usr/bin/ssh -p $uPort $cNamePrefix$N$cNameSuffix grep $DEV /proc/net/dev \
			 | awk -F: '{print $2}' | awk '{printf"CtIn=%-15d\nCtOut=%-15d\n", $1, $9}'`;
	if [ $? == 0 ] && [ "$CtIn" != "" ] && [ "$CtOut" != "" ];then
		CtInTotal=$[$CtInTotal+$CtIn];
		CtOutTotal=$[$CtOutTotal+$CtOut];
	else
		fLog "eval $N error";
		exit 1;
	fi
done


#note reversal 
nice /usr/bin/rrdtool update $RRDFILE N:$CtOutTotal:$CtInTotal
if [ $? != 0 ];then
	fLog "rrdtool update $RRDFILE error";
	exit 1;
fi

PNGFILE="/var/www/unxs/html/traffic/$DATACENTER.png"

nice /usr/bin/rrdtool graph $PNGFILE \
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
	exit 1;
fi

nice /usr/sbin/allnodescp.sh $PNGFILE > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "allnodescp.sh $PNGFILE error";
	exit 1;
fi
