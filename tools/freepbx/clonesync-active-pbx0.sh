#!/bin/bash
#
#FILE
#	clonesync-remote.sh
#PURPOSE
#	keep a cloned live pbx container
#NOTES
#	This is an asterisk/freepbx specific sync script.
#	Example script for use with unxsVZ virtualization
#	manager.
#LEGAL
#	Copyright 2013 Gary Wallis and Ricardo Armas for Unixservice, LLC.
#	GPLv2 license applies.

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }
echo "`date +%b' '%d' '%T` $0[$$]: $@" >> /tmp/clonesync-active.log 2>&1

if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ];then
	echo "usage: $0 <source VEID> <target VEID> <target node host>";
	exit 0;
fi
uContainer=$1;
uRemoteContainer=$2;
cTargetNode=$3;
cLocalNetFileSpec="/etc/unxsvz/asterisk/localnet";
cExternIpFileSpec="/etc/unxsvz/asterisk/externip";
cBindPortFileSpec="/etc/unxsvz/asterisk/bindport";

/usr/sbin/vzlist | grep $uContainer  > /dev/null 2>&1;
if [ $? != 0 ];then
        fLog "$uContainer not running. Will not sync.";
	#exit code 7 is temporary failure. Should be used by unxsVZ jobqueue.c to
	#maek sure backup date is NOT updated!
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
#copy dump to target
/usr/bin/scp /vz/private/$uContainer/tmp/dump.sql $cTargetNode:/vz/private/$uRemoteContainer/tmp/dump.sql \
		2>> /tmp/clonesync-active.log 1> /dev/null

#restore on target
/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer \"cat /tmp/dump.sql | mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk\";" \
		2>> /tmp/clonesync-active.log 1> /dev/null

#get last seq from sipsettings
cSQL="SELECT max(seq) FROM sipsettings WHERE type=9"
uLastSeq=` funcQuerySelect $cSQL `
#echo $uLastSeq

if [ $uLastSeq = "NULL" ];then
	uNextSeq=0;
else
	#echo $uNextSeq
	let uNextSeq=$uLastSeq+1;
fi

#echo nextseq: $uNextSeq


#
#localnet setup
cLocalNet=`/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec $uRemoteContainer cat '$cLocalNetFileSpec'"`;
cSQL="INSERT INTO sipsettings SET keyword='\''localnet'\'',data='\''$cLocalNet'\'',seq=$uNextSeq,type=9";
funcQuery $cSQL;
if [ $? != 0 ];then
	cSQL="UPDATE sipsettings SET data='\''$cLocalNet'\'',seq=0,type=9 WHERE keyword='\''localnet'\''";
	funcQuery $cSQL;
	if [ $? != 0 ];then
		echo "insert and update failed";
		exit 1;
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
                echo "insert and update failed";
                exit 1;
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
                echo "insert and update failed";
                exit 1;
        fi
fi
#bindport setup end
#

#only log errors
#retrieve conf
/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer \"/var/lib/asterisk/bin/retrieve_conf\""\
		2>> /tmp/clonesync-active.log 1> /dev/null
#stop asterisk and freepbx
/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer \"amportal stop\""\
		2>> /tmp/clonesync-active.log 1> /dev/null
#sync etc/asterisk
/usr/bin/rsync -axlH  --rsh '/usr/bin/ssh -ax -c arcfour' --delete /vz/private/$1/etc/asterisk/ $3:/vz/private/$2/etc/asterisk/\
		2>> /tmp/clonesync-active.log 1> /dev/null
#sync spool
/usr/bin/rsync -axlH  --rsh '/usr/bin/ssh -ax -c arcfour' --delete /vz/private/$1/var/spool/asterisk/ $3:/vz/private/$2/var/spool/asterisk\
		2>> /tmp/clonesync-active.log 1> /dev/null
#sync www
/usr/bin/rsync -axlH  --rsh '/usr/bin/ssh -ax -c arcfour' --delete /vz/private/$1/var/www/ $3:/vz/private/$2/var/www/\
		2>> /tmp/clonesync-active.log 1> /dev/null
#sync var/lib/asterisk
/usr/bin/rsync -axlH  --rsh '/usr/bin/ssh -ax -c arcfour' --delete /vz/private/$1/var/lib/asterisk/ $3:/vz/private/$2/var/lib/asterisk/\
		2>> /tmp/clonesync-active.log 1> /dev/null
#restore rtp.conf 
/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer \"cp /etc/unxsvz/asterisk/rtp.conf  /etc/asterisk/rtp.conf\""\
		2>> /tmp/clonesync-active.log 1> /dev/null
#start asterisk and freepbx again
/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer \"amportal start\""\
		2>> /tmp/clonesync-active.log 1> /dev/null
#reload config
/usr/bin/ssh $cTargetNode "/usr/sbin/vzctl exec2 $uRemoteContainer \"/var/lib/asterisk/bin/module_admin reload\""\
		2>> /tmp/clonesync-active.log 1> /dev/null

#everything ok clean exit
rmdir $cContainerLock;
exit $uExitVal;
