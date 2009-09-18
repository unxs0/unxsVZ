#!/bin/sh
#
#FILE
#	/usr/local/sbin/datacenter_traffic_log.sh
#PURPOSE
#	Graph all node container (or node see DEV) traffic
#NOTES
#	Requires root ssh via public key, rrdtool and allnodescp.sh setup 
#	correctly.
#	Graphs are for OPenVZ venet containers not for veth bridged containers.
#	This script varies from the others only runs on a single node or 
#	external node server.

#DATACENTER must not have spaces
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
		echo "datacenter_traffic_log.sh eval error";
		exit 1;
	fi
done


#note reversal 
nice /usr/bin/rrdtool update $RRDFILE N:$CtOutTotal:$CtInTotal

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

nice /usr/sbin/allnodescp.sh $PNGFILE > /dev/null 2>&1;
