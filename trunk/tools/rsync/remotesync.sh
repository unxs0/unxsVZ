#!/bin/bash
#
#FILE
#	tools/rsync/remotesync.sh
#PURPOSE
#	for migration from remote datacenter
#NOTES
#	remote node has stopped clone containers
#	local node has running containers that need to be
#	incrementally updated before migration.
#	local container will be stopped while rsync takes place.

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

#Note that you must change the rsync line also. Since we have not had time to fix this.
cSSHPort="-p 22";

if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ];then
	echo "usage: $0 <remote source node host> <remote source VEID> <local target VEID>";
	exit 0;
fi

cLockfile="/tmp/remotesync.sh.lock.$2.$3";

fLog "start";
#do not run if another (same source and target VEIDs) clone job is also running
if [ -d $cLockfile ]; then
	fLog "waiting for lock release $cLockfile";
	exit 0;
else
	mkdir $cLockfile; 
fi

/usr/sbin/vzctl stop $3 > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "could not stop $3";
	#rollback
	rmdir $cLockfile;
	exit 1;
fi
fLog "stopped $3";


#make sure ssh is working
/usr/bin/ssh $cSSHPort $1 "ls /vz/private/$2 > /dev/null 2>&1";
if [ $? != 0 ];then
	fLog "/usr/bin/ssh $1 ls /vz/private/$2 failed";
	#rollback
	rmdir $cLockfile;
	/usr/sbin/vzctl start $3 > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "could not start $3";
	fi
	exit 2;
fi
fLog "ssh test ok $1 $2. Starting rsync";

/usr/bin/rsync -e '/usr/bin/ssh -ax -c arcfour -p 22' -avxlH --delete \
			--exclude "/proc/" --exclude "/root/.ccache/" \
			--exclude "/sys" --exclude "/dev" --exclude "/tmp" \
			--exclude /etc/sysconfig/network \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:0 \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:1 \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:2 \
				$1:/vz/private/$2/ /vz/private/$3 
#we can ignore return value 24:
#rsync warning: some files vanished before they could be transferred (code 24) at main.c(892) [sender=2.6.8]
if [ $? != 0 ] && [ $? != 24 ];then
	fLog "rsync failed";
	#rollback
	rmdir $cLockfile;
	/usr/sbin/vzctl start $3 > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "could not start $3";
	fi
	exit 3;
else
	#remove lock file
	rmdir $cLockfile;
	/usr/sbin/vzctl start $3 > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "could not start $3";
		exit 1;
	fi
	#debug only
	fLog "end";
	exit 0;
fi

