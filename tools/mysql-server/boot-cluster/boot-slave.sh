#!/bin/bash

#FILE
#	boot-cluster.sh
#	$Id$
#LEGAL
#	Copyright Gary Wallis for Unixservice, LLC. 2010.
#	GPLv2 license applies.
#NOTES
#	Requires fast internal LAN cluster interconnect with passwordless ssh set up.
#	Warning mail not implemented yet.

#Configuration section
#2 hours
iTimeoutSecs="7200";
cWarnEmail="supportgrp@unixservice.com";
cSSHPort="22"
#end

#our standard bash logging function.
fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

if [ "$1" == "" ] || [ "$2" == "" ];then
	echo "usage: $0 <mysql password> <master ip>";
	exit 0;
fi

#wait for master to come up. If does not come up after iTimeoutSecs send warning email, exit error.
iCounter=0;
ping -c 1 $2 > /dev/null 2>&1;
while [ $? != 0 ] && [ $iCounter -lt $iTimeoutSecs ];do
	let iCounter=iCounter+1;
	#fLog "retrying ping $iCounter/$iTimeoutSecs";
	ping -c 1 $2 > /dev/null 2>&1;
done
if [ $iCounter -eq "$iTimeoutSecs" ];then
	fLog "ping master timeout";
	#exit 1;
fi

iCounter=0;
/usr/bin/ssh -p $cSSHPort $2 exit > /dev/null 2>&1;
while [ $? != 0 ] && [ $iCounter -lt $iTimeoutSecs ];do
	let iCounter=iCounter+1;
	sleep 1;
	#fLog "retrying check for ssh $iCounter/$iTimeoutSecs";
	/usr/bin/ssh -p $cSSHPort $2 exit > /dev/null 2>&1;
done
if [ $iCounter -eq "$iTimeoutSecs" ];then
	fLog "remote ssh timeout";
	#exit 2;
fi

iCounter=0;
/usr/bin/ssh -p $cSSHPort $2 ps -ef | grep mysqld  > /dev/null 2>&1;
while [ $? != 0 ] && [ $iCounter -lt $iTimeoutSecs ];do
	let iCounter=iCounter+1;
	sleep 1;
	#fLog "retrying check for mysqld $iCounter/$iTimeoutSecs";
	/usr/bin/ssh -p $cSSHPort $2 ps -ef | grep mysqld  > /dev/null 2>&1;
done
if [ $iCounter -eq "$iTimeoutSecs" ];then
	fLog "remote mysqld timeout";
	#exit 2;
fi


iCounter=0;
cBinLog="";
cPosition="";
eval `/usr/bin/ssh -p $cSSHPort $2 "echo 'SHOW MASTER STATUS' | mysql -p$1 | grep mysql_binary_log" | awk '{printf"cBinLog=%s\ncPosition=%s\n",$1,$2}'`;
while ( [ "$cBinLog" == "" ] || [ "$cPosition" == "" ] ) && [ $iCounter -lt $iTimeoutSecs ];do
	let iCounter=iCounter+1;
	cBinLog="";
	cPosition="";
	sleep 1;
	#fLog "retrying get master data $iCounter/$iTimeoutSecs";
	eval `/usr/bin/ssh -p $cSSHPort $2 "echo 'SHOW MASTER STATUS' | mysql -p$1 | grep mysql_binary_log" | awk '{printf"cBinLog=%s\ncPosition=%s\n",$1,$2}'`;
done
if [ $iCounter -eq "$iTimeoutSecs" ];then
	fLog "remote get master data timeout $iCounter/$iTimeoutSecs";
	#exit 2;
fi

#fLog "debug exit $iCounter/$iTimeoutSecs cBinLog=$cBinLog cPosition=$cPosition";
#exit 0;

#master is up, get master status and parse master status.
cBinLog="";
cPosition="";
eval `/usr/bin/ssh -p $cSSHPort $2 "echo 'SHOW MASTER STATUS' | mysql -p$1 | grep mysql_binary_log" | awk '{printf"cBinLog=%s\ncPosition=%s\n",$1,$2}'`;
if [ $? == 0 ]  && [ "$cBinLog" != "" ] && [ "$cPosition" != "" ];then

	#make sure slave mode is off.
	echo "STOP SLAVE" | mysql -p$1 > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "local slave stop failed";
		exit 4;
	fi

	#SQL "change master to" parsed data on local slave MySQL server.
	echo "CHANGE MASTER TO MASTER_LOG_FILE='$cBinLog',MASTER_LOG_POS=$cPosition" | mysql -p$1 > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "local change master failed";
		exit 5;
	fi
	fLog "change master ok cBinLog=$cBinLog cPosition=$cPosition";

	#start slave
	echo "START SLAVE" | mysql -p$1 > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "local slave stop failed";
		exit 6;
	fi
	sleep 2;

	#check status. if slave did not catch up in max iTimeoutSecs send warning email, exit error.
	eval `echo "SHOW SLAVE STATUS\G" | mysql -p$1 | grep Seconds_Behind_Master | awk '{printf"iSecondsBehindMaster=%s\n",$2}'`;
	if [ $? != 0 ];then
		fLog "show local slave status failed";
		exit 7;
	fi
	if [ "$iSecondsBehindMaster" == "NULL" ];then
		fLog "local slave status NULL";
		exit 7;
	fi
	#fLog "iSecondsBehindMaster=$iSecondsBehindMaster";
	iCounter=0;
	while [ $iCounter -lt "$iTimeoutSecs" ] && [ $iSecondsBehindMaster -gt "0" ];do
		let iCounter=iCounter+1;
		eval `echo "SHOW SLAVE STATUS\G" | mysql -p$1 | grep Seconds_Behind_Master | awk '{printf"iSecondsBehindMaster=%s\n",$2}'`;
		if [ $? != 0 ];then
			fLog "show local slave status failed";
			exit 8;
		fi
		if [ "$iSecondsBehindMaster" == "NULL" ];then
			fLog "local slave status NULL";
			exit 9;
		fi
		sleep 1;
	done
	if [ $iCounter -eq "$iTimeoutSecs" ];then
		fLog "local slave catch-up timeout";
		exit 10;
	fi


else
	fLog "parse master status failed";
	exit 3;
fi


