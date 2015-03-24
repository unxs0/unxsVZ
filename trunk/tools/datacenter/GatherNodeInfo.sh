#!/bin/sh
#
#FILE
#	/usr/sbin/GatherNodeInfo.sh
#PURPOSE
#	Add/ipdate tProperty info for hardware node

#
#user conf section
cHost="64.71.154.153";
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
cIPv4eth0=`ifconfig eth0 | grep -w inet | awk -F':' '{print $2}'|cut -f 1 -d' '`;
cIPv4eth1=`ifconfig eth1 | grep -w inet | awk -F':' '{print $2}'|cut -f 1 -d' '`;
cKernel=`uname -r`;
cdmiSytemManufacturer=`/usr/sbin/dmidecode -s system-manufacturer`;
cdmiSytemProductName=`/usr/sbin/dmidecode -s system-product-name`;

if [ "$1" != "run" ];then
	echo "usage: $0 run";
	exit 0;
fi

cHostname=`hostname -f`;
if [ $? != 0 ];then
	echo "hostname -f failed";
	exit;
fi
cShortHostname=`hostname -s`;
if [ $? != 0 ];then
	echo "hostname -s failed";
	exit;
fi
uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cShortHostname'" | $cMySQLConnect | grep -v uNode`;
if [ $? != 0 ];then
	echo "Select node failed $cShortHostname/$cHostname. Trying registration via IP";
	uDatacenter=`echo "SELECT uDatacenter FROM tIP WHERE cLabel='$cIPv4eth0'" | $cMySQLConnect | grep -v uDatacenter`;
	if [ $? != 0 ];then
		echo "Select datacenter failed for $cIPv4eth0";
		exit;
	fi
	echo "Creating new tNode entry with uDatacenter=$uDatacenter";
	echo "INSERT INTO tNode SET cLabel='$cShortHostname',uOwner=2,uDatacenter=$uDatacenter,uStatus=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "INSERT INTO tNode failed!";
		exit;
	fi
	uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cShortHostname'" | $cMySQLConnect | grep -v uNode`;
	if [ $? != 0 ];then
		echo "Select node failed $cShortHostname after trying registration via IP!";
		exit
	fi
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

#
#cIPv4/cShortHostname
echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cIPv4'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 11 failed";
fi
echo "INSERT INTO tProperty SET cName='cIPv4',cValue='$cShortHostname',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 12 failed";
fi
#
#

#
#cIPv4:eth0/cIPv4eth0
echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cIPv4:eth0'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 11 failed";
fi
echo "INSERT INTO tProperty SET cName='cIPv4:eth0',cValue='$cIPv4eth0',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 12 failed";
fi
#
#

#
#cIPv4:eth1/cIPv4eth1
echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cIPv4:eth1'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 11 failed";
fi
echo "INSERT INTO tProperty SET cName='cIPv4:eth1',cValue='$cIPv4eth1',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 12 failed";
fi
#
#

#
#cKernel
echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cKernel'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 11 failed";
fi
echo "INSERT INTO tProperty SET cName='cKernel',cValue='$cKernel',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 12 failed";
fi
#
#

#
#cdmiSytemManufacturer
echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cdmiSytemManufacturer'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 101 failed";
fi
echo "INSERT INTO tProperty SET cName='cdmiSytemManufacturer',cValue='$cdmiSytemManufacturer',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 102 failed";
fi
#
#

#
#cdmiSytemProductName
echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cdmiSytemProductName'" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 103 failed";
fi
echo "INSERT INTO tProperty SET cName='cdmiSytemProductName',cValue='$cdmiSytemProductName',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	echo "mysql command 104 failed";
fi
#
#
