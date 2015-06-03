#!/bin/bash

#very simplistic load everything into switch

/usr/sbin/unxsLoadFromSPS DNSUpdateGW Unxs0 silent;
if [ $? != 0 ];then
	echo "DNSUpdateGW error";
fi
/usr/sbin/unxsLoadFromSPS DNSUpdatePBX Unxs0 silent;
if [ $? != 0 ];then
	echo "DNSUpdatePBX error";
fi
/usr/sbin/unxsLoadFromSPS AddAll Unxs0 silent;
if [ $? != 0 ];then
	echo "AddAll error";
fi
/usr/bin/kill -HUP `cat /var/run/uSIPSwitch.pid`;
if [ $? != 0 ];then
	echo "Load rules error";
fi
