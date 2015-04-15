#!/bin/bash
#
#FILE
#	/usr/sbin/unxsvzEveryNodeCmd.sh
#PURPOSE
#	Run a command via ssh for all active nodes in the
#	same datacenter
#AUTHOR/LEGAL
#	(C) 2014-2015 Gary Wallis for Unixservice, LLC.

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
#
#

if [ "$1" == "" ] || [ "$2" != "" ]; then
        echo "usage $0: <command string quote if it has spaces>";
        exit;
fi

cShortHostname=`hostname -s`;
if [ $? != 0 ];then
	echo "hostname -s failed";
	exit;
fi
uNode=`$cMySQLConnect -N -e "SELECT uNode FROM tNode WHERE cLabel='$cShortHostname' AND uStatus=1"`;
if [ $? != 0 ];then
	echo "Select node failed $cShortHostname";
	exit;
fi
if [ "$uNode" == "" ];then
	echo "uNode failed";
	exit;
fi
uDatacenter=`$cMySQLConnect -N -e "SELECT uDatacenter FROM tNode WHERE uNode=$uNode"`;
if [ $? != 0 ];then
	echo "Select datacenter failed $uNode";
	exit;
fi
if [ "$uDatacenter" == "" ];then
	echo "uDatacenter failed";
	exit;
fi
cServerList=`$cMySQLConnect -N -e "SELECT cLabel FROM tNode WHERE uStatus=1 ORDER BY uNode"`;
if [ $? != 0 ];then
	echo "Select cLabel failed $uDatacenter";
	exit;
fi

#echo $cServerList;
#exit;

for cServer in $cServerList; do
	echo $cServer
	/usr/bin/ssh -c $cEncAlg $cServer "$1";
done
