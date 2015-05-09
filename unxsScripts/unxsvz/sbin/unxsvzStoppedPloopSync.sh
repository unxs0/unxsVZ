#!/bin/bash
#
#FILE
#	/usr/sbin/unxsvzStoppedPloopSync.sh	
#PURPOSE
#	Copy ploop files with rsync rdiff algo
#NOTES
#	/etc/ssh/ssh_config must be setup correctly
#	we rely on unxsVZ to keep remote vz.conf files ok

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

if [ "$3" == "" ];then
	echo "usage: $0 <local veid> <remote veid> <target node host>";
	exit 0;
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

#take snapshot

#make sure ssh is working
/usr/bin/ssh $3 "ls /vz/private/$2/root.hdd/root.hdd > /dev/null 2>&1";
if [ $? != 0 ];then
	fLog "ssh $s ls /vz/private/$2/root.hdd/root.hdd failed";
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


/usr/bin/rsync -e '/usr/bin/ssh -ax -c arcfour'\
	 --stats -axlH  /vz/private/$1/root.hdd  $3:/vz/private/$2/ > /tmp/$1-$2-$3.rsync 2>&1
#we can ignore return value 24:
#rsync warning: some files vanished before they could be transferred (code 24) at main.c(892) [sender=2.6.8]
if [ $? != 0 ] && [ $? != 24 ];then
	fLog "rsync failed";
	#rollback
	rm -rf $cLockfile;
	exit 3;
else
	#remove lock file
	rm -rf $cLockfile;

	#debug only
	#fLog "end";
	exit 0;
fi

