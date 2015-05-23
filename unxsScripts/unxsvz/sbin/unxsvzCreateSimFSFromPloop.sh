#!/bin/bash

#FILE
#	/usr/sbin/unxsvzCreateSimFSFromPloop.sh
#PURPOSE
#	create a stopped simfs container from a running ploop container
#	just a KISS sample script for you to improve on.
#DEPENDENCIES
#	jobqueue.c uses the 2M (2000000) VEID base for the tmp
#	simfs VEID to create a usable ostemplate from.
#

#cRsync_options='-aHAX --progress --stats --numeric-ids --delete'
cRsync_options='-aHAX --stats --numeric-ids --delete'
uPrefix='2000';

if [ "$1" == "" ];then
	echo "usage: $0 <VEID>";
	exit 0;
fi

if [ ! -e /etc/vz/conf/$1.conf ]; then
        echo "Virtual server configuration file: /etc/vz/conf/$1.conf does not exist."
        exit 1
fi
if [ ! -d /vz/private/$1/root.hdd ]; then
        echo "Server does not have a ploop device"
        exit 1
fi
if [ ! -d /vz/private/$1 ]; then
        echo "Server does not exist"
        exit 1
fi

mkdir -p /vz/private/$uPrefix$1/

cp /etc/vz/conf/$1.conf /etc/vz/conf/$uPrefix$1.conf
if [ $? != 0 ];then
	echo "cp conf error";
	exit 1;
fi

/usr/sbin/vzctl mount $uPrefix$1
if [ $? != 0 ];then
	echo "/usr/sbin/vzctl mount $uPrefix$1 error";
	exit 1;
fi

rsync $cRsync_options /vz/root/$1/. /vz/private/$uPrefix$1/ 2>/dev/null;
cResult=$?;
if [ "$cResult" == "24" ] || [ "$cResult" == "0" ];then
	echo "rsync ok";
else
	echo $cResult;
	echo "first rsync $cRsync_options /vz/root/$1/. /vz/private/$uPrefix$1/ error";
	exit 1;
fi

/usr/sbin/vzctl stop $1
if [ $? != 0 ];then
	echo "/usr/sbin/vzctl stop $1 error";
	exit 1;
fi
/usr/sbin/vzctl mount $1
if [ $? != 0 ];then
	echo "/usr/sbin/vzctl mount $1 error";
	exit 1;
fi
rsync $cRsync_options /vz/root/$1/. /vz/private/$uPrefix$1/
if [ $? != 0 ];then
	echo "final rsync $cRsync_options /vz/root/$1/. /vz/private/$uPrefix$1/  error";
	exit 1;
fi

#restart source container
/usr/sbin/vzctl start $1
if [ $? != 0 ];then
	echo "/usr/sbin/vzctl start $1 error";
	exit 1;
fi

/usr/sbin/vzctl umount $uPrefix$1
if [ $? != 0 ];then
	echo "/usr/sbin/vzctl umount $uPrefix$1 error";
	exit 1;
fi

/usr/sbin/vzctl set $uPrefix$1 --ipdel all --save
if [ $? != 0 ];then
	echo "/usr/sbin/vzctl set $uPrefix$1 --ipdel all --save error";
	exit 1;
fi
/usr/sbin/vzctl set $uPrefix$1 --ipadd 10.99.99.99 --save
if [ $? != 0 ];then
	echo "/usr/sbin/vzctl set $uPrefix$1 --ipadd 10.99.99.99 --save error";
	exit 1;
fi
exit 0;
