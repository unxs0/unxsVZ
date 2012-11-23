#!/bin/bash

# FILE
#	/usr/sbin/GatherNodePropertyData.sh

#
#PURPOSE
#	Set tProperty values for this node.
#
#	cProcCPUInfo 8*Intel(R)Xeon(R)CPUX3450@2.67GHz
#	cKernel 2.6.18-274.7.1.el5.028stab095.1
#	cIPv4 rc12 (eth1 private cabinet LAN via IP or via /etc/hosts)
#	luInstalledDiskSpace 650237688
#	luInstalledRam 24624700
#	NewContainerMode Active and Clone
#	cIPMIPasswd
#	cIPMIIPv4
#	cRootPasswd
#	(Phrase Passwds should be stored as short acronym
#		 so as to keep the plain text private to non initiates)


cHost="rc1.callingcloud.net";
cPasswd="wsxedc";
cMySQLConnect="/usr/bin/mysql -h $cHost -u unxsvz -p$cPasswd unxsvz";

if [ "$1" != "run" ];then
	echo "usage: $0 run";
	exit 0;
fi

cShortHostname=`hostname -s`;
if [ $? != 0 ];then
	echo "hostname -s failed";
	exit 1;
fi
uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cShortHostname'" | $cMySQLConnect | grep -v uNode`;
if [ $? != 0 ];then
	echo "Select node failed";
	exit 1;
fi
if [ "$uNode" == "" ];then
	echo "uNode failed";
	exit 1;
fi

echo uNode=$uNode;

###
cProcCPU=`grep "model name" /proc/cpuinfo | head -n 1 | cut -f 2 -d : | tr -d ' '`;
uNumCPUs=`grep -c "model name" /proc/cpuinfo`;
cProcCPUInfo="$uNumCPUs*$cProcCPU";

#cleanup
#echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cProcCPUInfo'" | $cMySQLConnect;
#echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cKernel'" | $cMySQLConnect;

uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cProcCPUInfo'" | $cMySQLConnect | grep -v uProperty`;
if [ "$uProperty" == "" ];then
	echo "INSERT INTO tProperty SET cName='cProcCPUInfo',cValue='$cProcCPUInfo',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Insert cProcCPUInfo $cProcCPUInfo;
else
	echo "UPDATE tProperty SET cValue='$cProcCPUInfo',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uNode AND uType=2 AND cName='cProcCPUInfo'" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Update cProcCPUInfo $cProcCPUInfo;
fi
###

###
luInstalledRam=`free | grep Mem: | awk -F' ' '{ printf $2 }'`;

uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='luInstalledRam'" | $cMySQLConnect | grep -v uProperty`;
if [ "$uProperty" == "" ];then
	echo "INSERT INTO tProperty SET cName='luInstalledRam',cValue='$luInstalledRam',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Insert luInstalledRam $luInstalledRam;
else
	echo "UPDATE tProperty SET cValue='$luInstalledRam',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uNode AND uType=2 AND cName='luInstalledRam'" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Update luInstalledRam $luInstalledRam;
fi
###

###
cKernel=`uname -r`;

uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cKernel'" | $cMySQLConnect | grep -v uProperty`;
if [ "$uProperty" == "" ];then
	echo "INSERT INTO tProperty SET cName='cKernel',cValue='$cKernel',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Insert cKernel $cKernel;
else
	echo "UPDATE tProperty SET cValue='$cKernel',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uNode AND uType=2 AND cName='cKernel'" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Update cKernel $cKernel;
fi
###

###
cIPv4=$cShortHostname;

uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cIPv4'" | $cMySQLConnect | grep -v uProperty`;
if [ "$uProperty" == "" ];then
	echo "INSERT INTO tProperty SET cName='cIPv4',cValue='$cIPv4',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Insert cIPv4 $cIPv4;
else
	echo "UPDATE tProperty SET cValue='$cIPv4',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uNode AND uType=2 AND cName='cIPv4'" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Update cIPv4 $cIPv4;
fi
###

###
NewContainerMode="Active and Clone";

uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='NewContainerMode'" | $cMySQLConnect | grep -v uProperty`;
if [ "$uProperty" == "" ];then
	echo "INSERT INTO tProperty SET cName='NewContainerMode',cValue='$NewContainerMode',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Insert NewContainerMode $NewContainerMode;
fi
###

###
luInstalledDiskSpace=`df -k /vz | grep vz | awk -F' ' '{ printf $1 }'`;

uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='luInstalledDiskSpace'" | $cMySQLConnect | grep -v uProperty`;
if [ "$uProperty" == "" ];then
	echo "INSERT INTO tProperty SET cName='luInstalledDiskSpace',cValue='$luInstalledDiskSpace',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Insert luInstalledDiskSpace $luInstalledDiskSpace;
else
	echo "UPDATE tProperty SET cValue='$luInstalledDiskSpace',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uNode AND uType=2 AND cName='luInstalledDiskSpace'" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Update luInstalledDiskSpace $luInstalledDiskSpace;
fi
###

###
cIPMIPasswd="NWGing";

uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cIPMIPasswd'" | $cMySQLConnect | grep -v uProperty`;
if [ "$uProperty" == "" ];then
	echo "INSERT INTO tProperty SET cName='cIPMIPasswd',cValue='$cIPMIPasswd',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Insert cIPMIPasswd $cIPMIPasswd;
fi
###

###
cIPMIIPv4="0.0.0.0";

uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cIPMIIPv4'" | $cMySQLConnect | grep -v uProperty`;
if [ "$uProperty" == "" ];then
	echo "INSERT INTO tProperty SET cName='cIPMIIPv4',cValue='$cIPMIIPv4',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Insert cIPMIIPv4 $cIPMIIPv4;
fi
###

###
cRootPasswd="Unknown";

uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cRootPasswd'" | $cMySQLConnect | grep -v uProperty`;
if [ "$uProperty" == "" ];then
	echo "INSERT INTO tProperty SET cName='cRootPasswd',cValue='$cRootPasswd',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Insert cRootPasswd $cRootPasswd;
fi
###

exit 0;
