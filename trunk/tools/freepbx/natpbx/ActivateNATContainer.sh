#!/bin/bash

#FILE
#	/usr/sbin/ActivateNATContainer.sh
#USAGE
#	Runs via jobqueue.c unxsVZ.cgi ProcessJobQueue
#	/usr/sbin/PBXActivateNAT.sh <container VEID>
#PURPOSE
#	Configure PBX container for NAT
#	Configure Asterisk/FreePBX
#		NAT settings and port usage.
#	Configure hardware node iptables
#		Port range DNAT.
#		SIP port DNAT.
#	Resources used
#		Hardware node NAT public IP.
#		Container rfc1918 IP.
#		Subset of ports of public IP.
#		

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@" >> /tmp/ActivateNATContainer.sh.log; }

if [ "$3" == "" ];then
	echo "usage: $0 <container VEID> <public NAT IP> <private NAT network>";
	fLog "usage error $0 $1 $2 $3";
	exit 1;
fi

fLog "start $1 $2 $3";
fLog "end";
