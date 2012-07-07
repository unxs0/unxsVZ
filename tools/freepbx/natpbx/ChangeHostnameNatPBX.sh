#/bin/bash

#FILE
#	/usr/sbin/ChangeHostnameNatPBX.sh

#NatPBX is a special type of container that is built in a single group and once all in
#	the group have been started. A special group install script is run. This
#	script uses the unxsNATPBX commmand mostly.
#	Since the group install script handles many functions on name change all we need to do
#	is monitoring and dns changes. These can be divided in to two groups: Cleanup (delete
#	entries for previous hostname) and the provisioning/registration of new items
#	like new DNS SRV RRs for the new hostname.

echo "cHostname=$1 uContainer=$2 cPrevHostname=$3" >> /tmp/ChangeHostnameNatPBX.sh.log 2>&1;

#Auto-registration with Zabbix
/usr/sbin/vzctl exec2 $2 'service zabbix_agentd change-hostname' >> /tmp/ChangeHostnameNatPBX.sh.log 2>&1;
/usr/sbin/vzctl exec2 $2 'service zabbix_agentd restart' >> /tmp/ChangeHostnameNatPBX.sh.log 2>&1;

#Remove old zabbix monitored host
if [ "$3" != "" ];then
	/usr/sbin/DeleteZabbixHost.sh $3 >> /tmp/ChangeHostnameNatPBX.sh.log 2>&1;
fi

#A record is added by unxsVZ
#we need to add any required extra DNS RRs like in this case SRV record(s)
/usr/sbin/unxsNATPBX AddSIPSRV $2  >> /tmp/ChangeHostnameNatPBX.sh.log 2>&1;
#cleanup DNS
/usr/sbin/unxsNATPBX DelSIPSRV $3  >> /tmp/ChangeHostnameNatPBX.sh.log 2>&1;
