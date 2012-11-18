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
	/usr/bin/rrdtool create $RRDFILE --start N --step 300 \
	DS:LossSend:DERIVE:600:0:10000 \
	DS:LossRecv:DERIVE:600:0:10000 \
	DS:JitterSend:DERIVE:600:0:10000 \
	DS:JitterRecv:DERIVE:600:0:10000 \
	DS:NumCalls:DERIVE:600:0:10000 \
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
		--title="$HOSTNAME QOS" \
		--vertical-label="" \
		--base=1000 \
		--height=120 \
		--width=500 \
		--slope-mode \
		--font TITLE:10: \
		--font AXIS:6: \
		--font LEGEND:8: \
		--font UNIT:8: \
		"DEF:LossSend=$RRDFILE:LossSend:MAX" \
		"DEF:LossRecv=$RRDFILE:LossRecv:MAX" \
		"DEF:JitterSend=$RRDFILE:JitterSend:MAX" \
		"DEF:JitterRecv=$RRDFILE:JitterRecv:MAX" \
		"DEF:NumCalls=$RRDFILE:NumCalls:MAX" \
		"LINE1:LossSend#00ff00:LossSend" \
		"LINE1:LossRecv#0000ff:LossRecv" \
		"LINE1:JitterSend#00a000:JitterSend" \
		"LINE1:JitterRecv#0000a0:JitterRecv" \
		"LINE1:NumCalls#a00000:NumCalls"
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
