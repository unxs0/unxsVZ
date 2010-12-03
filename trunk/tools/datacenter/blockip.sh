#!/bin/bash

#FILE
#	blockip.sh
#LEGAL
#	Released into the public domain 12/2010

#This is a very simple script for stopping traffic to containers 
#without using iptables.

#It has some minor error checking and accident prevention so
#it can be used via sudo by jr. admins.

function valid_ip()
{
    local  ip=$1
    local  stat=1

    if [[ $ip =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
        OIFS=$IFS
        IFS='.'
        ip=($ip)
        IFS=$OIFS
        [[ ${ip[0]} -le 255 && ${ip[1]} -le 255 \
            && ${ip[2]} -le 255 && ${ip[3]} -le 255 ]]
        stat=$?
    fi
    return $stat
}


if [ "$1" == "" ] || [ "$2" == "" ];then
	echo "usage: $0 <VEID> <ipnumber>";
	exit 0;
fi

if ! valid_ip $2;then
	echo "ipnumber $2 is not a valid dotted quad";
	exit 1;
fi

#IP section
/sbin/ifconfig -a | /bin/grep -w $2 > /dev/null 2>&1;
if [ $? == 0 ];then
	echo "ipnumber $2 in use on hardware node";
	exit 1;
fi

/usr/sbin/vzlist -o ip | /bin/grep $2 > /dev/null 2>&1;
if [ $? == 0 ];then
	echo "ipnumber $2 in use by OpenVZ";
	exit 1;
fi

#VEID section
/usr/sbin/vzlist -o ctid | /bin/grep -w $1 > /dev/null 2>&1;
if [ $? != 0 ];then
	echo "invalid VEID=$1";
	exit 1;
fi

#debug only
#echo "/sbin/ifconfig eth0:$1 $2 up;"
#exit 0;


/sbin/ifconfig eth0:$1 $2 up;
