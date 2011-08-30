#!/bin/bash
#
#FILE
#	tools/rsync/diffdata.sh
#PURPOSE
#	Using a common base dir
#	create an xz file of only
#	those files different from
#	source dir
#	Move that xz file to remote
#	server.
#NOTES


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
	echo "usage: $0 <source veid> <base veid> <remote server> [<ssh port>]";
	exit 0;
fi

if [ "$4" != "" ];then
	cSSHPort="$4";
fi

#debug only
fLog "start $1 $2";

cLockfile="/tmp/diffdata.sh.lock";
if [ -d $cLockfile ]; then
	fLog "waiting for lock release $cLockfile";
	exit 0;
else
	mkdir $cLockfile; 
fi

cat /dev/null > /tmp/diffdata.list;
/usr/bin/diff --brief -r -x udev -x dev $cDir/$1 $cDir/$2 2>/dev/null 1>/tmp/diffdata.list;
if [ ! -s /tmp/diffdata.list ];then
	fLog "/tmp/diffdata.list empty";
	rmdir $cLockfile;
	exit 1;
fi

cat /dev/null > /tmp/diffdata.files;
while read cLine; do
	cFile=`echo ${cLine} | /bin/grep differ | cut -f 2 -d " "`;
	if [ "$cFile" != "" ];then
		echo $cFile >> /tmp/diffdata.files;
	fi
	cDir=`echo ${cLine} | /bin/grep Only | /bin/grep "/$1/" | cut -f 3 -d " " | cut -f 1 -d ":"`;
	cFile2=`echo ${cLine} | /bin/grep Only | /bin/grep "/$1/" | cut -f 4 -d " "`;
	if [ "$cDir" != "" ] && [ "$cFile2" != "" ];then
		echo $cDir/$cFile2 >> /tmp/diffdata.files;
	fi
done < /tmp/diffdata.list;
if [ ! -s /tmp/diffdata.files ];then
	fLog "/tmp/diffdata.files empty";
	rmdir $cLockfile;
	exit 1;
fi

cat /tmp/diffdata.files | /usr/bin/xargs /bin/tar cf /tmp/diffdata.tar > /dev/null 2>&1;
if [ $? != 0 ]; then
	fLog "tar failed";
	rmdir $cLockfile;
	exit 1;
fi

/usr/bin/xz -f /tmp/diffdata.tar;
if [ $? != 0 ]; then
	fLog "xz failed";
	rmdir $cLockfile;
	exit 1;
fi

/usr/bin/scp -c arcfour -P $cSSHPort /tmp/diffdata.tar.xz $3:/tmp/diffdata.$1.tar.xz > /dev/null;
if [ $? != 0 ];then
	fLog "scp failed";
	rmdir $cLockfile;
	exit 1;
fi

rmdir $cLockfile;
#debug only
fLog "end";
exit 0;
