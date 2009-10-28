#!/bin/bash
#
#FILE
#	tools/rsync/clonesync.sh
#PURPOSE
#	keep a cloned unxsVZ container on diff node rsync'd
#NOTES
#	This is a general sync script. Certain types of
#	containers can be kept in sync much faster by only
#	rsync'ing specific container service and log files.
#	For example an Asterix FreePBX container can be analyzed
#	to see what actually changes over a long period and then
#	have this script modified just for the service important items.


fLog()
{
	echo "`date +%b' '%d' '%T` clonesync.sh.$$ $@";
}


cSSHPort="-p 12337";

if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ];then
	echo "usage: $0 <source VEID> <target VEID> <target node host>";
	exit 0;
fi

fLog "start $1 to $3:$2";

cLockfile="/tmp/clonesync.sh.lock.$1.$2";

#do not run if another (same source and target VEIDs) clone job is also running
if [ -e $cLockfile ]; then
	fLog "waiting for lock release";
	exit 0;
else
	touch $cLockfile; 
fi

#flush as much as possible to disk.
#special services could have hotcopy type commands here.
#if corruptible files will be transferred we need to vzdump with LVM
#and rsync from a dir structure taken from that snaphot.
#this is very slow and HD space consuming.
#so we test as herein a quick and dirty or a targeted single service type
#container approach
/usr/sbin/vzctl exec $1 /bin/sync;
if [ $? != 0 ];then
	fLog  "/usr/sbin/vzctl exec $1 /bin/sync failed";
	rm -f $cLockfile;
	exit 1;
fi

/usr/bin/ssh $cSSHPort $3 "ls /vz/private/$2 > /dev/null 2>&1";
if [ $? != 0 ];then
	fLog "/usr/bin/ssh $3 ls /vz/private/$2 failed";
	rm -f $cLockfile;
	exit 2;
fi

#/usr/bin/rsync -e '/usr/bin/ssh -ax -c blowfish -p 12337' -avxzlH --delete \
#no compression
#/usr/bin/rsync -e '/usr/bin/ssh -ax -c blowfish -p 12337' -avxlH --delete \
#no verbose and no encryption
/usr/bin/rsync -e '/usr/bin/ssh -ax -e none -p 12337' -axlH --delete \
			--exclude "/proc/" --exclude "/root/.ccache/" \
			--exclude "/sys" --exclude "/dev" --exclude "/tmp" \
			--exclude /etc/sysconfig/network \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:0 \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:1 \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:2 \
			/vz/private/$1/ $3:/vz/private/$2

#remove lock file
rm -f $cLockfile;

fLog "end";
exit 0;

