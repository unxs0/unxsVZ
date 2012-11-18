#!/bin/bash
#FILE
#       AllPBXContainerCmd.sh
#PURPOSE
#       Using db run a command via vzctl enter into each one.
#AUTHOR
#       (C) 2010 Gary Wallis for Unixservice, LLC.
#       GPLv2 license applies. See LICENSE file.
#
#NOTES
#
#TODO
#

cHost="sx1";
cPasswd="KJahsdyuer73jihsdf";
cGroupStatement="(tGroup.cLabel='Production PBXs')";
cMySQLConnect="/usr/bin/mysql -h $cHost -u unxsvz -p$cPasswd unxsvz";

if [ "$1" == "" ];then
	echo "usage: $0 <command> [<VEID>]";
	exit 0;
fi

cShortHostname=`hostname -s`;
if [ $? != 0 ];then
	echo "hostname -s failed";
	exit;
fi
uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cShortHostname'" | $cMySQLConnect | grep -v uNode`;
if [ $? != 0 ];then
	echo "Select node failed";
	exit;
fi
if [ "$uNode" == "" ];then
	echo "uNode failed";
	exit;
fi

for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uNode=$uNode AND tContainer.uStatus=1" | $cMySQLConnect | grep -v uContainer`;do

	#Optional VEID and setup
	
	if [ "$2" != "" ] && [ "$uContainer" != "$2" ];then
		continue;
	fi

        /usr/sbin/vzctl exec2 $uContainer $1;

done

exit 0;
