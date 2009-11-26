#!/bin/bash

#AUTHOR
#	(C) 2009-2010 Gary Wallis for Unixservice, LLC.
#	GPLv2 applies.

#Remote reorganization of eth0 with standard OpenVZ bridge example script.
#DANGER: Should be able to be run via remote ssh into running eth0 of hardware node
#	and still allow remote access again.
#For CentOS 5+ with complete rollback on any failure.
#Simple /tmp/vmbr0.log provided

cBR="vmbr0"; #default OpenVZ bridge name for VETH device bridging
cHostDev="eth0"; #hardware node primary interface device name
cIPv4CIDR="192.168.22.128/24"; #original cHostDev IP with CIDR specified subnet
cIPv4GW="192.168.22.2"; #original gateway

#run in background
{
	/usr/sbin/brctl addbr $cBR > /tmp/$cBR.log 2>&1;
	#if the above fails it might be that it exists so we continue.

	/sbin/ifconfig $cHostDev 0 > /tmp/$cBR.log 2>&1;
	if [ $? != 0 ]; then
		echo "optimistic roll back level 0" > /tmp/$cBR.log 2>&1;
		/usr/sbin/brctl delbr $cBR > /tmp/$cBR.log 2>&1;
		exit 1;
	fi

	/usr/sbin/brctl addif $cBR $cHostDev > /tmp/$cBR.log 2>&1;
	if [ $? != 0 ]; then
		echo "optimistic roll back level 1" > /tmp/$cBR.log 2>&1;
		/sbin/ifconfig $cHostDev $cIPv4CIDR > /tmp/$cBR.log 2>&1;
		/usr/sbin/brctl delbr $cBR > /tmp/$cBR.log 2>&1;
		exit 1;
	fi

	/sbin/ifconfig $cBR $cIPv4CIDR > /tmp/$cBR.log 2>&1;
	if [ $? != 0 ]; then
		echo "optimistic roll back level 2" > /tmp/$cBR.log 2>&1;
		/usr/sbin/brctl delif $cBR $cHostDev > /tmp/$cBR.log 2>&1;
		/sbin/ifconfig $cHostDev $cIPv4CIDR > /tmp/$cBR.log 2>&1;
		/sbin/ip route add default via $cIPv4GW dev $cHostDev > /tmp/$cBR.log 2>&1;
		/usr/sbin/brctl delbr $cBR > /tmp/$cBR.log 2>&1;
		exit 1;
	fi

	/sbin/ip route add default via $cIPv4GW dev $cBR > /tmp/$cBR.log 2>&1;
	if [ $? != 0 ]; then
		echo "optimistic roll back level 3" > /tmp/$cBR.log 2>&1;
		/usr/sbin/brctl delif $cBR $cHostDev > /tmp/$cBR.log 2>&1;
		#down is needed to inactivate bridge
		/sbin/ifconfig $cBR 0 down > /tmp/$cBR.log 2>&1;
		/sbin/ifconfig $cHostDev $cIPv4CIDR > /tmp/$cBR.log 2>&1;
		/sbin/ip route add default via $cIPv4GW dev $cHostDev > /tmp/$cBR.log 2>&1;
		/usr/sbin/brctl delbr $cBR > /tmp/$cBR.log 2>&1;
		exit 1;
	fi
} &
