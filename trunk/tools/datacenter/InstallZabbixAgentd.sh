#!/bin/bash
#FILE
#       InstallZabbixAgentd.sh
#PURPOSE
#       Using vzlist install and start zabbix agent in a single or all running
#	containers.
#AUTHOR
#       (C) 2010 Gary Wallis for Unixservice, LLC.
#       GPLv2 license applies. See LICENSE file.
#
#NOTES
#
#TODO
#	Might be nice to have an option to not install unless a valid DNS A record
#	exists (resolves) and is the same as the vzlist IP_ADDR.
#
#	Would also be good to not reinstall if agent is already installed.
#	Maybe only insure it is running in that case?
#

if [ "$1" != "install" ];then
	echo "usage: $0 install [<hostname>]";
	exit 0;
fi

if [ ! -f /usr/local/sbin/zabbix_agentd ];then
	echo "/usr/local/sbin/zabbix_agentd not installed";
	exit 0;
fi

if [ ! -f ./zabbix_agentd.init.d.script ];then
	echo "./zabbix_agentd.init.d.script not installed";
	exit 0;
fi

if [ ! -f ./zabbix_agentd.conf ];then
	echo "./zabbix_agentd.conf not installed";
	exit 0;
fi

#      CTID      NPROC STATUS  IP_ADDR         HOSTNAME
#       181         48 running 174.34.136.100 ns1.singtone.com
/usr/sbin/vzlist | grep running | \
while read cvzlist
do
        uContainer=`echo $cvzlist | awk -F' ' '{print $1}'`;
        cHostname=`echo $cvzlist | awk -F' ' '{print $5}'`;
        cIPAddr=`echo $cvzlist | awk -F' ' '{print $4}'`;
	cPrevInstalled="n";

	if [ "$2" != "" ];then
		if [[ ! "$cHostname" =~ $2 ]];then
			continue;
		fi
	fi

        echo $uContainer:$cHostname;
	cARecord=`/usr/bin/dig $cHostname A +short`;
	if [ $? != 0 ];then
        	echo "dig failed for $cHostname";
		continue;
	fi
	if [ "$cIPAddr" != "$cARecord" ];then
		echo "$cARecord does not match $cIPAddr";
		continue;
	fi

	#debug only
	#echo "$cARecord matches $cIPAddr";
	#continue;
	#just in case ;)
	#exit;

	#from outside

	#if previously installed
	if [ -f /vz/private/$uContainer/etc/zabbix/zabbix_agentd.conf ];then
		cPrevInstalled="y";
	fi

	#install agent binary if not there (it is probably running.)
	if [ ! -f /vz/private/$uContainer/usr/local/sbin/zabbix_agentd ];then
		cp /usr/local/sbin/zabbix_agentd\
			 /vz/private/$uContainer/usr/local/sbin/zabbix_agentd < /dev/tty;
	fi

	#install init.d script always (upgrade works here.)
	cp zabbix_agentd.init.d.script\
		 /vz/private/$uContainer/etc/init.d/zabbix_agentd < /dev/tty;
	#create /etc/zabbix dir
	mkdir -p /vz/private/$uContainer/etc/zabbix < /dev/tty;
	#create zabbix agent conf file (upgrade works here.)
	cp ./zabbix_agentd.conf /vz/private/$uContainer/etc/zabbix/ < /dev/tty;
	echo "Hostname=$cHostname" >> /vz/private/$uContainer/etc/zabbix/zabbix_agentd.conf;

	#from inside	

	#create zabbix user and group
	#set perms
	#start agent
	if [ "$cPrevInstalled" == "n" ];then
	        /usr/sbin/vzctl exec2 $uContainer 'groupadd zabbix;\
						useradd -g zabbix -s /sbin/nologin zabbix;\
						chown zabbix.zabbix /etc/zabbix/;\
						mkdir /var/log/zabbix;\
						touch /var/log/zabbix/zabbix_agentd.log;\
						chown -R zabbix.zabbix /var/log/zabbix;\
						/etc/init.d/zabbix_agentd start;\
						/sbin/chkconfig --level 3 zabbix_agentd on; ' < /dev/tty;
	else
		#we assume that the first install was done correctly DANGER
		#	and that this is just an update like after a hostname change etc.
		#	or we install a better conf or init.
		#	New agentd case is not covered in this simplistic script.
	        /usr/sbin/vzctl exec2 $uContainer '/etc/init.d/zabbix_agentd stop;\
						sleep 1;\
						/etc/init.d/zabbix_agentd start; ' < /dev/tty;
	fi

done

exit 0;
