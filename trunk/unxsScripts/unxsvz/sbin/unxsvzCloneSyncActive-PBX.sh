#!/bin/bash
#
#FILE
#	unxsvzCloneSyncActive-PBX.sh
#PURPOSE
#	Keep a cloned live pbx container sync's up to master PBX
#NOTES
#	This is an asterisk/freepbx specific sync script.
#	Example script for use with unxsVZ virtualization
#	manager.
#LEGAL
#	Copyright 2013-2015 Ricardo Armas and Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies.

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@" >> /tmp/unxsvzCloneSyncActive-PBX.sh.log; }

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
cLocalNetFileSpec="/etc/unxsvz/asterisk/localnet";
cExternIpFileSpec="/etc/unxsvz/asterisk/externip";
cBindPortFileSpec="/etc/unxsvz/asterisk/bindport";

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

uActiveChannels=`/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer  \"asterisk -rx 'sip show channelstats'\" | grep active | cut -f1 -d ' '"`
if [ $uActiveChannels != 0 ];then
        fLog "$uContainer has active calls. Will not sync.";
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
/usr/bin/ssh $cTargetNode "ls /vz/private/$uRemoteContainer > /dev/null 2>&1";
if [ $? != 0 ];then
	fLog "/usr/bin/ssh $cTargetNode ls /vz/private/$uRemoteContainer failed";
	#rollback
	rmdir $cContainerLock;
	exit 2;
fi

cAMPDBUSER=`/usr/sbin/vzctl exec2 $uContainer '/bin/grep "^AMPDBUSER=" /etc/amportal.conf | tail -n 1 | /bin/cut -f 2 -d ='`;
cAMPDBPASS=`/usr/sbin/vzctl exec2 $uContainer '/bin/grep "^AMPDBPASS=" /etc/amportal.conf | tail -n 1 | /bin/cut -f 2 -d ='`;
#functions
funcQuery() { 

	/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec $uRemoteContainer \" echo '$@' | mysql -N -u $cAMPDBUSER -p$cAMPDBPASS asterisk \" >/dev/null 2>&1";
}

funcQuerySelect() {

        /usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec $uRemoteContainer \" echo '$@' | mysql -N -u $cAMPDBUSER -p$cAMPDBPASS asterisk \" ";
}


#dump source sql
/usr/sbin/vzctl exec2 $uContainer "/usr/bin/mysqldump -u $cAMPDBUSER --password=$cAMPDBPASS asterisk > /tmp/dump.sql";
if [ $? != 0 ];then
	fLog "mysqldump failed";
	rmdir $cContainerLock;
	exit 3;
fi

#copy dump to target
/usr/bin/scp /vz/private/$uContainer/tmp/dump.sql $cTargetNode:/vz/private/$uRemoteContainer/tmp/dump.sql \
		2>> /tmp/clonesync-active.log 1> /dev/null
if [ $? != 0 ];then
	fLog "scp of mysqldump failed";
	rmdir $cContainerLock;
	exit 4;
fi

#restore on target
/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer \"cat /tmp/dump.sql | mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk\";" \
		2>> /tmp/clonesync-active.log 1> /dev/null
if [ $? != 0 ];then
	fLog "restore of mysqldump failed";
	rmdir $cContainerLock;
	exit 5;
fi

#get last seq from sipsettings
cSQL="SELECT max(seq) FROM sipsettings WHERE type=9"
uLastSeq=` funcQuerySelect $cSQL `
if [ $? != 0 ];then
	fLog "no sipsettings table?";
fi
#echo $uLastSeq
if [ "$uLastSeq" == "NULL" ];then
	uNextSeq=0;
else
	#echo $uNextSeq
	let uNextSeq=$uLastSeq+1;
fi
#echo nextseq: $uNextSeq


###
#start NAT code block
#

#NAT section. Not always the case! So we use first config file as test
cLocalNet=`/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec $uRemoteContainer cat '$cLocalNetFileSpec'" 2> /dev/null`;
if [ "$cLocalNet" != "" ];then

	fLog "active clone appears to be NAT";
	cNATMode="Yes";

	#
	#localnet setup
	cSQL="INSERT INTO sipsettings SET keyword='\''localnet'\'',data='\''$cLocalNet'\'',seq=$uNextSeq,type=9";
	funcQuery $cSQL;
	if [ $? != 0 ];then
		cSQL="UPDATE sipsettings SET data='\''$cLocalNet'\'',seq=0,type=9 WHERE keyword='\''localnet'\''";
		funcQuery $cSQL;
		if [ $? != 0 ];then
			fLog "insert and update failed";
			rmdir $cContainerLock;
			exit 10;
		fi
	fi
	#localnet setup end
	#
	
	#
	#externip setup
	let uNextSeq=$uNextSeq+1;
	cExternIp=`/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec $uRemoteContainer cat '$cExternIpFileSpec'"`;
	cSQL="INSERT INTO sipsettings SET keyword='\''externip'\'',data='\''$cExternIp'\'',seq=$uNextSeq,type=9";
	funcQuery $cSQL;
	if [ $? != 0 ];then
	        cSQL="UPDATE sipsettings SET data='\''$cExternIp'\'',seq=1,type=9 WHERE keyword='\''externip'\''";
	        funcQuery $cSQL;
	        if [ $? != 0 ];then
	                fLog "insert and update failed";
			rmdir $cContainerLock;
			exit 11;
	        fi
	fi
	#externip setup end
	#
	
	#
	#bindport setup
	let uNextSeq=$uNextSeq+1;
	cBindPort=`/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec $uRemoteContainer cat '$cBindPortFileSpec'"`;
	cSQL="INSERT INTO sipsettings SET keyword='\''bindport'\'',data='\''$cBindPort'\'',seq=$uNextSeq,type=9";
	funcQuery $cSQL;
	if [ $? != 0 ];then
	        cSQL="UPDATE sipsettings SET data='\''$cBindPort'\'',seq=2,type=9 WHERE keyword='\''bindport'\''";
	        funcQuery $cSQL;
	        if [ $? != 0 ];then
	                fLog "insert and update failed";
			rmdir $cContainerLock;
			exit 12;
	        fi
	fi
	#bindport setup end
	#

else	
	fLog "NOT NAT active clone";
fi
#
#end NAT code block
###


#only log errors
#retrieve conf
/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer \"/var/lib/asterisk/bin/retrieve_conf\""\
		2>> /tmp/clonesync-active.log 1> /dev/null
if [ $? != 0 ];then
	fLog "retrieve_conf failed";
fi

#stop asterisk and freepbx
/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer \"amportal stop\""\
		2>> /tmp/clonesync-active.log 1> /dev/null
if [ $? != 0 ];then
	fLog "retrieve_conf failed";
fi

###
#start rsync code section
#

#idea:
#we can limit rsync activity by keeing some kind of local check sum of rsync dirs
#and then only rsync if the checksum has changed.

#sync etc/asterisk
/usr/bin/rsync -axlH $cBWLimit --rsh '/usr/bin/ssh -ax -c arcfour' --delete /vz/private/$1/etc/asterisk/ $3:/vz/private/$2/etc/asterisk/\
		2>> /tmp/clonesync-active.log 1> /dev/null
if [ $? != 0 ];then
	fLog "rsync /etc/asterisk/ failed";
fi

#sync spool
/usr/bin/rsync -axlH $cBWLimit --rsh '/usr/bin/ssh -ax -c arcfour' --delete /vz/private/$1/var/spool/asterisk/ $3:/vz/private/$2/var/spool/asterisk\
		2>> /tmp/clonesync-active.log 1> /dev/null
if [ $? != 0 ];then
	fLog "rsync /var/spool/asterisk/ failed";
fi

#sync www
/usr/bin/rsync -axlH $cBWLimit --rsh '/usr/bin/ssh -ax -c arcfour' --delete /vz/private/$1/var/www/ $3:/vz/private/$2/var/www/\
		2>> /tmp/clonesync-active.log 1> /dev/null
if [ $? != 0 ];then
	fLog "rsync /var/www/ failed";
fi

#sync var/lib/asterisk
/usr/bin/rsync -axlH $cBWLimit --rsh '/usr/bin/ssh -ax -c arcfour' --delete /vz/private/$1/var/lib/asterisk/ $3:/vz/private/$2/var/lib/asterisk/\
		2>> /tmp/clonesync-active.log 1> /dev/null
if [ $? != 0 ];then
	fLog "rsync /var/lib/asterisk/ failed";
fi

#
#rsync code block end
##


if [ "$cNATMode" == "Yes" ];then
	#restore rtp.conf 
	/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer \"cp /etc/unxsvz/asterisk/rtp.conf  /etc/asterisk/rtp.conf\""\
		2>> /tmp/clonesync-active.log 1> /dev/null
	if [ $? != 0 ];then
		fLog "restore rtp.conf failed";
	fi
fi

#start asterisk and freepbx again
/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer \"amportal start\""\
		2>> /tmp/clonesync-active.log 1> /dev/null
if [ $? != 0 ];then
	fLog "start asterisk and freepbx failed";
fi

#reload config
/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer \"/var/lib/asterisk/bin/module_admin reload\""\
		2>> /tmp/clonesync-active.log 1> /dev/null
if [ $? != 0 ];then
	fLog "module_admin reload failed";
fi


funcBEQuery() { 

	echo "$@" | $cMySQLConnect -N >/dev/null 2>&1;
}

funcBEQuerySelect() {

        echo "$@" | $cMySQLConnect -N ;
}

##function funcSetProperty
#Pass uContainer, cName and cValue.
#returns nothing
funcSetProperty() {

	local uContainer="$1";
	local cName="$2";
	local cValue="$3";
	local cRetVal="";

	cSQL="SELECT cValue FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='$cName'";
	cRetVal=` funcBEQuerySelect $cSQL `
	if [ $? != 0 ];then
		fLog "mysql command 1 failed";
		return;
	fi
	if [ "$cRetVal" == "" ];then
		cSQL="INSERT INTO tProperty SET cName='$cName',cValue='$cValue',uKey=$uContainer,\
				uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())";
		funcBEQuery $cSQL;
		if [ $? != 0 ];then
			fLog "mysql command 2 failed";
			return;
		fi
	else
		cSQL="UPDATE tProperty SET cValue='$cValue',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW())\
				 WHERE uKey=$uContainer AND uType=3 AND cName='$cName'"
		funcBEQuery $cSQL;
		if [ $? != 0 ];then
			fLog "mysql command 3 failed";
			return;
		fi
	fi
}
#funcSetProperty()


##function funcGetProperty
#Pass uContainer, cName
#returns cValue
funcGetProperty() {

	local uContainer="$1";
	local cName="$2";
	local cRetVal="";

	cSQL="SELECT cValue FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='$cName'";
	cRetVal=` funcBEQuerySelect $cSQL `
	if [ $? != 0 ];then
		fLog "mysql command 4 failed";
		return "";
	fi
	echo $cRetVal;
}
#funcGetProperty()

#set backup passwords from main
cOrg_FreePBXAdminPasswd=`funcGetProperty $uContainer "cOrg_FreePBXAdminPasswd"`;
funcSetProperty $uRemoteContainer "cOrg_FreePBXAdminPasswd" $cOrg_FreePBXAdminPasswd;

cOrg_FreePBXEngPasswd=`funcGetProperty $uContainer "cOrg_FreePBXEngPasswd"`;
funcSetProperty $uRemoteContainer "cOrg_FreePBXEngPasswd" $cOrg_FreePBXEngPasswd;

cOrg_FreePBXOperatorPasswd=`funcGetProperty $uContainer "cOrg_FreePBXOperatorPasswd"`;
funcSetProperty $uRemoteContainer "cOrg_FreePBXOperatorPasswd" $cOrg_FreePBXOperatorPasswd;

#everything ok clean exit
rmdir $cContainerLock;
fLog "end $uContainer";
exit $uExitVal;
