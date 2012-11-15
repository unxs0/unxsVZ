#!/bin/bash

if [ "$1" != "run" ];then
	echo "usage: $0 run <ssh command> <pbx label>";
	exit 0;
fi

cPasswds=`$2 /usr/sbin/ResetCPEPBXPasswds.sh run 101`;

cPasswdArray=( $cPasswds );
cAdminPasswd=${cPasswdArray[0]};
cEngPasswd=${cPasswdArray[1]};
cOpPasswd=${cPasswdArray[2]};
cSSHPasswd=${cPasswdArray[3]};

cHost="rc1";
cPasswd="wsxedc";
cMySQLConnect="/usr/bin/mysql -h $cHost -u unxsvz -p$cPasswd unxsvz";
cGroupStatement="(tGroup.cLabel='Appliance PBXs')";

uContainer=`echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.cLabel=\"$3\" AND tContainer.uStatus=101" | $cMySQLConnect | grep -v uContainer`;
if [ $? != 0 ];then
	echo "Select container failed";
	exit;
fi

echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXAdminPasswd'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 1 failed";
fi
	echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXEngPasswd'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 2 failed";
fi
	echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXOperatorPasswd'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 3 failed";
fi
echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cPasswd'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 4b failed";
fi

echo "INSERT INTO tProperty SET cName='cOrg_FreePBXAdminPasswd',cValue='$cAdminPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 5 failed";
fi

echo "INSERT INTO tProperty SET cName='cOrg_FreePBXEngPasswd',cValue='$cEngPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 6 failed";
fi

echo "INSERT INTO tProperty SET cName='cPasswd',cValue='$cSSHPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 6b failed";
fi

echo "INSERT INTO tProperty SET cName='cOrg_FreePBXOperatorPasswd',cValue='$cOpPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 7 failed";
fi
