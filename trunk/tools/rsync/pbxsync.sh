#!/bin/bash
#
#FILE
#	tools/rsync/pbxsync.sh
#PURPOSE
#	Keep a local backup unxsVZ asterisk PBX container rsync'd with
#	remote normally active PBX container or appliance.
#NOTES
#	Possible dirs of interest for backup of PBXs based on node rsync records of
#	dozens of active PBXs.
#
#	/var/spool/asterisk
#	(voicemail and monitor)
#
#	/var/lib/mysql/asterisk
#	/var/lib/mysql/asteriskcdrdb
#	(MySQL files .MYD .MYI)
#
#	/etc/asterisk
#	(.conf files)
#
#	/var/lib/asterisk
#	(astdb and agi-bin .agi files)
#
#	/var/www/html/panel
#	(op_buttons_additional.cfg)

#Settings
cSSHPort="22";

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }


if [ "$1" == "" ] || [ "$2" == "" ] ;then
	echo "usage: $0 <remote source pbx> <local target pbx VEID>";
	exit 0;
fi

#debug only
fLog "start $1 to local $2";

cLockfile="/tmp/pbxsync.sh.lock.$1.$2";

#do not run if another (same source and target VEIDs) clone job is also running
if [ -d $cLockfile ]; then
	fLog "waiting for lock release $cLockfile";
	exit 0;
else
	mkdir $cLockfile; 
fi

if [ ! -d /vz/private/$2/var/spool/asterisk/ ]; then
	fLog "/vz/private/$2/var/spool/asterisk/ does not exist";
	exit 0;
fi

#this sync may be too slow for mail servers and other similar 
#many file open #servers
/usr/bin/ssh -p $cSSHPort $1 "/bin/sync > /dev/null 2>&1";
if [ $? != 0 ];then
	fLog "remote sync failed";
	#rollback
	rmdir $cLockfile;
	exit 1;
fi

/usr/bin/rsync -e '/usr/bin/ssh -ax -c arcfour -p '\''$cSSHPort'\''' -avxlH --delete 
			$1:/var/spool/asterisk/ /vz/private/$2/var/spool/asterisk/
#we can ignore return value 24:
#rsync warning: some files vanished before they could be transferred (code 24) at main.c(892) [sender=2.6.8]
if [ $? != 0 ] && [ $? != 24 ];then
	fLog "rsync failed";
	#rollback
	rmdir $cLockfile;
	exit 3;
else
	#remove lock file
	rmdir $cLockfile;

	#debug only
	#fLog "end";
	exit 0;
fi

