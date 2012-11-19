#!/bin/bash

#FILE
#	unxsVZ/tools/freepbx/qos/nodeQOSGraph.sh
#PURPOSE
#REQUIRES
#OS
#	Only tested on CentOS 5+ with OpenVZ
#LEGAL
#	Copyright (C) Unixservice, LLC. 2012.
#	GPLv2 license applies.
#

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

#This may need to be adjusted here on at your cluster /etc/hosts files for example
HOSTNAME=`hostname -s`; 
if [ "$HOSTNAME" == "" ];then
	fLog "hostname error";
	exit 0;
fi
RRDFILE="/var/lib/rrd/$HOSTNAME-nodeQOS.rrd";
 
if ! test -e $RRDFILE; then
	/usr/bin/rrdtool create $RRDFILE --start N --step 60 \
	DS:LossSend:GAUGE:120:0:10000 \
	DS:LossRecv:GAUGE:120:0:10000 \
	DS:JitterSend:GAUGE:120:0:10000 \
	DS:JitterRecv:GAUGE:120:0:10000 \
	DS:NumCalls:GAUGE:120:0:10000 \
	RRA:AVERAGE:0.5:1:1440 \
	RRA:AVERAGE:0.5:6:1680 \
	RRA:AVERAGE:0.5:24:1800 \
	RRA:AVERAGE:0.5:288:1825 \
	RRA:MAX:0.5:1:1440 \
	RRA:MAX:0.5:6:1680 \
	RRA:MAX:0.5:24:1800 \
	RRA:MAX:0.5:288:1825
	if [ $? != 0 ];then
		fLog "rrdtool create $RRDFILE error";
		exit 0;
	fi
fi

cData=`/usr/sbin/nodeQOS --rrdtool`;
cDataArray=( $cData );
LossSend=${cDataArray[0]};
LossRecv=${cDataArray[1]};
JitterSend=${cDataArray[2]};
JitterRecv=${cDataArray[3]};
NumCalls=${cDataArray[4]};

echo "$LossSend:$LossRecv:$JitterSend:$JitterRecv:$NumCalls";

nice /usr/bin/rrdtool update $RRDFILE N:$LossSend:$LossRecv:$JitterSend:$JitterRecv:$NumCalls
if [ $? != 0 ];then
	fLog "rrdtool update $RRDFILE error";
	exit 0;
fi

PNGFILE="/var/www/unxs/html/traffic/$HOSTNAME-nodeQOS.png"

nice /usr/bin/rrdtool graph $PNGFILE \
		--start "end-8 hours" \
		--title="$HOSTNAME QOS" \
		--vertical-label="percent*100 ms*100" \
		--height=240 \
		--width=1000 \
		--slope-mode \
		--font TITLE:10: \
		--font AXIS:6: \
		--font LEGEND:8: \
		--font UNIT:8: \
		--right-axis 10:0 \
		"DEF:LossSend=$RRDFILE:LossSend:MAX" \
		"DEF:LossRecv=$RRDFILE:LossRecv:MAX" \
		"DEF:JitterSend=$RRDFILE:JitterSend:MAX" \
		"DEF:JitterRecv=$RRDFILE:JitterRecv:MAX" \
		"AREA:LossRecv#81BEF7:LossRecv" \
		"LINE1:LossSend#0000FF:LossSend" \
		"LINE1:JitterRecv#40FF00:JitterRecv" \
		"LINE1:JitterSend#DF0101:JitterSend" 
		#"GPRINT:LossSend:MAX: Max LossSend\:%0.0lf" \
		#"GPRINT:LossRecv:MAX:Max LossRecv\:%0.0lf" \
		#"GPRINT:JitterSend:MAX: Max JitterSend\:%0.0lf" \
		#"GPRINT:JitterRecv:MAX:Max JitterRecv\:%0.0lf" \
		#"GPRINT:LossSend:LAST:Last LossSend\:%0.0lf" \
		#"GPRINT:LossRecv:LAST:Last LossRecv\:%0.0lf" \
		#"GPRINT:JitterSend:LAST:Last JitterSend\:%0.0lf" \
		#"GPRINT:JitterRecv:LAST:Last JitterRecv\:%0.0lf";
if [ $? != 0 ];then
	fLog "rrdtool graph $PNGFILE error";
	exit 0;
fi

PNGFILE="/var/www/unxs/html/traffic/$HOSTNAME-nodeQOS-calls.png"

nice /usr/bin/rrdtool graph $PNGFILE \
		--start "end-8 hours" \
		--title="$HOSTNAME NumCalls" \
		--vertical-label="NumCalls" \
		--height=240 \
		--width=1000 \
		--right-axis 1:0 \
		--slope-mode \
		--font TITLE:10: \
		--font AXIS:6: \
		--font LEGEND:8: \
		--font UNIT:8: \
		"DEF:NumCalls=$RRDFILE:NumCalls:MAX" \
		"LINE1:NumCalls#DF0101:NumCalls" 
if [ $? != 0 ];then
	fLog "rrdtool graph $PNGFILE error";
	exit 0;
fi
