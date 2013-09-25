#!/bin/sh
#
#FILE
#	/usr/sbin/GatherNodeInfo.sh
#PURPOSE
#	Add/ipdate tProperty info for hardware node

#
#user conf section
cHost="rc1";
cPasswd="wsxedc";
cMySQLConnect="/usr/bin/mysql -h $cHost -u unxsvz -p$cPasswd unxsvz";
#
#

cShortHostname=`hostname -s`;
cProcCPU=`grep "model name" /proc/cpuinfo | head -n 1 | cut -f 2 -d : | tr -d ' '`;
uNumCPUs=`grep -c "model name" /proc/cpuinfo`;
cProcCPUInfo="$uNumCPUs*$cProcCPU";
luInstalledRAM=`free | grep Mem: | awk -F' ' '{ print $2 }'`;
luInstalledDiskSpace=`df | grep "\/vz" | awk -F' ' '{print $1}'`;
cMACeth0=`ifconfig eth0 | grep HWaddr | awk -F' ' '{print $5}'`;
cMACeth1=`ifconfig eth1 | grep HWaddr | awk -F' ' '{print $5}'`;

if [ "$1" != "run" ];then
	echo "usage: $0 run";
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

#
#cProcCPUInfo
echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cProcCPUInfo'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 1 failed";
fi
echo "INSERT INTO tProperty SET cName='cProcCPUInfo',cValue='$cProcCPUInfo',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 2 failed";
fi
#
#

#
#luInstalledRAM
echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='luInstalledRAM'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 3 failed";
fi
echo "INSERT INTO tProperty SET cName='luInstalledRAM',cValue='$luInstalledRAM',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 4 failed";
fi
#
#

#
#luInstalledDiskSpace
echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='luInstalledDiskSpace'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 5 failed";
fi
echo "INSERT INTO tProperty SET cName='luInstalledDiskSpace',cValue='$luInstalledDiskSpace',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 6 failed";
fi
#
#

#
#cMACeth0
echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cMACeth0'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 7 failed";
fi
echo "INSERT INTO tProperty SET cName='cMACeth0',cValue='$cMACeth0',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 8 failed";
fi
#
#

#
#cMACeth1
echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cMACeth1'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 9 failed";
fi
echo "INSERT INTO tProperty SET cName='cMACeth1',cValue='$cMACeth1',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 10 failed";
fi
#
#
