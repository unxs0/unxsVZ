#!/bin/sh
#
#FILE
#	container_year_graph.sh
#PURPOSE
#	Last year simple graph based on vz_traffic_log.sh
#	generated and consolidated rrd data.

if [ "$1" == "" ];then
	echo "usage: $0 <uVEID>";
	exit 0;
fi

#VEID must not have spaces
VEID="$1"; 
RRDFILE="/var/lib/rrd/$VEID.rrd"
PNGFILE="/var/www/unxs/html/traffic/$VEID.year.png"

nice /usr/bin/rrdtool graph $PNGFILE --start end-1y\
		--title="$VEID last year traffic" \
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
		"GPRINT:out:LAST:Last out\:%0.0lf"

/usr/sbin/allnodescp.sh $PNGFILE;
