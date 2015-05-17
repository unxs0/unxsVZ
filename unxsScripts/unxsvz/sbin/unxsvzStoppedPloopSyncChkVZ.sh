#!/bin/bash
#
#FILE
#	unxsScripts/unxsvz/sbin/unxsvzStoppedPloopSyncChkVZ.sh
#PURPOSE
#	Copy ploop files with rsync rdiff algo
#NOTES
#	/etc/ssh/ssh_config must be setup correctly
#	KISS script is commented and programmed for easy reading.
#
#	This version checks against VZ backend and if remote veid is 0 it
#	selects a clone container VEID for you.
#AUTHOR
#	Gary Wallis for Unixservice, LLC. (C) 2015-2015
#	GPLv2 Licensed.

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

if [ "$3" == "" ];then
	echo "usage: $0 <local veid> <remote veid> <target node host> [--reset-remote]";
	exit 0;
fi

cMySQLConnect="";
if [ -f "/etc/unxsvz/mysql.local.sh" ];then
	source /etc/unxsvz/mysql.local.sh;
else
	echo "no /etc/unxsvz/mysql.local.sh";
	exit 1;
fi
if [ "$cMySQLConnect" == "" ];then
	fLog "no cMySQLConnect";
	exit 1;
fi

uTargetNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$3'"|$cMySQLConnect|grep -v uNode`;
if [ $? != 0 ];then
	fLog "uNode SELECT error or not found";
	exit;
fi
if [ "$uTargetNode" == "" ];then
	fLog "uTargetNode could not be determined for $3";
	exit;
fi

if [ "$2" == "0" ];then
	uRemoteVEID=`echo "SELECT uContainer FROM tContainer WHERE uSource=$1 AND uNode=$uTargetNode"|$cMySQLConnect|grep -v uContainer`;
	if [ $? != 0 ];then
		fLog "uNode SELECT error or not found for uTargetNode=$uTargetNode";
		exit;
	fi
else
	uRemoteVEID=`echo "SELECT uContainer FROM tContainer WHERE uSource=$1 AND uNode=$uTargetNode AND uContainer=$2 "|$cMySQLConnect|grep -v uContainer`;
	if [ $? != 0 ];then
		fLog "uNode SELECT error or not found for uTargetNode=$uTargetNode";
		exit;
	fi
fi
if [ "$uRemoteVEID" == "" ];then
	fLog "uRemoteVEID could not be determined or did not match for $1 and $uTargetNode";
	exit;
fi

#echo $uRemoteVEID $uTargetNode;
#exit 0;
if [ "$2" == "0" ];then
	echo "Using $uRemoteVEID";
fi


if [ "$4" == "--reset-remote" ];then
	echo "--reset-remote actions start";
	/usr/bin/ssh $3 "rm -rf /vz/private/$uRemoteVEID/root.hdd" > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "rm -rf /vz/private/$uRemoteVEID/root.hdd";
	fi
	/usr/bin/ssh $3 "rm -rf /vz/private/$uRemoteVEID/dump" > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "rm -rf /vz/private/$uRemoteVEID/dump";
	fi
	echo "end";
fi

#one at a time no run away ssh ps please
cLockfile="/tmp/stoppedploopsync.sh.lock";

#do not run if another (same source and target VEIDs) clone job is also running
if [ -d $cLockfile ]; then
	fLog "waiting for lock release $cLockfile";
	exit 0;
else
	mkdir $cLockfile; 
fi


#make sure ssh is working and we have correct remote HN
/usr/bin/ssh $3 "ls /vz/private/$uRemoteVEID" > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "ssh $3 ls /vz/private/$uRemoteVEID failed";
	#rollback
	rm -rf $cLockfile;
	exit 2;
fi

#make sure remote container is stopped, else rsync will fail.
/usr/bin/ssh $3 vzlist -H $uRemoteVEID | grep running> /dev/null 2>&1;
if [ $? == 0 ];then
	fLog "Container $3:$uRemoteVEID is running";
	#rollback
	rm -rf $cLockfile;
	exit 2;
fi

#make sure local file exists
if [ ! -f /vz/private/$1/root.hdd/root.hdd ];then
	fLog "/vz/private/$1/root.hdd/root.hdd does not exist";
	#rollback
	rm -rf $cLockfile;
	exit 4;
fi

#take snapshot
UUID=$(uuidgen);
/usr/sbin/vzctl snapshot $1 --skip-suspend --skip-config --id $UUID > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "/usr/sbin/vzctl snapshot $1 --skip-suspend --skip-config --id $UUID";
	#rollback
	rm -rf $cLockfile;
	exit 2;
fi

RSYNC_OPTIONS="-vv -aHAX --delete --numeric-ids --stats";
/usr/bin/rsync -e '/usr/bin/ssh -ax -c arcfour'\
	 $RSYNC_OPTIONS  /vz/private/$1/  $3:/vz/private/$uRemoteVEID/ > /tmp/$1-$uRemoteVEID-$3.rsync 2>&1
#we can ignore return value 24:
#rsync warning: some files vanished before they could be transferred (code 24) at main.c(892) [sender=2.6.8]
if [ $? != 0 ] && [ $? != 24 ];then
	fLog "rsync failed";
	#rollback
	#remove snapshot
	/usr/sbin/vzctl snapshot-delete $1 --id $UUID > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "/usr/sbin/vzctl snapshot-delete $1 --id $UUID;";
	fi
	rm -rf $cLockfile;
	exit 3;
fi

#make sure remote container vz conf file has correct disk settings
cDISKINODES=`grep DISKINODES /etc/vz/conf/$1.conf`;
cDISKSPACE=`grep DISKSPACE /etc/vz/conf/$1.conf`;

ssh $3 sed -i -e "s/^DISKINODES.*/$cDISKINODES/g" /etc/vz/conf/$uRemoteVEID.conf > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "sed cDISKINODES failed";
fi
ssh $3 sed -i -e "s/^DISKSPACE.*/$cDISKSPACE/g" /etc/vz/conf/$uRemoteVEID.conf > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "sed cDISKSPACE failed";
fi


#remove local snapshot
/usr/sbin/vzctl snapshot-delete $1 --id $UUID > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "/usr/sbin/vzctl snapshot-delete $1 --id $UUID;";
fi

#remote switch and delete so container is ready to start
ssh $3 /usr/sbin/vzctl snapshot-switch $uRemoteVEID --id $UUID > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "ssh $3 /usr/sbin/vzctl snapshot-switch $uRemoteVEID --id $UUID;";
fi
ssh $3 /usr/sbin/vzctl snapshot-delete $uRemoteVEID --id $UUID > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "ssh $3 /usr/sbin/vzctl snapshot-delete $uRemoteVEID --id $UUID;";
fi


#remove lock file
rm -rf $cLockfile;
#debug only
#fLog "end";
exit 0;
