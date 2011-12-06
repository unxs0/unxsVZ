#!/bin/bash

#
#FILE
#	PBXGatherIncoming.sh
#PURPOSE
#	Gather extension DIDs from incoming table for each vPBX on this node.
#	Gather SIP trunks.
#LEGAL
#	(C) 2011, 2012 Unixservice, LLC.
#	GPLv2 license applies


cHost="node1.someisp.net";
cPasswd="wsxedc";
cGroupStatement="(tGroup.cLabel='Production PBXs' OR tGroup.cLabel='UK PBXs')";
cMySQLConnect="/usr/bin/mysql -h $cHost -u unxsvz -p$cPasswd unxsvz";

if [ "$1" != "run" ];then
	echo "usage: $0 run [<VEID>]";
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
echo "Select containers for $cShortHostname/$uNode";

for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uNode=$uNode AND tContainer.uStatus=1" | $cMySQLConnect | grep -v uContainer`;do

	#Optional VEID 
	if [ "$2" != "" ] ;then

		if [ "$uContainer" != "$2" ];then
			continue;
		fi
	fi

        #echo $uContainer;

	cAMPDBUSER=`/usr/sbin/vzctl exec2 $uContainer '/bin/grep "^AMPDBUSER=" /etc/amportal.conf | tail -n 1 | /bin/cut -f 2 -d ='`;
	if [ $? != 0 ];then
		echo "vzctl exec2 1 failed for $uContainer";
		continue;
	fi
	if [ "$cAMPDBUSER" == "" ];then
		echo "cAMPDBUSER empty for $uContainer";
		continue;
	fi

	cAMPDBPASS=`/usr/sbin/vzctl exec2 $uContainer '/bin/grep "^AMPDBPASS=" /etc/amportal.conf | tail -n 1 | /bin/cut -f 2 -d ='`;
	if [ $? != 0 ];then
		echo "vzctl exec2 2 failed for $uContainer";
		continue;
	fi
	if [ "$cAMPDBPASS" == "" ];then
		echo "cAMPDBPASS empty for $uContainer";
		continue;
	fi

	#
	#Extension
	#

	#Check to see if this PBX has the new schema
	/usr/sbin/vzctl exec2 $uContainer "echo 'SELECT extension FROM incoming LIMIT 1' | /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk" > /dev/null 2>&1;
	if [ $? != 0 ];then
		echo "No incoming table for $uContainer";
		continue;
	fi

	echo "DELETE FROM tProperty WHERE cName='cOrg_Extension' AND uKey=$uContainer AND uType=3 AND uOwner=2 AND uCreatedBy=1" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 1a failed for $uContainer";
		continue;
	fi

	for cQueryResponse in `/usr/sbin/vzctl exec2 $uContainer "echo 'SELECT REPLACE(CONCAT(extension,'\'','\'',destination,'\'','\'',description),'\'' '\'','\'''\'') FROM incoming WHERE ((SUBSTRING(LOWER(extension),1,1) BETWEEN '\''0'\'' AND '\''9'\'') OR SUBSTRING(LOWER(extension),1,1)='\''+'\'') '| /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk" | grep -v CONCAT`;do
        	#echo $cQueryResponse;
		echo "INSERT INTO tProperty SET cName='cOrg_Extension',cValue='$cQueryResponse',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2a failed for $uContainer";
		fi
	done


	#
	#Version
	#

	echo "DELETE FROM tProperty WHERE cName='cOrg_FreePBXVersion' AND uKey=$uContainer AND uType=3 AND uOwner=2 AND uCreatedBy=1" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 1b failed for $uContainer";
		continue;
	fi

	for cQueryResponse in `/usr/sbin/vzctl exec2 $uContainer "echo 'SELECT value FROM admin WHERE variable='\''version'\''' | /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk" | grep -v value`;do
        	#echo $cQueryResponse;
		echo "INSERT INTO tProperty SET cName='cOrg_FreePBXVersion',cValue='$cQueryResponse',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2b failed for $uContainer";
			continue;
		fi
	done

	#
	#SIP Trunk
	#

	echo "DELETE FROM tProperty WHERE cName='cOrg_SIPTrunk' AND uKey=$uContainer AND uType=3 AND uOwner=2 AND uCreatedBy=1" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 1b failed";
		continue;
	fi

	#Check to see if this PBX has the new schema
	/usr/sbin/vzctl exec2 $uContainer "echo 'SELECT name,outcid FROM trunks LIMIT 1' | /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk" > /dev/null 2>&1;
	if [ $? != 0 ];then
		echo "No trunks table for $uContainer";
		continue;
	fi

	for cQueryResponse in `/usr/sbin/vzctl exec2 $uContainer "echo 'SELECT CONCAT(name,'\'','\'',outcid) FROM trunks WHERE tech='\''sip'\''' | /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk" | grep -v CONCAT | sed -e 's/ /-/g'`;do
        	#echo $cQueryResponse;
		echo "INSERT INTO tProperty SET cName='cOrg_SIPTrunk',cValue='$cQueryResponse',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2b failed for $uContainer";
			continue;
		fi
	done

done

