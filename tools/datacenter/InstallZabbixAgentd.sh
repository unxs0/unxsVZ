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
#

if [ "$1" != "install" ];then
	echo "usage: $0 install [<hostname>]";
	exit 0;
fi

#      CTID      NPROC STATUS  IP_ADDR         HOSTNAME
#       181         48 running 174.34.136.100 ns1.singtone.com
/usr/sbin/vzlist | grep running | \
while read cvzlist
do
        uContainer=`echo $cvzlist | awk -F' ' '{print $1}'`;
        cHostname=`echo $cvzlist | awk -F' ' '{print $5}'`;

	if [ "$2" != "" ];then
		if [[ ! "$cHostname" =~ $2 ]];then
			continue;
		fi
	fi

        echo $uContainer:$cHostname;

	#from outside

	#install agent binary
	cp /usr/local/sbin/zabbix_agentd\
		 /vz/private/$uContainer/usr/local/sbin/zabbix_agentd < /dev/tty;
	#install init.d script
	cp /etc/init.d/zabbix_agentd\
		 /vz/private/$uContainer/etc/init.d/zabbix_agentd < /dev/tty;
	#create /etc/zabbix dir
	mkdir -p /vz/private/$uContainer/etc/zabbix < /dev/tty;
	#create zabbix agent conf file
	cp ./zabbix_agentd.conf /vz/private/$uContainer/etc/zabbix/ < /dev/tty;
	echo "Hostname=$cHostname" >> /vz/private/$uContainer/etc/zabbix/zabbix_agentd.conf;

	#from inside	

	#create zabbix user and group
	#set perms
	#start agent
        /usr/sbin/vzctl exec2 $uContainer 'groupadd zabbix;\
						useradd -g zabbix -s /sbin/nologin zabbix;\
						chown zabbix.zabbix /etc/zabbix/;\
						touch /var/log/zabbix_agentd.log;\
						chown zabbix.zabbix /var/log/zabbix_agentd.log;\
						/etc/init.d/zabbix_agentd start;\
						/sbin/chkconfig --level 3 zabbix_agentd on; ' < /dev/tty;

done

exit 0;
