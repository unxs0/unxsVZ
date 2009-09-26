#!/bin/sh
#
#FILE
#	/usr/local/sbin/datacenter_week_graph.sh
#PURPOSE
#	Last week simple graph based on /usr/local/sbin/datacenter_traffic_log.sh
#	generated and consolidated rrd data.

#DATACENTER must not have spaces
DATACENTER="Wilshire1"; 
RRDFILE="/var/lib/rrd/$DATACENTER.rrd"
PNGFILE="/var/www/unxs/html/traffic/$DATACENTER.week.png"

nice /usr/bin/rrdtool graph $PNGFILE --start end-1w\
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
		"DEF:in=$RRDFILE:in:MAX" \
		"DEF:out=$RRDFILE:out:MAX" \
		"LINE1:in#00ff00:in" \
		"LINE1:out#0000ff:out" \
		"GPRINT:in:MAX: Max in\:%0.0lf" \
		"GPRINT:out:MAX:Max out\:%0.0lf" \
		"GPRINT:in:LAST:Last in\:%0.0lf" \
		"GPRINT:out:LAST:Last out\:%0.0lf" > /dev/null 2>&1;
