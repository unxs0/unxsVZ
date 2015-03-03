#!/bin/bash

#FILE
#	/usr/sbin/unxsvzGetPBXTrunkInfo.sh
#PURPOSE
#	FreePBX family script
#	Gather trunk info including trunk sequence for outbound routes.
#	Update or insert appropiate tProperty entries accordingly.
#NOTES
#	Requires later verion FreePBX setup with no MySQL passwd required for root user
#AUTHOR
#	(C) 2011-2014 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

if [ "$1" != "run" ];then
	echo "usage: $0 run <uVEID>";
	exit 0;
fi

cMySQLConnect="";
if [ -f "/etc/unxsvz/mysql.local.sh" ];then
	source /etc/unxsvz/mysql.local.sh;
else
	echo "no /etc/unxsvz/mysql.local.sh";
	exit 1;
fi
if [ -f "/etc/unxsvz/unxsvz.functions.sh" ];then
	source /etc/unxsvz/unxsvz.functions.sh;
else
	echo "no /etc/unxsvz/unxsvz.functions.sh";
	exit 1;
fi
if [ "$cMySQLConnect" == "" ];then
	fLog "no cMySQLConnect";
	exit 1;
fi

cNode=`hostname -s`;
if [ $? != 0 ];then
	fLog "cNode hostname error";
	exit;
fi
uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cNode'"|$cMySQLConnect -B -N`;
if [ $? != 0 ];then
	fLog "uNode SELECT error";
	exit;
fi
if [ "$uNode" == "" ];then
	fLog "uNode could not be determined for $cNode";
	exit;
fi
fLog "Start for $cNode/$uNode";

for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uNode=$uNode AND tContainer.uStatus=1"|$cMySQLConnect -B -N`;do

	#single veid option
	if [ "$2" != "" ];then
		if [ "$2" != "$uContainer" ];then
			continue;
		fi
	fi

	echo $uContainer;

	#all used trunks where they are used and in what order
	#instead of select and update or insert we just clean out and add again.
	echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cPBXTrunkSeqAndRoute'" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 1 failed";
	fi
        for cResult in `/usr/sbin/vzctl exec2 $uContainer "/usr/bin/mysql asterisk -N -e 'SELECT CONCAT(trunks.name,'\'':'\'',outbound_route_trunks.seq,'\'':'\'',outbound_routes.name) FROM outbound_route_trunks,outbound_routes,trunks WHERE outbound_routes.route_id=outbound_route_trunks.route_id AND trunks.trunkid=outbound_route_trunks.trunk_id'"`;do
		echo $cResult;
		echo "INSERT INTO tProperty SET cName='cPBXTrunkSeqAndRoute',cValue='$cResult',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2 failed";
		fi
	done
done
fLog "End";
