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

#Low priority script do not run when server is loaded.
nLoad=`uptime | cut -f 5 -d : | cut -f 1 -d .`;
if (( $nLoad > 10 ));then
	fLog "waiting for $nLoad to go below 10";
	exit 0;
fi

if [ "$1" == "" ] || [ "$2" == "" ] ;then
	echo "usage: $0 <remote source pbx> <local target pbx VEID> [<ssh port>]";
	exit 0;
fi

if [ "$3" != "" ];then
	cSSHPort="$3";
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

#determine if target is running
cStatus=`/usr/sbin/vzlist -o status $2 | tail -n 1`;

if [ "$cStatus" == "" ] || [ "$cStatus" != "stopped" ] && [ "$cStatus" != "running" ];then
	fLog "local VEID status could not be determined";
	#rollback
	rmdir $cLockfile;
	exit 1;
fi

#debug only
#rmdir $cLockfile;
#exit 0;

#wrap asterisk stop start conditional
if [ "$cStatus" == "running" ];then
	/usr/sbin/vzctl exec2 $2 "service asterisk stop";
	if [ $? != 0 ];then
		fLog "local asterisk stop failed";
		#rollback
		rmdir $cLockfile;
		exit 1;
	fi
else 
	#this sync may be too slow for mail servers and other similar 
	#many file open #servers
	/usr/bin/ssh -p $cSSHPort $1 "/bin/sync > /dev/null 2>&1";
	if [ $? != 0 ];then
		fLog "remote sync failed";
		#rollback
		rmdir $cLockfile;
		exit 1;
	fi
fi


#test with --dry-run switch
/usr/bin/rsync -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete \
			$1:/var/spool/asterisk/ /vz/private/$2/var/spool/asterisk/
#we can ignore return value 24:
#rsync warning: some files vanished before they could be transferred (code 24) at main.c(892) [sender=2.6.8]
if [ $? != 0 ] && [ $? != 24 ];then
	fLog "rsync 1 failed";
fi

/usr/bin/rsync -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete \
			$1:/etc/asterisk/ /vz/private/$2/etc/asterisk/
if [ $? != 0 ] && [ $? != 24 ];then
	fLog "rsync 4 failed";
fi

/usr/bin/rsync -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete \
			$1:/var/lib/asterisk/ /vz/private/$2/var/lib/asterisk/
if [ $? != 0 ] && [ $? != 24 ];then
	fLog "rsync 5 failed";
fi

/usr/bin/rsync -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete \
			$1:/var/www/html/panel/ /vz/private/$2/var/www/html/panel/
if [ $? != 0 ] && [ $? != 24 ];then
	fLog "rsync 6 failed";
fi

#wrap mysql data in stop start conditional
if [ "$cStatus" == "running" ];then
	/usr/sbin/vzctl exec2 $2 "service mysqld stop";
	if [ $? != 0 ];then
		fLog "local mysqld stop failed. mysql data not rsync'd";
	else
		/usr/bin/rsync -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete \
			$1:/var/lib/mysql/asterisk/ /vz/private/$2/var/lib/mysql/asterisk/
		if [ $? != 0 ] && [ $? != 24 ];then
			fLog "rsync 2 failed";
		fi

		/usr/bin/rsync -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete \
			$1:/var/lib/mysql/asteriskcdrdb/ /vz/private/$2/var/lib/mysql/asteriskcdrdb/
		if [ $? != 0 ] && [ $? != 24 ];then
			fLog "rsync 3 failed";
		fi

		/usr/sbin/vzctl exec2 $2 "service mysqld start";
		if [ $? != 0 ];then
			fLog "local mysqld start failed";
		fi
	fi
fi


#wrap asterisk stop start conditional
if [ "$cStatus" == "running" ];then
	/usr/sbin/vzctl exec2 $2 "service asterisk start";
	if [ $? != 0 ];then
		fLog "local asterisk start failed";
	fi
fi
#end asterisk wrapper


rmdir $cLockfile;
#debug only
fLog "end";
exit 0;
