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

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@" >> /tmp/clonesync.sh.log; }

#Note that you must change the rsync line also. Since we have not had time to fix this.
#cUseLVM="Yes";
cUseLVM="No";

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

fLog "start $1 to $3:$2";

#simple turn off
if [ -f "/root/clonesync.sh.off" ];then
	fLog "/root/clonesync.sh.off";
	exit 1;
fi

uRunning=`nice /bin/ps -ef | /bin/grep clonesync.sh | /bin/grep -v /bin/grep | /usr/bin/wc -l`;
if [ "$uRunning" -gt 5 ];then
	fLog "clonesync is already running $uRunning times";
	exit 1;
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
/usr/bin/ssh $3 "ls /vz/private/$2 > /dev/null 2>&1";
if [ $? != 0 ];then
	fLog "/usr/bin/ssh $3 ls /vz/private/$2 failed";
	#rollback
	rmdir $cContainerLock;
	exit 2;
fi

cBaseDir="vz";
if [ "$cUseLVM" == "Yes" ];then

	mkdir -p /mntsnapvol;

	cLVMLock="/tmp/unxsvz.lvm.lock";
	if [ -d $cLVMLock ]; then
		fLog "waiting for lock release $cLVMLock";
		rmdir $cContainerLock;
		exit 3;
	else
		mkdir $cLVMLock; 
	fi

	#create a stopped source veid dir
	cVZVolName=`/usr/sbin/lvs --noheadings -o lv_name 2>/dev/null| head -n 1 | awk '{print $1}'`;
	cVZMount=`/bin/mount | grep -w vz`;
	cVZVolGroup=`/usr/sbin/lvs --noheadings -o vg_name 2>/dev/null| head -n 1 | awk '{print $1}'`;
	cTest=`echo ${cVZMount} | grep ${cVZVolName}`;
	if [ "$cTest" != "$cVZMount" ];then
		fLog "Could not determine correct LVM vol name to use";
		rmdir $cLVMLock;
		rmdir $cContainerLock;
		exit 4;
	fi

	/usr/sbin/lvcreate --size 1G --snapshot --name snapvol /dev/$cVZVolGroup/$cVZVolName > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "lvcreate snapvol of vz failed";
		#see if we can recover
		cd /;
		/bin/umount /mntsnapvol;
		/usr/sbin/lvremove -f /dev/$cVZVolGroup/snapvol > /dev/null 2>&1;
		if [ $? != 0 ]; then
			fLog "lvremove snapvol fix failed";
		else
			/usr/sbin/lvcreate --size 1G --snapshot --name snapvol /dev/$cVZVolGroup/$cVZVolName > /dev/null 2>&1;
			if [ $? != 0 ]; then
				fLog "lvcreate snapvol second try failed";
				rmdir $cLVMLock;
				rmdir $cContainerLock;
				exit 5;
			fi
		fi
	fi

	/bin/mount /dev/$cVZVolGroup/snapvol /mntsnapvol > /dev/null 2>&1;
	if [ $? != 0 ];then
		fLog "mount failed";
		/usr/sbin/lvremove -f /dev/$cVZVolGroup/snapvol > /dev/null 2>&1;
		if [ $? != 0 ]; then
			fLog "lvremove rollback failed";
		fi
		rmdir $cLVMLock;
		rmdir $cContainerLock;
		exit 6;
	fi

	cBaseDir="mntsnapvol";
fi


#change /etc/ssh/ssh_conf for non standard port
#verbose for check
#/usr/bin/rsync -e '/usr/bin/ssh -ax -c blowfish' -avxlH --delete \
#/usr/bin/rsync -e '/usr/bin/ssh -ax -c blowfish' -axlH --delete \
#nice ionice with bwlimit and verbose for testing
#/bin/nice /usr/bin/ionice -c2 -n5 /usr/bin/rsync -avxlH --bwlimit=1000 --rsh '/usr/bin/ssh -ax -c blowfish' --delete \
#/usr/bin/rsync -axlH --rsh '/usr/bin/ssh -ax -c blowfish' --delete /vz/private/veid/ rc10:/vz/private/veid2
/bin/nice /usr/bin/ionice -c2 -n5 /usr/bin/rsync -axlH $cBWLimit --rsh '/usr/bin/ssh -ax -c blowfish' --delete \
			--exclude "/proc/" --exclude "/root/.ccache/" \
			--exclude "/sys" --exclude "/dev" --exclude "/tmp" \
			--exclude /etc/sysconfig/network \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:0 \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:1 \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:2 \
			/$cBaseDir/private/$1/ $3:/vz/private/$2 >> /tmp/clonesync.sh.log 2>&1;
#we can ignore return value 24:
#rsync warning: some files vanished before they could be transferred (code 24) at main.c(892) [sender=2.6.8]
uExitVal=0;
if [ $? != 0 ] && [ $? != 24 ];then
	fLog "rsync failed";
	uExitVal=9;
fi

if [ "$cUseLVM" == "Yes" ];then
	cd /;
	/bin/umount /mntsnapvol;
	if [ $? != 0 ]; then
		fLog "umount failed will retry in 60s";
		sleep 60;
		/bin/umount /mntsnapvol;
		if [ $? != 0 ]; then
			fLog "umount failed twice will retry in 120s";
			sleep 120;
			/bin/umount /mntsnapvol;
			if [ $? != 0 ]; then
				fLog "umount failed 3 times giving up";
				rmdir $cContainerLock;
				exit 7;
			fi
		fi
	fi

	/usr/sbin/lvremove -f /dev/$cVZVolGroup/snapvol > /dev/null 2>&1;
	if [ $? != 0 ]; then
		fLog "lvremove failed";
		rmdir $cContainerLock;
		exit 8;
	fi
	rmdir $cLVMLock;
fi

rmdir $cContainerLock;
fLog "end";
exit $uExitVal;
