#!/bin/bash

#FILE
#	/usr/sbin/unxsvzInstallZabbixAgent.sh
#	From unxsVZ/unxsScripts/unxsvz/sbin/unxsvzInstallZabbixAgent.sh
#	$Id$
#PURPOSE
#	Install a zabbix agent using 
#		/etc/unxsvz/$cType.zabbix_agentd.conf
#		/etc/unxsvz/$cType.zabbix_agentd.d
#INPUT
#	VEID and agent configuration cType file prefix
#REQUIRES
#	Host node zabbix same arch as container arch
#OS
#	Only tested on CentOS 6 with OpenVZ
#LEGAL
#	Copyright (C) Unixservice, LLC. 2015-2015.
#	GPLv2 license applies.
#

if [ -f "/etc/unxsvz/unxsvz.functions.sh" ];then
	source /etc/unxsvz/unxsvz.functions.sh;
else
	echo "no /etc/unxsvz/unxsvz.functions.sh";
	exit 1;
fi

if [ -f "/etc/unxsvz/zabbix.local.sh" ];then
	source /etc/unxsvz/zabbix.local.sh;
else
	echo "no /etc/unxsvz/zabbix.local.sh";
	exit 1;
fi


if [ "$2" == "" ];then
	echo "usage: $0 <uVeid> <cZabbixConfType>";
	exit 1;
fi

if [ ! -f "/etc/unxsvz/$2.zabbix_agentd.conf" ];then
	echo "No /etc/unxsvz/$2.zabbix_agentd.conf found";
	exit 2;
fi

if [ ! -d "/etc/unxsvz/$2.zabbix_agentd.d" ];then
	echo "No /etc/unxsvz/$2.zabbix_agentd.d dir found";
	exit 2;
fi

if [ ! -d "/vz/root/$1/etc" ];then
	echo "No /vz/root/$1/etc dir found";
	exit 3;
fi

#install conf files
cHostname=`vzlist -H -o hostname $1`;
if [ "$cHostname" == "" ];then
	echo "No vzlist hostname";
	exit 3;
fi

if [ ! -d /vz/root/$1/etc/zabbix ];then
	mkdir /vz/root/$1/etc/zabbix;
	if [ "$?" != "0" ];then
		echo "mkdir /vz/root/$1/etc/zabbix failed";
		exit 4;
	fi
fi
if [ ! -d /vz/root/$1/etc/zabbix/zabbix_agentd.d ];then
	mkdir /vz/root/$1/etc/zabbix/zabbix_agentd.d;
	if [ "$?" != "0" ];then
		echo "mkdir /vz/root/$1/etc/zabbix/zabbix_agentd.d failed";
		exit 5;
	fi
fi

#do not allow upgrade for now
if [ -f "/vz/root/$1/etc/zabbix/zabbix_agentd.conf" ];then
	echo "/vz/root/$1/etc/zabbix/zabbix_agentd.conf already exists aborting";
	exit 6;
fi
cp /etc/unxsvz/$2.zabbix_agentd.conf /vz/root/$1/etc/zabbix/zabbix_agentd.conf;
if [ "$?" != "0" ];then
	echo "cp /etc/unxsvz/$2.zabbix_agentd.conf /vz/root/$1/etc/zabbix failed";
	exit 7;
fi

cp /etc/unxsvz/$2.zabbix_agentd.d/*.conf /vz/root/$1/etc/zabbix/zabbix_agentd.d/;
if [ "$?" != "0" ];then
	echo "cp /etc/unxsvz/$2.zabbix_agentd.d/*.conf /vz/root/$1/etc/zabbix/zabbix_agentd.d/ failed";
	exit 8;
fi

echo "Hostname=$cHostname" >> /vz/root/$1/etc/zabbix/zabbix_agentd.conf;
if [ "$?" != "0" ];then
	echo "configuring /vz/root/$1/etc/zabbix/zabbix_agentd.d/ failed";
	exit 9;
fi
echo "Server=$cZabbixAgentServer" >> /vz/root/$1/etc/zabbix/zabbix_agentd.conf;
if [ "$?" != "0" ];then
	echo "configuring /vz/root/$1/etc/zabbix/zabbix_agentd.d/ failed";
	exit 9;
fi
echo "ServerActive=$cZabbixAgentServerActive" >> /vz/root/$1/etc/zabbix/zabbix_agentd.conf;
if [ "$?" != "0" ];then
	echo "configuring /vz/root/$1/etc/zabbix/zabbix_agentd.d/ failed";
	exit 9;
fi

echo "conf files installed";

#install zabbix binaries from host node dependency on same arch

#configure container boot for zabbix
#install zabbix init file

#start zabbix agent

exit 0;
