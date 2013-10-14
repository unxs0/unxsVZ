#!/bin/bash

#FILE
#      /usr/sbin/InstallNewZabbixInPBXs.sh
#PURPOSE
#      Very simple script to update a given zabbix host's proxy.
#AUTHOR
#      (C) 2011,2013 Gary Wallis for Unixservice, LLC.
#      GPLv3 license applies see root dir LICENSE

#use this to run only the json api
#cOnlySetProxy="true";

if [ "$1" == "" ] || ( [ "$1" != "install" ] && [ "$1" != "help" ] );then
	echo "usage: $0 <install|help> [<veid>]";
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


cGroup="Production PBXs";

cNode=`hostname -s`;
if [ $? != 0 ];then
	echo "cNode hostname error";
	exit 2;
fi
uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cNode'"|/usr/bin/mysql -h rc1 -u unxsvz -pwsxedc unxsvz|grep -v uNode`;
if [ $? != 0 ];then
	echo "uNode SELECT error";
	exit 3;
fi
if [ "$uNode" == "" ];then
	echo "uNode could not be determined for $cNode";
	exit 4;
fi

#stopped or running containers
for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE tGroup.cLabel='$cGroup' AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uNode=$uNode AND (tContainer.uStatus=1 OR tContainer.uStatus=31)" | /usr/bin/mysql -h rc1 -u unxsvz -pwsxedc unxsvz | grep -v uContainer`;do

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
	sed -i -e 's/Server=zabbix.isp.net$/Server=zabbix.isp.net,zabbix-proxy0.isp.net/g' \
			/vz/private/$uContainer/etc/zabbix/zabbix_agentd.conf;

	#update iptables. add new subnet for ping only once please
	grep -w "6.4.5.0" /vz/private/$uContainer/etc/sysconfig/iptables > /dev/null;
	if [ "$?" != "0" ];then
		sed -i -e 's/-A INPUT -p icmp -m icmp -s 19.20.11.0\/24 -j ACCEPT/-A INPUT -p icmp -m icmp -s 19.20.11.0\/24 -j ACCEPT\n-A INPUT -p icmp -m icmp -s 6.4.5.0\/24 -j ACCEPT/g' \
			/vz/private/$uContainer/etc/sysconfig/iptables;
	fi

	#restart zabbix-agent /tmp/zabbix_agentd.pid
	/usr/sbin/vzctl exec $uContainer 'kill `cat /tmp/zabbix_agentd.pid`;sleep 1';
	/usr/sbin/vzctl exec $uContainer "service zabbix_agentd start";

	#restart iptables
	/usr/sbin/vzctl exec $uContainer "service iptables restart";

  else

	#now you need to change zabbix to use proxy
	echo $cHostname;
	/usr/sbin/UpdateZabbixSetProxy.sh $cHostname 14124;
  fi

done
