#!/bin/bash
#
#FILE
#	/usr/sbin/container-rsync.sh
#	$Id$
#
#PURPOSE
#	Test alpha dev script for unxsVZ hot spare container sync
#	Keep a container on this host rsync'd with a source container on a remote host
#
#AUTHOR/LEGAL
#	(C) 2009- Gary Wallis for Unixservice. GPLv2 Licensed.
#
#

if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ];then
	echo "usage $0 <local veid> <remote veid> <remote-host>";
	exit 0;
fi

ssh $3 "ls /vz/private/$2" > /dev/null 2>&1;
if [ $? != 0 ]; then
	echo "no $3 /vz/private/$2";
	exit 1;
fi

ls /vz/private/$1 > /dev/null 2>&1;
if [ $? != 0 ]; then
	echo "no local /vz/private/$1";
	exit 1;
fi

#/usr/bin/rsync -avxlH --dry-run\
/usr/bin/rsync -avxlH \
		 --delete \
		--exclude "/proc" --exclude "/sys" --exclude "/dev" --exclude "/tmp" \
		--exclude "/var/run" --exclude "/etc/sysconfig/network/" --exclude "/etc/sysconfig/network-scripts/" \
		--exclude "/var/lock" --exclude "etc/hosts" --exclude "etc/resolv.conf" \
		--exclude "/var/lib/mlocate/mlocate.db" --exclude "/etc/rc.d/rc6.d/S00vzreboot" \
		--exclude "/var/log/" --exclude "/var/lib/random-seed" --exclude "/var/spool/" \
		--exclude "/etc/aliases.db" \
		-e 'ssh -c blowfish -ax' \
		$3:/vz/private/$2/* \
		/vz/private/$1
