#!/bin/bash

#FILE
#	RestartAllAsteriskServers.sh
#PURPOSE
#	Restart containers for this node based on group
#LEGAL
#	(C) 2011, 2012 Unixservice, LLC.
#	GPLv2 license applies

cHost="node1.someisp.net";
cPasswd="wsxedc";
cGroupStatement="(tGroup.cLabel='Production PBXs' OR tGroup.cLabel='UK PBXs')";
cMySQLConnect="/usr/bin/mysql -h $cHost -u unxsvz -p$cPasswd unxsvz";

if [ "$1" != "run" ];then
	echo "usage: $0 run [<VEID>] [setup]";
	exit 0;
fi

cShortHostname=`hostname -s`;
if [ $? != 0 ];then
	echo "hostname -s failed";
	exit;
fi
uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cShortHostname'" | $cMySQLConnect | grep -v uNode`;
if [ $? != 0 ];then
	echo "Select node failed";
	exit;
fi
if [ "$uNode" == "" ];then
	echo "uNode failed";
	exit;
fi

echo "Start" > /var/log/RestartAllAsteriskServers.log 2>&1;

for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uNode=$uNode AND tContainer.uStatus=1" | $cMySQLConnect | grep -v uContainer`;do
	#debug only
	#echo $uContainer;
	echo $uContainer >> /var/log/RestartAllAsteriskServers.log 2>&1;
	#debug only
	#continue;
        /usr/sbin/vzctl stop $uContainer >> /var/log/RestartAllAsteriskServers.log 2>&1;
        if [ $? != 0 ];then
                echo "vzctl stop $uContainer error";
        fi
        sleep 2;
        /usr/sbin/vzctl start $uContainer >> /var/log/RestartAllAsteriskServers.log 2>&1;
        if [ $? != 0 ];then
                echo "vzctl start $uContainer error";
        fi
done
echo "End" >> /var/log/RestartAllAsteriskServers.log 2>&1;
