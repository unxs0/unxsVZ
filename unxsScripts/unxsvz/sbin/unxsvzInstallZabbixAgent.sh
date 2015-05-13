#!/bin/bash

#FILE
#	/usr/sbin/unxsvzInstallZabbixAgent.sh
#	From unxsVZ/unxsScripts/unxsvz/sbin/unxsvzInstallZabbixAgent.sh
#	$Id$
#PURPOSE
#	Install a simple zabbix agent using 
#		/etc/unxsvz/$cType.zabbix_agentd.conf
#		/etc/unxsvz/$cType.zabbix_agentd.d
#		/etc/unxsvz/zabbix.local.sh
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
cp /usr/sbin/zabbix_agentd /vz/root/$1/usr/sbin/zabbix_agentd;
if [ "$?" != "0" ];then
	echo "cp /usr/sbin/zabbix_agentd /vz/root/$1/usr/sbin/zabbix_agentd failed";
	exit 10;
fi
cp /etc/init.d/zabbix-agent /vz/root/$1/etc/init.d/zabbix-agent;
if [ "$?" != "0" ];then
	echo "cp /etc/init.d/zabbix-agent /vz/root/$1/etc/init.d/zabbix-agent failed";
	exit 11;
fi

echo "other files installed";

vzctl exec2 $1 "chkconfig --level 3 zabbix-agent on";
if [ "$?" != "0" ];then
	echo "chkconfig --level 3 zabbix-agent on failed";
	exit 12;
fi
vzctl exec2 $1 "useradd -s /sbin/nologin zabbix";
if [ "$?" != "0" ];then
	echo "useradd -s /sbin/nologin zabbix failed";
fi
vzctl exec2 $1 "mkdir /var/run/zabbix;chown -R zabbix:zabbix /var/run/zabbix;mkdir /var/log/zabbix;touch /var/log/zabbix/zabbix_agentd.log;chown -R zabbix:zabbix /var/log/zabbix";
if [ "$?" != "0" ];then
	echo "log items failed";
fi
vzctl exec2 $1 "service zabbix-agent start";
if [ "$?" != "0" ];then
	echo "service zabbix-agent start failed";
	exit 14;
fi
echo "zabbix_agentd started";


exit 0;
