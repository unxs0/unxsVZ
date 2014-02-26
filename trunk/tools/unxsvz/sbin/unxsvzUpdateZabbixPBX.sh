#!/bin/bash

#FILE
#	/usr/sbin/unxsvzUpdateZabbixPBX.sh
#	source file tools.unxsvz/sbin/unxsvzUpdateZabbixPBX.sh
#PURPOSE
#	Script to update a given PBX container's zabbix configuration
#	including iptables.
#DEPENDENCIES
#	/etc/unxsvz/ config and zabbix files
#	These two files need to be changed to unxsvzFormat scripts ASAP:
#	/usr/sbin/UpdateZabbixSetProxy.sh
#	/usr/sbin/unxsvzDeleteZabbixHost.sh
#	
#AUTHOR
#	(C) 2011-2014 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

#

#required local settings. mysql and iptables
if [ -f "/etc/unxsvz/mysql.local.sh" ];then
	source /etc/unxsvz/mysql.local.sh;
else
	echo "no /etc/unxsvz/mysql.local.sh";
	exit 1;
fi
if [ -f "/etc/unxsvz/iptables.local.sh" ];then
	source /etc/unxsvz/iptables.local.sh;
else
	echo "no /etc/unxsvz/iptables.local.sh";
	exit 1;
fi

#functions

fUpdateIptables() {

	if [ "$@" == "" ];then
		echo "no fUpdateIptables uContainer";
		exit;
	fi
	uContainer="$@";
	#update iptables. add new subnet for ping only once please
	grep -w "$cNewRange" /vz/private/$uContainer/etc/sysconfig/iptables > /dev/null;
	if [ "$?" != "0" ];then
		sed -i -e "s/-A INPUT -p icmp -m icmp -s $cRefRange\/24 -j ACCEPT/-A INPUT -p icmp -m icmp -s $cRefRange\/24 -j ACCEPT\n-A INPUT -p icmp -m icmp -s $cNewRange\/24 -j ACCEPT/g" \
			/vz/private/$uContainer/etc/sysconfig/iptables;
		sed -i -e "s/-A INPUT -p icmp -m icmp -s $cRefRange2\/24 -j ACCEPT/-A INPUT -p icmp -m icmp -s $cRefRange2\/24 -j ACCEPT\n-A INPUT -p icmp -m icmp -s $cNewRange\/24 -j ACCEPT/g" \
			/vz/private/$uContainer/etc/sysconfig/iptables;
	fi

	echo "fUpdateIptables ran";
}



if [ "$1" == "" ] || ( [ "$1" != "install" ] && [ "$1" != "help" ] && [ "$1" != "setproxy" ] && [ "$1" != "iptablesfix" ]);then
	echo "usage: $0 <install|setproxy|help|iptablesfix> [<veid>] [new-container]";
	exit 0;
fi

if [ "$1" == "help" ];then
	echo "This script upgrades current PBX zabbix_agentd to the version in the /etc/unxsvz/zabbix_agentd_upgrade dir";
	cat /etc/unxsvz/zabbix_agentd_upgrade/README;
	cat /etc/unxsvz/mysql.local.sh;
	cat /etc/unxsvz/iptables.local.sh;
	exit 0;
fi

if [ ! -d "/etc/unxsvz/zabbix_agentd_upgrade" ];then
	echo "No /etc/unxsvz/zabbix_agentd_upgrade directory. Aborting.";
	exit 1;
fi

#iptablesfix only debug only
if [ "$1" == "iptablesfix" ] && [ "$2" != "" ];then
	#check local system
	cHostname=`/usr/sbin/vzlist -H -o hostname $2`;
	if [ "$?" != "0" ];then
		echo "vzlist failed for $2.";
		exit;
	fi

	fUpdateIptables $2;
	#restart iptables
	/usr/sbin/vzctl exec $2 "service iptables restart";
	exit;
fi

#use this to run only the json api
cOnlySetProxy="false";
if [ "$1" == "setproxy" ];then
	cOnlySetProxy="true";
fi

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

#running containers only but that is checked via vzlist not via unxsVZ status
for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uNode=$uNode" | $cMySQLConnect | grep -v uContainer`;do
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

	#only iptablesfix but in loop for all uContainer;
	if [ "$1" == "iptablesfix" ];then
		#fix iptables
		fUpdateIptables $uContainer;

		#restart iptables
		/usr/sbin/vzctl exec $uContainer "service iptables restart";

		continue;
	fi

	#we will restart zabbix and this will register again we hope with correct group etc.
	#but only if we remove it here.
	#timing is hard we need to wait for new containers to register.
	if [ "$3" != "new-container" ];then
		/usr/sbin/unxsvzDeleteZabbixHost.sh $cHostname;
	fi


	#fix iptables
	fUpdateIptables $uContainer;


	#
	#zabbix configuration
	#delete server and server active lines
	sed -i -e "/^Server=/d" /vz/private/$uContainer/etc/zabbix/zabbix_agentd.conf;
	sed -i -e "/^ServerActive=/d" /vz/private/$uContainer/etc/zabbix/zabbix_agentd.conf;
	#add the new ones
	#add ServerActive=zabbix-proxy0.callingcloud.net
	echo "ServerActive=$cZabbixActiveServer.$cDomain" >> /vz/private/$uContainer/etc/zabbix/zabbix_agentd.conf;
	echo "adding ServerActive";
	#add Server=zabbix.callingcloud.net,zabbix-proxy0.callingcloud.net
	echo "Server=zabbix.$cDomain,$cZabbixActiveServer.$cDomain" >> /vz/private/$uContainer/etc/zabbix/zabbix_agentd.conf;
	echo "adding Server";

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
	#install/update binary and provide backwords compatibility sym link
	if [ -f /etc/unxsvz/zabbix_agentd_upgrade/zabbix_agentd ];then
		cp /etc/unxsvz/zabbix_agentd_upgrade/zabbix_agentd /vz/private/$uContainer/usr/sbin/zabbix_agentd; 
		if [ "$?" != "0" ];then
			echo "cp failed $uContainer.";
			cRestart="y";
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

	if [ "$cRestart" == "y" ];then
		/usr/sbin/vzctl exec $uContainer "service zabbix_agentd restart";
	else
		/usr/sbin/vzctl exec $uContainer "service zabbix_agentd start";
	fi

	#restart iptables
	/usr/sbin/vzctl exec $uContainer "service iptables restart";

  else

	#only doing setproxy below. provide some feedback.
	echo $cHostname;
  fi


  if [ "$3" != "new-container" ];then
	#now you need to change zabbix to use proxy
	echo "updating zabbix server for new proxy";
	/usr/sbin/UpdateZabbixSetProxy.sh $cHostname $uZabbixId > /dev/null;
  fi
done
