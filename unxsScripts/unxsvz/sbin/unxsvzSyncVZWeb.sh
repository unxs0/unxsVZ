#!/bin/bash
#
#FILE
#	/usr/sbin/unxsvzSyncVZWeb.sh
#PURPOSE
#	Keep a cloned unxsVZWeb server updated
#NOTES
#LEGAL
#	Copyright 2015-2016 Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies.


cLogFile="/tmp/unxsvzSyncVZWeb.sh.log";

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@" >> $cLogFile; }

if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ];then
	echo "usage: $0 <source VEID> <target VEID> <target node host> [<ssh port ignored> <rsync --bwlimit=numeric-value>]";
	exit 0;
fi
#$4 is the not currently used ssh port
if [ "$5" != "" ] && [ "$5" != "0" ];then
	cBWLimit="--bwlimit=$5";
	fLog "$cBWLimit";
else
	cBWLimit="";
fi
uContainer=$1;
uRemoteContainer=$2;
cTargetNode=$3;

if [ -f "/etc/unxsvz/mysql.local.sh" ];then
	source /etc/unxsvz/mysql.local.sh;
else
	echo "no /etc/unxsvz/mysql.local.sh";
	exit 1;
fi
#

uExitVal="0";

fLog "start $uContainer to $cTargetNode:$uRemoteContainer";

/usr/sbin/vzlist | grep $uContainer  > /dev/null 2>&1;
if [ $? != 0 ];then
        fLog "$uContainer not running. Will not sync.";
	#exit code 7 is temporary failure. Should be used by unxsVZ jobqueue.c to
	#maek sure backup date is NOT updated!
        exit 7;
fi

#check to see if remote is actually active
cWhoAmI=`/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer whoami"`
if [ "$cWhoAmI" != "root" ];then
        fLog "$uRemoteContainer not running. Will not sync.";
        exit 7;
fi

#do not run if another (same source and target VEIDs) clone job is also running
cContainerLock="/tmp/clonesync.sh.lock.$1.$2";
if [ -d $cContainerLock ]; then
	fLog "waiting for lock release $cContainerLock";
	exit 1;
else
	mkdir $cContainerLock; 
	fLog "lock dir created";
fi


#make sure ssh is working
#change /etc/ssh/ssh_conf for non standard port
/usr/bin/ssh $cTargetNode "ls /vz/root/$uRemoteContainer > /dev/null 2>&1";
if [ $? != 0 ];then
	fLog "/usr/bin/ssh $cTargetNode ls /vz/root/$uRemoteContainer failed";
	#rollback
	rmdir $cContainerLock;
	exit 2;
fi

###
#start rsync code section
#
fRsync() {
	cDebug="No";
	if [ $cDebug != "Yes" ];then
	/usr/bin/rsync -axlH $cBWLimit --rsh '/usr/bin/ssh -ax -c arcfour' --delete /vz/root/$uContainer/$@ $cTargetNode:/vz/root/$uRemoteContainer/$@\
			2>> $cLogFile 1> /dev/null
	else
	/usr/bin/rsync -avxlH $cBWLimit --rsh '/usr/bin/ssh -ax -c arcfour' --delete /vz/root/$uContainer/$@ $cTargetNode:/vz/root/$uRemoteContainer/$@
	fi
	if [ $? != 0 ];then
		fLog "rsync $4 failed";
	fi
}

#sync www content
fRsync "var/www/";

#sync www server config
fRsync "etc/httpd/";
fRsync "etc/pki/tls/certs/unixservice.bundle.pem";

ssh $cTargetNode "vzctl exec2 $uRemoteContainer 'service httpd condrestart'";
if [ $? != 0 ];then
	fLog "ssh command failed";
fi

#mysql tunnel stuff
ssh $cTargetNode "ls /vz/root/$uRemoteContainer/etc/sysconfig/network-scripts/ifcfg-lo:1" | grep ifcfg-lo > /dev/null;
if [ $? != 0 ];then
	fRsync "etc/sysconfig/network-scripts/ifcfg-lo:1";
	ssh $cTargetNode "vzctl exec2 $uRemoteContainer 'ifconfig lo:1 127.0.0.2 up'";
	if [ $? != 0 ];then
		fLog "ssh command3 failed";
	fi
fi
ssh $cTargetNode "ls /vz/root/$uRemoteContainer/etc/init.d/unxsautossh" | grep unxsautossh > /dev/null;
if [ $? != 0 ];then
	ssh $cTargetNode "vzctl exec2 $uRemoteContainer 'useradd -s /sbin/nologin unxsvz'";
	if [ $? != 0 ];then
		fLog "ssh command1 failed";
	fi
	fRsync "root/.ssh/";#must be a dir
	fRsync "home/unxsvz/.ssh/";#must be a dir
	fRsync "usr/bin/autossh";
	fRsync "usr/bin/autossh2";
	fRsync "etc/init.d/unxsautossh";
	fRsync "etc/init.d/unxsautossh2";
	ssh $cTargetNode "vzctl exec2 $uRemoteContainer 'service unxsautossh start'";
	if [ $? != 0 ];then
		fLog "ssh command1 failed";
	fi
	ssh $cTargetNode "vzctl exec2 $uRemoteContainer 'service unxsautossh2 start'";
	if [ $? != 0 ];then
		fLog "ssh command2 failed";
	fi
	ssh $cTargetNode "vzctl exec2 $uRemoteContainer 'chkconfig --level 3 unxsautossh2 on'";
	if [ $? != 0 ];then
		fLog "ssh command2 failed";
	fi
	ssh $cTargetNode "vzctl exec2 $uRemoteContainer 'chkconfig --level 3 unxsautossh on'";
	if [ $? != 0 ];then
		fLog "ssh command2 failed";
	fi
	ssh $cTargetNode "vzctl exec2 $uRemoteContainer 'chkconfig --level 3 sshd off'";
	if [ $? != 0 ];then
		fLog "ssh command2 failed";
	fi
fi

#
#end rsync code section
###

#everything ok clean exit
rmdir $cContainerLock;
fLog "end $uContainer";
exit $uExitVal;
