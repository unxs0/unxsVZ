#!/bin/bash
#
#FILE
#	tools/rsync/osdeltasync.sh
#PURPOSE
#	Using a common base dir
#	create an xz file of only
#	those files different from
#	source dir
#	Move that xz file to remote
#	server.
#	Install files on target veid
#NOTES
#	Source veid will be vzdump'd to get a consistent
#	snapshot. LVM should be setup correctly.
#
#	Base veid should be of a stopped container or equiv.


#Settings
cSSHPort="22";
cDir="/vz/private";

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

#Low priority script do not run when server is loaded.
nLoad=`uptime | awk '{print $10}'| cut -f 1 -d .`;
if (( $nLoad > 5 ));then
	fLog "waiting for $nLoad to go below 5";
	exit 0;
fi

if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ];then
	echo "usage: $0 <source veid> <clone veid> <remote server> [<ssh port>]";
	exit 0;
fi

if [ "$4" != "" ];then
	cSSHPort="$4";
fi

#All datacente nodes must have special stopped containers that are
#the base OS template of reference
cOSTemplate=`vzlist --no-header --output ostemplate $1 | awk '{print $1}'`;
cOSTemplateVEID=`vzlist --no-header --output veid $cOSTemplate | awk '{print $1}'`;

fLog "start $1 $2 $3:$cSSHPort $cOSTemplate $cOSTemplateVEID";

#Pre qualify to avoid load for no reason.
if [ ! -d "$cDir/$1" ];then
	fLog "$cDir/$1 is not a dir";
	exit 1;
fi

if [ ! -d "$cDir/$cOSTemplateVEID" ];then
	fLog "$cDir/$cOSTemplateVEID is not a dir";
	exit 1;
fi

if [ ! -x "/usr/sbin/lvcreate" ];then
	fLog "lvcreate not found";
	exit 1;
fi

if [ ! -x "/usr/sbin/lvremove" ];then
	fLog "lvcreate not found";
	exit 1;
fi

/usr/bin/ssh -c arcfour -p $cSSHPort $3 ls > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "ssh test to $3:$cSSHPort failed";
	exit 1;
fi

cLockfile="/tmp/osdeltasync.sh.lock";
if [ -d $cLockfile ]; then
	fLog "waiting for lock release $cLockfile";
	exit 0;
else
	mkdir $cLockfile; 
fi

#create a stopped source veid dir
cVZVolName=`/usr/sbin/lvs --noheadings -o lv_name | head -n 1 | awk '{print $1}'`;
cVZMount=`/bin/mount | grep -w vz`;
cVZVolGroup=`/usr/sbin/lvs --noheadings -o vg_name | head -n 1 | awk '{print $1}'`;
cTest=`echo ${cVZMount} | grep ${cVZVolName}`;
if [ "$cTest" != "$cVZMount" ];then
	fLog "Could not determine correct LVM vol name to use";
	rmdir $cLockfile;
	exit 1;
fi
##debug only
#echo "$cTest /dev/$cVZVolGroup/$cVZVolName";
#rmdir $cLockfile;
#exit 0;
/usr/sbin/lvcreate --size 1G --snapshot --name snapvol /dev/$cVZVolGroup/$cVZVolName;
if [ $? != 0 ];then
	fLog "lvcreate snapvol of vz failed";
	rmdir $cLockfile;
	exit 1;
fi

/bin/mount /dev/$cVZVolGroup/snapvol /mnt;
if [ $? != 0 ];then
	fLog "mount failed";
	rmdir $cLockfile;
	exit 1;
fi

cat /dev/null > /tmp/osdeltasync.list;
cd /mnt;
/usr/bin/diff --brief -r -x udev -x dev private/$1 $cDir/$cOSTemplateVEID 2>/dev/null 1>/tmp/osdeltasync.list;
if [ ! -s /tmp/osdeltasync.list ];then
	fLog "/tmp/osdeltasync.list empty";
	rmdir $cLockfile;
	exit 1;
fi

cat /dev/null > /tmp/osdeltasync.files;
while read cLine; do
	cFile=`echo ${cLine} | /bin/grep differ | cut -f 2 -d " "`;
	if [ "$cFile" != "" ];then
		echo $cFile >> /tmp/osdeltasync.files;
	fi
	cDir=`echo ${cLine} | /bin/grep Only | /bin/grep "/$1/" | cut -f 3 -d " " | cut -f 1 -d ":"`;
	cFile2=`echo ${cLine} | /bin/grep Only | /bin/grep "/$1/" | cut -f 4 -d " "`;
	if [ "$cDir" != "" ] && [ "$cFile2" != "" ];then
		echo $cDir/$cFile2 >> /tmp/osdeltasync.files;
	fi
done < /tmp/osdeltasync.list;
if [ ! -s /tmp/osdeltasync.files ];then
	fLog "/tmp/osdeltasync.files empty";
	rmdir $cLockfile;
	exit 1;
fi

cat /tmp/osdeltasync.files | /usr/bin/xargs /bin/tar cf /tmp/osdeltasync.tar > /dev/null 2>&1;
if [ $? != 0 ]; then
	fLog "tar failed";
	rmdir $cLockfile;
	exit 1;
fi

#we don;t remove the lock file here to insure this
#is fixed.
cd /;
/bin/umount /mnt;
if [ $? != 0 ]; then
	fLog "umount failed";
	exit 1;
fi
/usr/sbin/lvremove -f /dev/$cVZVolGroup/snapvol;
if [ $? != 0 ]; then
	fLog "lvremove failed";
	exit 1;
fi

/usr/bin/xz -f /tmp/osdeltasync.tar;
if [ $? != 0 ]; then
	fLog "xz failed";
	rmdir $cLockfile;
	exit 1;
fi

/usr/bin/scp -c arcfour -P $cSSHPort /tmp/osdeltasync.tar.xz $3:/tmp/osdeltasync.$1.tar.xz > /dev/null;
if [ $? != 0 ];then
	fLog "scp failed";
	rmdir $cLockfile;
	exit 1;
fi

#now we patch the clone file system with the tar
/usr/bin/ssh -c arcfour -p $cSSHPort $3 "cd /vz;unxz /tmp/osdeltasync.$1.tar.xz;tar xf /tmp/osdeltasync.$1.tar" > /dev/null;
if [ $? != 0 ];then
	fLog "ssh tar failed";
	rmdir $cLockfile;
	exit 1;
fi


rmdir $cLockfile;
#debug only
fLog "end";
exit 0;
