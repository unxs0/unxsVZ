#!/bin/bash
#FILE
#       vzAllContainerCmd2.sh
#PURPOSE
#       Using vzlist run a command after vzctl enter into each one.
#AUTHOR
#       (C) 2010 Gary Wallis for Unixservice, LLC.
#       GPLv2 license applies. See LICENSE file.
#
#NOTES
#
#TODO
#

if [ "$1" == "" ];then
        echo "usage: $0 'command arg0 arg1...argN' [<hostname prefix>]";
        exit 0;
fi

cd /vz/private/;
for cVEID in `ls -1`; do
	uContainer=`/usr/sbin/vzlist --output ctid,hostname | grep -w $cVEID | awk -F' ' '{print $1}'`;
	if [ $? != 0 ];then
                continue;
        fi
        cHostname=`/usr/sbin/vzlist --output ctid,hostname | grep -w $cVEID | awk -F' ' '{print $2}'`;
	if [ $? != 0 ];then
                continue;
        fi
        if [ "$cHostname" == "" ];then
                continue;
        fi

        if [ "$2" != "" ];then
                if [[ ! "$cHostname" =~ $2 ]];then
                        continue;
                fi
        fi

        echo $uContainer:$cHostname;
       	/usr/sbin/vzctl exec2 $uContainer $1;

done

exit 0;
