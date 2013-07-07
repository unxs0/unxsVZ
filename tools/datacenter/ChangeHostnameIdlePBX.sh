#/bin/bash

if [ "$2" == "" ];then
	echo "usage: $0 <cHostname> <uContainer> [<cPrevHostname>]";
	exit;
fi

echo "cHostname=$1 uContainer=$2 cPrevHostname=$3" >> /tmp/ChangeHostnameIdlePBX.sh.log 2>&1;

#Auto-registration with Zabbix
/usr/sbin/vzctl exec2 $2 'service zabbix_agentd change-hostname' >> /tmp/ChangeHostnameIdlePBX.sh.log 2>&1;
/usr/sbin/vzctl exec2 $2 'service zabbix_agentd restart' >> /tmp/ChangeHostnameIdlePBX.sh.log 2>&1;

#Other basic chores for new PBXs
/usr/sbin/ResetAllPBXAdminPasswds.sh run $2 >> /tmp/ChangeHostnameIdlePBX.sh.log 2>&1;
/usr/sbin/GetResetSSHInfo.sh run $2 >> /tmp/ChangeHostnameIdlePBX.sh.log 2>&1;
/usr/sbin/SetTemplateVersion.sh run $2 >> /tmp/ChangeHostnameIdlePBX.sh.log 2>&1;

#Remove old zabbix monitored host
if [ "$3" != "" ];then
	/usr/sbin/DeleteZabbixHost.sh $3 >> /tmp/ChangeHostnameIdlePBX.sh.log 2>&1;
	#/usr/sbin/UpdateZabbixHostPort.sh <cHost> <uPort>
	uPort=`/usr/sbin/unxsNATPBX GetZabbixPort $1`;
	echo uPort=$uPort >> /tmp/ChangeHostnameIdlePBX.sh.log;
	/usr/sbin/UpdateZabbixHostPort.sh $1 $uPort >> /tmp/ChangeHostnameIdlePBX.sh.log 2>&1;
fi
