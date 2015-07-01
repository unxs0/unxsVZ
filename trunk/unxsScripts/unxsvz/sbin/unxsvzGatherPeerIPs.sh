#!/bin/bash

#FILE
#	/usr/sbin/unxsvzGatherPeerIPs.sh
#PURPOSE
#	FreePBX family script
#	Gather Asterisk peer IP Class Cs of numeric extensions only
#NOTES
#AUTHOR
#	(C) 2015 Gary Wallis for Unixservice, LLC.
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
	cHostname=`/usr/sbin/vzlist -H $uContainer -o hostname`;
	echo $cHostname;

	for cIPClassC in `/usr/sbin/vzctl exec $uContainer "asterisk -r -x 'sip show peers'| grep -w OK" | grep "^[0-9]" |awk '{ print $2 }' | cut -f 1,2,3 -d '.' | sort -u`;do
		uIP=`$cMySQLConnect -B -N -e "SELECT uIP FROM tIP WHERE cLabel='$cIPClassC.0' AND uIPType=1 AND uDatacenter=41 LIMIT 1"`;
		if [ "$uIP" == "" ];then
			echo add $cIPClassC.0;
			$cMySQLConnect -B -N -e \
				"INSERT INTO tIP SET cLabel='$cIPClassC.0',uIPType=1,cComment='unxsvzGatherPeerIPs.sh $cHostname $uContainer',uDatacenter=41,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())";
			if [ $? != 0 ];then
				fLog "INSERT INTO tIP error";
			fi
		else
			echo $cIPClassC.0 already added as tIP.uIP=$uIP;
			$cMySQLConnect -B -N -e \
				"UPDATE tIP SET uModDate=UNIX_TIMESTAMP(NOW()),cComment='unxsvzGatherPeerIPs.sh $cHostname $uContainer' WHERE uIP=$uIP";
			if [ $? != 0 ];then
				fLog "UPDATE tIP error";
			fi
		fi
	done

done
fLog "End";
