#!/bin/sh
#
# Example crontab bash script for providing per container mrtg style traffic
# graphs.
#

/usr/sbin/vzlist > /dev/null 2>&1; 
if [ $? != 0 ];then
	exit 0;
fi
for veid in `/usr/sbin/vzlist -o veid -H | sed 's/ //g'`; do
 
	RRDFILE="/var/lib/rrd/$veid.rrd"
 
	if ! test -e $RRDFILE; then
		#1 day every sample and one day of 15 min avgs
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
 
	eval `/usr/sbin/vzctl exec $veid "grep venet0 /proc/net/dev"  |  \
		awk -F: '{print $2}' | awk '{printf"CTIN=%-15d\nCTOUT=%-15d\n", $1, $9}'`
	if [ $? != 0 ] || [ "$CTIN" == "" ] || [ "$CTOUT" == "" ];then
		echo "vz_traffic_log.sh eval error";
		continue;
	fi
 
	nice /usr/bin/rrdtool update $RRDFILE N:$CTIN:$CTOUT

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

	#Here you need to copy all graphics to all nodes that provide possible unxsVZ admin interfaces.
	nice /usr/sbin/allnodescp.sh $PNGFILE > /dev/null 2>&1;

done
