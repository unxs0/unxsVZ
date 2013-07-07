#!/bin/bash

#PURPOSE
#	Set cOrg_TemplateVersion in tProperty for given PBX container


cTemplateVersion="2.4.0";



cHost="node1.isp.net";
cPasswd="wsxedc";
cGroupStatement="(tGroup.cLabel='Production PBXs' OR tGroup.cLabel='Idle PBXs')";
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

if [ "$1" != "run" ];then
	echo "usage: $0 run [<VEID>]";
	exit 0;
fi

for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uNode=$uNode AND tContainer.uStatus=1" | $cMySQLConnect | grep -v uContainer`;do

	#Optional VEID and setup
	if [ "$2" != "" ];then

		if [ "$uContainer" != "$2" ];then
			continue;
		fi
	fi

	cTemplateVersion=`/usr/sbin/vzlist -H -o ostemplate $uContainer`;
	if [ $? != 0 ];then
		cTemplateVersion="unknown";
	fi

        #echo $uContainer $cTemplateVersion;

	#cOrg_TemplateVersion
	uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_TemplateVersion'" | $cMySQLConnect | grep -v uProperty`;
	if [ "$uProperty" == "" ];then
		echo "INSERT INTO tProperty SET cName='cOrg_TemplateVersion',cValue='$cTemplateVersion',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2 failed";
		fi
	else
		echo "UPDATE tProperty SET cValue='$cTemplateVersion',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_TemplateVersion'" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2 failed";
		fi
	fi

	echo $cTemplateVersion set for $uContainer;

done

