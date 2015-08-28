#!/bin/bash

#FILE
#       /usr/sbin/unxsvzSetContainerStatus.sh 
#PURPOSE
#       Change container status 
#LEGAL
#	Copyright (C) 2015 Ricardo Armas for Unixservice, LLC.
#	Copyright (C) 2012-2015 Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies.
#NOTES
#	Maybe we should restrict to only containers running on this HN?


if [ "$1" == "" ] || [ "$2" == "" ];then
        echo "usage: $0 <uVEID> <uStatus>";
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

uContainer=`echo "SELECT uContainer FROM tContainer WHERE uContainer=$1"|$cMySQLConnect -B -N`;
if [ $? != 0 ] || [ "$uContainer" == "" ];then
	fLog "uVEID SELECT error";
	exit 1;
fi

uStatus=`echo "SELECT uStatus FROM tStatus WHERE uStatus=$2"|$cMySQLConnect -B -N`;
if [ $? != 0 ] || [ "$uStatus" == "" ];then
        fLog "uStatus SELECT error";
        exit 1;
fi

echo "UPDATE tContainer SET uStatus=$uStatus WHERE uContainer=$uContainer"|$cMySQLConnect;
if [ $? != 0 ];then
        fLog "UPDATE error";
        exit 1;
fi

exit 0;
