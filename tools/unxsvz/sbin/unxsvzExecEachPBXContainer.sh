#!/bin/bash

#FILE
#	unxsvzExecEachPBXContainer.sh
#PURPOSE
#	Very simple script to run a command inside each PBX
#	container.
#AUTHOR
#	(C) 2011-2014 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

if [ "$1" == "" ];then
	echo "usage: $0 <command no spaces unless quoted>";
	exit 0;
fi

cMySQLConnect="";
if [ -f "/etc/unxsvz/mysql.local.sh" ];then
	source /etc/unxsvz/mysql.local.sh;
else
	echo "no /etc/unxsvz/mysql.local.sh";
	exit 1;
fi
if [ -f "/etc/unxsvz/unxsvz.functions.sh" ];then
	source /etc/unxsvz/unxsvz.functions.sh;
else
	echo "no /etc/unxsvz/unxsvz.functions.sh";
	exit 1;
fi
if [ "$cMySQLConnect" == "" ];then
	fLog "no cMySQLConnect";
	exit 1;
fi

cNode=`hostname -s`;
if [ $? != 0 ];then
	fLog "cNode hostname error";
	exit;
fi
uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cNode'"|$cMySQLConnect -B -N`;
if [ $? != 0 ];then
	fLog "uNode SELECT error";
	exit;
fi
if [ "$uNode" == "" ];then
	fLog "uNode could not be determined for $cNode";
	exit;
fi
fLog "Start for $cNode/$uNode";

for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uNode=$uNode AND tContainer.uSource=0 AND tContainer.uStatus=1"|$cMySQLConnect -B -N`;do
	echo $uContainer;

        /usr/sbin/vzctl exec2 $uContainer $1;
        if [ $? != 0 ];then
                echo "vzctl exec2 $uContainer error";
        fi
done
fLog "End";
