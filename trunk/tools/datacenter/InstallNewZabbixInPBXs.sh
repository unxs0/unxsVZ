#!/bin/bash

#FILE
#      /usr/sbin/InstallNewZabbixInPBXs.sh
#PURPOSE
#      Very simple script to update a given zabbix host's proxy.
#AUTHOR
#      (C) 2011,2013 Gary Wallis for Unixservice, LLC.
#      GPLv3 license applies see root dir LICENSE

#
#this file contains confidential info keep secure!
cNewRange="6.4.5.0";
cRefRange="19.20.11.0";
cDomain="isp.net";
cMySQLConnect="/usr/bin/mysql -h server -u unxsvz -pwsxedc unxsvz";
#

if [ "$1" == "" ] || ( [ "$1" != "install" ] && [ "$1" != "help" ] && [ "$1" != "setproxy" ] );then
	echo "usage: $0 <install|setproxy|help> [<veid>]";
	exit 0;
fi

if [ "$1" == "help" ];then
	echo "This script upgrades current PBX zabbix_agentd to the version in the /root/zabbix_agentd_upgrade dir";
	cat /root/zabbix_agentd_upgrade/README;
	exit 0;
fi

if [ ! -d "/root/zabbix_agentd_upgrade" ];then
	echo "No /root/zabbix_agentd_upgrade directory. Aborting.";
	exit 1;
fi

#use this to run only the json api
cOnlySetProxy="false";
if [ "$1" == "setproxy" ];then
	cOnlySetProxy="true";
fi

cGroup="Production PBXs";

cNode=`hostname -s`;
if [ $? != 0 ];then
	echo "cNode hostname error";
	exit 2;
fi
uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cNode'"|$cMySQLConnect|grep -v uNode`;
if [ $? != 0 ];then
	echo "uNode SELECT error";
	exit 3;
fi
if [ "$uNode" == "" ];then
	echo "uNode could not be determined for $cNode";
	exit 4;
fi

#running containers only
for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE tGroup.cLabel='$cGroup' AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uNode=$uNode AND tContainer.uStatus=1" | $cMySQLConnect | grep -v uContainer`;do

	#run for only one option
	if [ "$2" != "" ] && [ "$uContainer" != "$2" ];then
		continue;
	fi

	#check local system
	cHostname=`/usr/sbin/vzlist -H -o hostname $uContainer`;
	if [ "$?" != "0" ];then
		echo "vzlist failed for $uContainer. next.";
		continue;
	fi

  if [ "$cOnlySetProxy" != "true" ];then

	echo $uContainer:$cHostname;

	#install/update binary and provide backwords compatibility sym link
	if [ -f /root/zabbix_agentd_upgrade/zabbix_agentd ];then
		cp /root/zabbix_agentd_upgrade/zabbix_agentd /vz/private/$uContainer/usr/sbin/zabbix_agentd; 
		if [ "$?" != "0" ];then
			echo "cp failed $uContainer.";
			continue;
		fi
		if [ ! -f /vz/private/$uContainer/usr/local/sbin/zabbix_agentd.old ];then
			mv -i /vz/private/$uContainer/usr/local/sbin/zabbix_agentd /vz/private/$uContainer/usr/local/sbin/zabbix_agentd.old;
			cd /vz/private/$uContainer/usr/local/sbin/;
			ln -s /usr/sbin/zabbix_agentd zabbix_agentd;
			if [ "$?" != "0" ];then
				echo "ln -s failed";
			fi
		fi
	fi

	#update /etc/zabbix/ files only once please
	sed -i -e "s/Server=zabbix.$cDomain\$/Server=zabbix.$cDomain,zabbix-proxy0.$cDomain/g" \
			/vz/private/$uContainer/etc/zabbix/zabbix_agentd.conf;

	#update iptables. add new subnet for ping only once please
	grep -w "$cNewRange" /vz/private/$uContainer/etc/sysconfig/iptables > /dev/null;
	if [ "$?" != "0" ];then
		sed -i -e "s/-A INPUT -p icmp -m icmp -s $cRefRange\/24 -j ACCEPT/-A INPUT -p icmp -m icmp -s $cRefRange\/24 -j ACCEPT\n-A INPUT -p icmp -m icmp -s $cNewRange\/24 -j ACCEPT/g" \
			/vz/private/$uContainer/etc/sysconfig/iptables;
	fi

	#restart zabbix-agent /tmp/zabbix_agentd.pid
	/usr/sbin/vzctl exec2 $uContainer 'kill `cat /tmp/zabbix_agentd.pid`';
	if [ "$?" != "0" ];then
		echo "could not kill zabbix_agentd w/pid file, trying killall";
		sleep 1;
		/usr/sbin/vzctl exec2 $uContainer 'killall zabbix_agentd';
		if [ "$?" != "0" ];then
			echo "could not kill or killall zabbix_agentd";
		fi
	fi;
	sleep 1;
	/usr/sbin/vzctl exec $uContainer "service zabbix_agentd start";

	#restart iptables
	/usr/sbin/vzctl exec $uContainer "service iptables restart";

  else

	#only doing setproxy below. provide some feedback.
	echo $cHostname;
  fi
	#now you need to change zabbix to use proxy
	/usr/sbin/UpdateZabbixSetProxy.sh $cHostname 14124 > /dev/null;

done
