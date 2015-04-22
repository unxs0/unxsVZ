#!/bin/bash
#
#FILE
#	/usr/sbin/unxsvzAllNodeScp.sh
#PURPOSE
#	scp a file to all OTHER active nodes in the
#	same datacenter
#AUTHOR/LEGAL
#	(C) 2014 Gary Wallis for Unixservice, LLC.

cEncAlg="arcfour";
#
#user conf section
#required local settings. mysql and iptables
if [ -f "/etc/unxsvz/mysql.local.sh" ];then
	source /etc/unxsvz/mysql.local.sh;
else
	echo "no /etc/unxsvz/mysql.local.sh";
	exit 1;
fi
if [ -f "/etc/unxsvz/scp.local.sh" ];then
	source /etc/unxsvz/scp.local.sh;
fi
#
#

if [ ! $1 ] || [ ! -f $1 ]; then
        echo "usage $0: <fully qualified path of file to copy> [--remotedatacenter]";
        exit 0;
fi

cShortHostname=`hostname -s`;
if [ $? != 0 ];then
	echo "hostname -s failed";
	exit 1;
fi
uNode=`$cMySQLConnect -N -e "SELECT uNode FROM tNode WHERE cLabel='$cShortHostname' AND uStatus=1"`;
if [ $? != 0 ];then
	echo "Select node failed $cShortHostname";
	exit 1;
fi
if [ "$uNode" == "" ];then
	echo "uNode failed";
	exit 1;
fi
uDatacenter=`$cMySQLConnect -N -e "SELECT uDatacenter FROM tNode WHERE uNode=$uNode"`;
if [ $? != 0 ];then
	echo "Select datacenter failed $uNode";
	exit 1;
fi
if [ "$uDatacenter" == "" ];then
	echo "uDatacenter failed";
	exit 1;
fi
cServerList=`$cMySQLConnect -N -e "SELECT cLabel FROM tNode WHERE uDatacenter=$uDatacenter AND uStatus=1"`;
if [ $? != 0 ];then
	echo "Select cLabel failed $uDatacenter";
	exit 1;
fi

#echo $cServerList;
#exit;

for cServer in $cServerList; do
        if [ "$cServer" != "$cShortHostname" ];then
                echo $cServer;
                /usr/bin/scp -c $cEncAlg "$1" $cServer:"$1";
        fi
done


#this is for the second arg to turn off remote transfers for any file. Very useful for cluster sysadmin.
if [ "$2" != "--remotedatacenter" ];then exit 0; fi
if [ "$cRemoteServer" != "" ];then

	#protect some files from user error
	echo "$1" | grep "/vz/template/cache";
	if [ "$?" == "0" ]; then exit 1; fi

	echo "$1" | grep "/etc/vz/conf";
	if [ "$?" == "0" ]; then exit 1; fi

	echo $cRemoteServer;
	/usr/bin/scp $cRemotePort -c $cEncAlg "$1" $cRemoteServer:"$1";
fi

exit 0;
