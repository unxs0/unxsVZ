#!/bin/bash

#Settings
cSSHPort="14432";
#test with --dry-run switch
#cDryrun="--dry-run";
cDryrun="";
cIgnoreSpool="yes";
cMySQLConnect="/usr/bin/mysql -h rc1.callingcloud.net -u unxsvz -pwsxedc unxsvz";



if [ "$1" == "" ] || [ "$2" == "" ] ;then
        echo "usage: $0 <remote source pbx> <local target pbx VEID> [<ssh port>]";
        exit 0;
fi


fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

#Low priority script do not run when server is loaded.
nLoad=`uptime | awk '{print $10}'| cut -f 1 -d .`;
if (( $nLoad > 5 ));then
	fLog "waiting for $nLoad to go below 5";
	exit 0;
fi

#Check if appliance is running.
ssh -p 14432 $1 "ls";
if [ $? != 0 ];then
	fLog "waiting for $1 to be up";
	exit 0;
fi


if [ "$3" != "" ];then
	cSSHPort="$3";
fi

#debug only
fLog "start $1 to local $2";

cLockfile="/tmp/pbxsync.sh.lock.$1.$2";
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

#wrap asterisk stop start conditional
if [ "$cStatus" == "running" ];then
	/usr/sbin/vzctl exec2 $2 "service asterisk stop" > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "local asterisk stop failed";
		#rollback
		rmdir $cLockfile;
		exit 1;
	fi
	echo running
fi

if [ "$cDryrun" == "" ];then
	#this sync may be too slow for mail servers and other similar 
	#many file open #servers
	echo sync
	/usr/bin/ssh -c arcfour -p $cSSHPort $1 "/bin/sync > /dev/null 2>&1";
	if [ $? != 0 ];then
		fLog "remote sync failed";
		#rollback
		rmdir $cLockfile;
		exit 1;
	fi
fi

#recover wav files later if needed from downed server or backup drive
if [ "$cIgnoreSpool" != "yes" ];then
	fLog "rsync /var/spool/asterisk/";
	/usr/bin/rsync -avz -e "ssh -p 12265" --delete root@$1:/vz/private/101/etc/asterisk/ /vz/private/$2/etc/asterisk/
	#/usr/bin/rsync $cDryrun -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete \
			$1:/vz/private/101/var/spool/asterisk/ /vz/private/$2/var/spool/asterisk/
	#we can ignore return value 24:
	#rsync warning: some files vanished before they could be transferred (code 24) at main.c(892) [sender=2.6.8]
	if [ $? != 0 ] && [ $? != 24 ];then
		fLog "rsync 1 failed";
	fi

fi

fLog "rsync /etc/asterisk/";

#echo /usr/bin/rsync $cDryrun -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete --exclude '*.gz'\
#			$1:/vz/private/101/etc/asterisk/ /vz/private/$2/etc/asterisk/
/usr/bin/rsync -avz -e "ssh -p $cSSHPort" --delete root@$1:/etc/asterisk/ /vz/private/$2/etc/asterisk/
if [ $? != 0 ] && [ $? != 24 ];then
	echo $?
	fLog "rsync 4 failed";
fi

#fLog "rsync /var/lib/asterisk/";
#/usr/bin/rsync -avz -e "ssh -p $cSSHPort" --delete root@$1:/vz/private/101/var/lib/asterisk/ /vz/private/$2/var/lib/asterisk/
#/usr/bin/rsync $cDryrun -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --bwlimit=100 --delete --exclude '*.gz'\
#			$1:/vz/private/101/var/lib/asterisk/ /vz/private/$2/var/lib/asterisk/
#if [ $? != 0 ] && [ $? != 24 ];then
#	fLog "rsync 5 failed";
#fi

fLog "rsync /var/www/html/panel/";
/usr/bin/rsync -avz -e "ssh -p $cSSHPort" --delete root@$1:/var/www/html/panel/ /vz/private/$2/var/www/html/panel/
#/usr/bin/rsync $cDryrun -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete --exclude '*.gz'\
#			$1:/vz/private/101/var/www/html/panel/ /vz/private/$2/var/www/html/panel/
if [ $? != 0 ] && [ $? != 24 ];then
	fLog "rsync 6 failed";
fi

#wrap mysql data in stop start conditional
if [ "$cStatus" == "running" ];then
	/usr/sbin/vzctl exec2 $2 "service mysqld stop" > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "local mysqld stop failed. mysql data not rsync'd";
	else
		#fRsyncDB;
	fLog "rsync /var/lib/mysql/asterisk/";
	/usr/bin/rsync -avz -e "ssh -p $cSSHPort" --delete root@$1:/var/lib/mysql/asterisk/ /vz/private/$2/var/lib/mysql/asterisk/
	#/usr/bin/rsync $cDryrun -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete \
	#		$1:/vz/private/101/var/lib/mysql/asterisk/ /vz/private/$2/var/lib/mysql/asterisk/
	if [ $? != 0 ] && [ $? != 24 ];then
		fLog "rsync 2 failed";
	fi

	fLog "rsync /var/lib/mysql/asteriskcdrdb/";
	/usr/bin/rsync -avz -e "ssh -p $cSSHPort" --delete root@$1:/var/lib/mysql/asteriskcdrdb/ /vz/private/$2/var/lib/mysql/asteriskcdrdb/	
	#/usr/bin/rsync $cDryrun -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete \
	#	$1:/vz/private/101/var/lib/mysql/asteriskcdrdb/ /vz/private/$2/var/lib/mysql/asteriskcdrdb/
	if [ $? != 0 ] && [ $? != 24 ];then
		fLog "rsync 3 failed";
	fi

		/usr/sbin/vzctl exec2 $2 "service mysqld start" > /dev/null 2>&1;
		comando_sql="\"delete from sipsettings where keyword='localnet_0' or keyword='netmask_0';\""
		/usr/sbin/vzctl exec2 $2 "echo $comando_sql | mysql asterisk" > /dev/null 2>&1;
		if [ $? != 0 ];then
			fLog "local mysqld start failed";
		fi
	fi
else
	#fRsyncDB;
	fLog "rsync /var/lib/mysql/asterisk/";
	/usr/bin/rsync -avz -e "ssh -p $cSSHPort" --delete root@$1:/var/lib/mysql/asterisk/ /vz/private/$2/var/lib/mysql/asterisk/
	#/usr/bin/rsync $cDryrun -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete \
	#		$1:/vz/private/101/var/lib/mysql/asterisk/ /vz/private/$2/var/lib/mysql/asterisk/
	if [ $? != 0 ] && [ $? != 24 ];then
		fLog "rsync 2 failed";
	fi

	fLog "rsync /var/lib/mysql/asteriskcdrdb/";
	/usr/bin/rsync -avz -e "ssh -p $cSSHPort" --delete root@$1:/var/lib/mysql/asteriskcdrdb/ /vz/private/$2/var/lib/mysql/asteriskcdrdb/	
	#/usr/bin/rsync $cDryrun -e '/usr/bin/ssh -ax -c arcfour -p '\'$cSSHPort\''' -avxlH  --delete \
	#	$1:/vz/private/101/var/lib/mysql/asteriskcdrdb/ /vz/private/$2/var/lib/mysql/asteriskcdrdb/
	if [ $? != 0 ] && [ $? != 24 ];then
		fLog "rsync 3 failed";
	fi
fi


#wrap asterisk stop start conditional
if [ "$cStatus" == "running" ];then
	/usr/sbin/vzctl exec2 $2 "service asterisk start" > /dev/null 2>&1;
	sleep 3
	/usr/sbin/vzctl exec2 $2 "/var/lib/asterisk/bin/retrieve_conf" > /dev/null 2>&1;
	sleep 3
	/usr/sbin/vzctl exec2 $2 "service asterisk reload" > /dev/null 2>&1;	
	if [ $? != 0 ];then
		fLog "local asterisk start failed";
	fi
fi
#end asterisk wrapper

echo "UPDATE tContainer SET uBackupDate=UNIX_TIMESTAMP(NOW()) WHERE uContainer=$2" | $cMySQLConnect;
if [ $? != 0 ];then
	fLog "mysql UPDATE failed for $2";
fi

rmdir $cLockfile;
#debug only
fLog "end";
exit 0;

