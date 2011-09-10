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

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

#Note that you must change the rsync line also. Since we have not had time to fix this.
cUseLVM="Yes";

if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ];then
	echo "usage: $0 <source VEID> <target VEID> <target node host>";
	exit 0;
fi

#debug only
#fLog "start $1 to $3:$2";
uRunning=`nice /bin/ps -ef | /bin/grep clonesync | /bin/grep -v /bin/grep | /usr/bin/wc -l`;
if [ "$uRunning" -gt 5 ];then
	fLog "clonesync is already running $uRunning times";
	exit 0;
fi

cLockfile="/tmp/clonesync.sh.lock.$1.$2";

#do not run if another (same source and target VEIDs) clone job is also running
if [ -e $cLockfile ]; then
	fLog "waiting for lock release $cLockfile";
	exit 0;
else
	touch $cLockfile; 
fi

#make sure ssh is working
#change /etc/ssh/ssh_conf for non standard port
/usr/bin/ssh $3 "ls /vz/private/$2 > /dev/null 2>&1";
if [ $? != 0 ];then
	fLog "/usr/bin/ssh $3 ls /vz/private/$2 failed";
	#rollback
	rm -f $cLockfile;
	exit 2;
fi

cBaseDir="vz";
if [ "$cUseLVM" == "Yes" ];then

	cLVMLock="/tmp/unxsvz.lvm.lock";
	if [ -d $cLVMLock ]; then
		fLog "waiting for lock release $cLVMLock";
		rm -f $cLockfile;
		exit 0;
	else
		mkdir $cLVMLock; 
	fi

	#create a stopped source veid dir
	cVZVolName=`/usr/sbin/lvs --noheadings -o lv_name | head -n 1 | awk '{print $1}'`;
	cVZMount=`/bin/mount | grep -w vz`;
	cVZVolGroup=`/usr/sbin/lvs --noheadings -o vg_name | head -n 1 | awk '{print $1}'`;
	cTest=`echo ${cVZMount} | grep ${cVZVolName}`;
	if [ "$cTest" != "$cVZMount" ];then
		fLog "Could not determine correct LVM vol name to use";
		rmdir $cLVMLock;
		rm -f $cLockfile;
		exit 1;
	fi

	/usr/sbin/lvcreate --size 1G --snapshot --name snapvol /dev/$cVZVolGroup/$cVZVolName > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "lvcreate snapvol of vz failed";
		rmdir $cLVMLock;
		rm -f $cLockfile;
		exit 1;
	fi

	/bin/mount /dev/$cVZVolGroup/snapvol /mnt > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "mount failed";
		rmdir $cLVMLock;
		rm -f $cLockfile;
		exit 1;
	fi

	cBaseDir="mnt";
fi


#change /etc/ssh/ssh_conf for non standard port
#verbose for check
#/usr/bin/rsync -e '/usr/bin/ssh -ax -c blowfish' -avxlH --delete \
/usr/bin/rsync -e '/usr/bin/ssh -ax -c blowfish' -axlH --delete \
			--exclude "/proc/" --exclude "/root/.ccache/" \
			--exclude "/sys" --exclude "/dev" --exclude "/tmp" \
			--exclude /etc/sysconfig/network \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:0 \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:1 \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:2 \
			/$cBaseDir/private/$1/ $3:/vz/private/$2
#we can ignore return value 24:
#rsync warning: some files vanished before they could be transferred (code 24) at main.c(892) [sender=2.6.8]
if [ $? != 0 ] && [ $? != 24 ];then
	fLog "rsync failed";
	uExitVal=3;
fi

if [ "$cUseLVM" == "Yes" ];then
	cd /;
	/bin/umount /mnt;
	if [ $? != 0 ]; then
		fLog "umount failed";
		exit 1;
	fi
	/usr/sbin/lvremove -f /dev/$cVZVolGroup/snapvol > /dev/null 2>&1;
	if [ $? != 0 ]; then
		fLog "lvremove failed";
		exit 1;
	fi
	rmdir $cLVMLock;
fi

rm -f $cLockfile;
exit $uExitVal;
