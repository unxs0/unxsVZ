#!/bin/bash
#FILE
#       vzAllContainerArpsend.sh
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
        echo "usage: $0 'command arg0 arg1...argN' [<hostname prefix> | <VEID>]";
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
        cIP=`/usr/sbin/vzlist --output ctid,ip | grep -w $cVEID | awk -F' ' '{print $2}'`;
	if [ $? != 0 ];then
		continue;
	fi

        if [ "$2" != "" ] ;then
                if [[ ! "$cHostname" =~ $2 ]] && [ "$2" != "$uContainer" ];then
                        continue;
                fi
        fi

        echo $uContainer:$cHostname:$cIP;
	/usr/sbin/arpsend -c 3 -U -i $cIP eth0;

done

exit 0;
