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

cLockfile="/tmp/diffdata.sh.lock.$1.$2";
if [ -d $cLockfile ]; then
	fLog "waiting for lock release $cLockfile";
	exit 0;
else
	mkdir $cLockfile; 
fi

/usr/bin/diff --brief -r -x udev -x dev $cDir/$1 $cDir/$2 2>/dev/null 1>/tmp/diffdata.list;

while read cLine; do
	cFile=`echo ${cLine} | grep differ | cut -f 2 -d " "`;
	if [ "$cFile" != "" ];then
		echo $cFile;
	fi
	cDir=`echo ${cLine} | grep Only | cut -f 3 -d " " | cut -f 1 -d ":"`;
	cFile2=`echo ${cLine} | grep Only | cut -f 4 -d " "`;
	if [ "$cDir" != "" ] && [ "$cFile2" != "" ];then
		echo $cDir$cFile2;
	fi
done < /tmp/diffdata.list;

rmdir $cLockfile;
#debug only
fLog "end";
exit 0;
