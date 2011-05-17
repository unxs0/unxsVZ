#!/bin/bash
#
#FILE
#	tools/rsync/vzcachesync.sh
#PURPOSE
#NOTES

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

#Note that you must change the rsync line also. Since we have not had time to fix this.
cSSHPort="-p 12337";

if [ "$1" == "" ];then
	echo "usage: $0 <target node host>";
	exit 0;
fi

cLockfile="/tmp/vzcachesync.sh.lock";

#do not run if another (same source and target VEIDs) clone job is also running
if [ -d $cLockfile ]; then
	fLog "waiting for lock release $cLockfile";
	exit 0;
else
	mkdir $cLockfile; 
fi

#make sure ssh is working
/usr/bin/ssh $cSSHPort $1 "ls /vz/template/cache > /dev/null 2>&1";
if [ $? != 0 ];then
	fLog "/usr/bin/ssh $1 ls /vz/template/cache failed";
	#rollback
	rm -rf $cLockfile;
	exit 2;
fi

/usr/bin/rsync -e '/usr/bin/ssh -ax -c arcfour -p 12337' --stats -axlH  /vz/template/cache/ $1:/vz/template/cache
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

