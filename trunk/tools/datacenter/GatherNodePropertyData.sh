#!/bin/bash

# FILE
#	/usr/sbin/GatherNodePropertyData.sh

#PURPOSE
#	Set tProperty values for this node
#	CPU
#	RAM
#	Kernel
#	Disk


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

echo $uNode;

###
cProcCPU=`grep "model name" /proc/cpuinfo | head -n 1 | cut -f 2 -d : | tr -d ' '`;
uNumCPUs=`grep -c "model name" /proc/cpuinfo`;
cProcCPUInfo="$uNumCPUs*$cProcCPU";

#cleanup
#echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cProcCPUInfo'" | $cMySQLConnect;

uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cProcCPUInfo'" | $cMySQLConnect | grep -v uProperty`;
if [ "$uProperty" == "" ];then
	echo "INSERT INTO tProperty SET cName='cProcCPUInfo',cValue='$cProcCPUInfo',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Insert $cProcCPUInfo;
else
	echo "UPDATE tProperty SET cValue='$cProcCPUInfo',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uNode AND uType=2 AND cName='cProcCPUInfo'" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Update $cProcCPUInfo;
fi
###

###
uInstalledRAM=`free | grep Mem: | awk -F' ' '{ print $2 }'`;

uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='uInstalledRAM'" | $cMySQLConnect | grep -v uProperty`;
if [ "$uProperty" == "" ];then
	echo "INSERT INTO tProperty SET cName='uInstalledRAM',cValue='$uInstalledRAM',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Insert $uInstalledRAM;
else
	echo "UPDATE tProperty SET cValue='$uInstalledRAM',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uNode AND uType=2 AND cName='uInstalledRAM'" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Update $uInstalledRAM;
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
	echo Insert $cKernel;
else
	echo "UPDATE tProperty SET cValue='$cKernel',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uNode AND uType=2 AND cName='cKernel'" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 2 failed";
	fi
	echo Update $cKernel;
fi
###

exit 0;
