#!/bin/bash

#PURPOSE
#	Setup FreePBX for NAT use
#	This script needs to be FOR EACH CONTAINER
#	The container needs to be setup previously with correct tProperties

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@" >> /tmp/ActivateNATContainer.sh.log; }

if [ "$2" == "" ] ;then
	echo "usage: $0 <uContainer> <cSourceIPv4>";
	fLog "usage error $0 $1 $2 $3";
	exit 1;
fi
fLog "start $1 $2 $3";

/usr/sbin/unxsNATPBX ChangeFreePBX $1 >>/tmp/ActivateNATContainer.sh.log 2>&1;
if [ $? != 0 ] ;then
	fLog "/usr/sbin/unxsNATPBX ChangeFreePBX $1 error";
fi

fLog "end";
