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
#	server on initial setup.
#	Else we rsync based on list of files.
#	Install files on target veid
#NOTES
#	LVM is required with enough free space for snapshot.
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
cOSTemplate=`/usr/sbin/vzlist --no-header --output ostemplate $1 | awk '{print $1}'`;
if [ $? != 0 ] || [ "$cOSTemplate" == "" ];then
	fLog "cOSTemplate error";
	exit 1;
fi
cOSTemplateVEID=`/usr/sbin/vzlist --no-header --output veid $cOSTemplate | awk '{print $1}'`;
if [ $? != 0 ] || [ "$cOSTemplateVEID" == "" ];then
	fLog "cOSTemplateVEID error";
	exit 1;
fi

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

/usr/bin/ssh -c arcfour -p $cSSHPort $3 "unxz --help" > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "ssh test to $3:$cSSHPort failed";
	exit 1;
fi

cLockfile="/tmp/unxsvz.lvm.lock";
if [ -d $cLockfile ]; then
	fLog "waiting for lock release $cLockfile";
	exit 2;
else
	mkdir $cLockfile; 
fi

fCleanExit ()
{
	rmdir $cLockfile;
	#debug only
	fLog "end";
	exit 0;
}

fErrorExit ()
{
	rmdir $cLockfile;
	fLog $@;
	exit 3;
}

#create a stopped source veid dir
cVZVolName=`/usr/sbin/lvs --noheadings -o lv_name | head -n 1 | awk '{print $1}'`;
cVZMount=`/bin/mount | /bin/grep -w vz`;
cVZVolGroup=`/usr/sbin/lvs --noheadings -o vg_name | head -n 1 | awk '{print $1}'`;
cTest=`echo ${cVZMount} | /bin/grep ${cVZVolName}`;
if [ "$cTest" != "$cVZMount" ];then
	fLog "Could not determine correct LVM vol name to use";
	rmdir $cLockfile;
	exit 4;
fi

fUnLVM ()
{
	cd /;
	/bin/umount /mntsnapvol;
	if [ $? != 0 ]; then
		fLog "umount failed";
		exit 5;
	fi
	/usr/sbin/lvremove -f /dev/$cVZVolGroup/snapvol > /dev/null;
	if [ $? != 0 ]; then
		fLog "lvremove failed";
		exit 6;
	fi
}

##debug only
#echo "$cTest /dev/$cVZVolGroup/$cVZVolName";
#rmdir $cLockfile;
#exit 0;
/usr/sbin/lvcreate --size 1G --snapshot --name snapvol /dev/$cVZVolGroup/$cVZVolName > /dev/null;
if [ $? != 0 ];then
	fLog "lvcreate snapvol of vz failed";
	rmdir $cLockfile;
	exit 7;
fi

/bin/mount /dev/$cVZVolGroup/snapvol /mntsnapvol;
if [ $? != 0 ];then
	fLog "mount failed";
	rmdir $cLockfile;
	exit 8;
fi

cLockfile="/tmp/unxsvz.lvm.lock";

cat /dev/null > /tmp/osdeltasync.list;
cd /mntsnapvol;
#/usr/bin/rsync --dry-run -avxlH \
/usr/bin/rsync --dry-run -avx \
			--exclude "/proc/" --exclude "/root/.ccache/" \
			--exclude "/sys" --exclude "/dev" --exclude "/tmp" \
			--exclude /etc/sysconfig/network \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:0 \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:1 \
			--exclude /etc/sysconfig/network-scripts/ifcfg-venet0:2 \
			--exclude /var/www/html/avantfax/tmp \
			--exclude /var/lib/php/session/ \
				private/$1/ $cDir/$cOSTemplateVEID/ > /tmp/osdeltasync.list;
if [ ! -s /tmp/osdeltasync.list ];then
	fLog "/tmp/osdeltasync.list empty";
	rmdir $cLockfile;
	exit 9;
fi

cat /dev/null > /tmp/osdeltasync.files;
while read cLine; do
	cFile=`echo ${cLine} | /bin/grep -v "/$" | /bin/grep -v "^building" | /bin/grep -v "^sent" | /bin/grep -v "^total" `;
	if [ "$cFile" != "" ];then
		#remove symlink parts
		echo $cFile | /bin/sed -e "s/ ->.*//g" | sed -e "s/.*/\"&\"/g" >> /tmp/osdeltasync.files;
	fi
done < /tmp/osdeltasync.list;
if [ ! -s /tmp/osdeltasync.files ];then
	fLog "/tmp/osdeltasync.files empty";
	rmdir $cLockfile;
	exit 10;
fi

#debug only
#fUnLVM;
#fErrorExit "testing part one";

#if we already created remote osdeltasync dir, then we rsync
#else we transfer over the whole thing
/usr/bin/ssh -c arcfour -p $cSSHPort $3 "ls /vz/private/$1/osdeltasync.flag" > /dev/null 2>&1;
if [ $? == 0 ];then
	fUnLVM;
	#rsync only the files directly and install
	#must configure /etc/ssh/ssh_config correctly if using non standard ssh port
	/usr/bin/rsync -e '/usr/bin/ssh -ax -c blowfish' -avxlH\
			 --files-from=/tmp/osdeltasync.files /vz/private/$1/ $3:/vz/private/$1
	if [ $? != 0 ];then
		fLog "rsync /vz/private/$1/ $3:/vz/private/$1 error";
	fi

	#now we update the clone file system with the tar
	#if target is warm or hot we rsync files. if target is cold the $2 dir does not exist
	/usr/bin/ssh -c arcfour -p $cSSHPort $3\
		"if [ -d /vz/private/$2 ];then rsync -axlH /vz/private/$1/ /vz/private/$2;fi;"
						 > /dev/null;
	if [ $? != 0 ];then
		fLog "ssh conditional rsync on $3 failed";
		rmdir $cLockfile;
		exit 11;
	fi
else
	fLog "scp tar.xz and install";
	cd /mntsnapvol/private/$1;
	if [ $? != 0 ];then
		fUnLVM;
		fErrorExit "cd private/$1 failed";
	fi
	cat /tmp/osdeltasync.files | /usr/bin/xargs /bin/tar cf /tmp/osdeltasync.tar;
	if [ $? != 0 ]; then
		fUnLVM;
		fErrorExit "tar failed";
	fi
	fUnLVM;

	/usr/bin/xz -f /tmp/osdeltasync.tar;
	if [ $? != 0 ]; then
		fLog "xz failed";
		rmdir $cLockfile;
		exit 12;
	fi

	/usr/bin/scp -c arcfour -P $cSSHPort /tmp/osdeltasync.tar.xz $3:/tmp/osdeltasync.$1.tar.xz > /dev/null;
	if [ $? != 0 ];then
		fLog "scp failed";
		rmdir $cLockfile;
		exit 13;
	fi

	#now we update the clone file system with the tar
	#if target is warm or hot we rsync files. if target is cold the $2 dir does not exist
	/usr/bin/ssh -c arcfour -p $cSSHPort $3\
		"mkdir -p /vz/private/$1;"\
		"cd /vz/private/$1;touch osdeltasync.flag;"\
		"unxz /tmp/osdeltasync.$1.tar.xz;"\
		"tar xf /tmp/osdeltasync.$1.tar;"\
		"rm /tmp/osdeltasync.$1.tar;"\
		"if [ -d /vz/private/$2 ];then rsync -axlH /vz/private/$1/ /vz/private/$2;fi;"
						 > /dev/null;
	if [ $? != 0 ];then
		fLog "ssh tar failed";
		rmdir $cLockfile;
		exit 14;
	fi
#end of if rsync or scp
fi

fCleanExit;

