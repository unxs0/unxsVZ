#!/bin/bash
#FILE
#       vzAllContainerCmd.sh
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

#      CTID      NPROC STATUS  IP_ADDR         HOSTNAME
#       181         48 running 174.34.136.100 ns1.singtone.com
/usr/sbin/vzlist | grep running | \
while read cvzlist
do
        uContainer=`echo $cvzlist | awk -F' ' '{print $1}'`;
        #cIP=`echo $cvzlist | awk -F' ' '{print $4}'`;
        cHostname=`echo $cvzlist | awk -F' ' '{print $5}'`;
        #cLabel=`echo $cHostname | awk -F. '{print $1}'`;
        #cOSTemplate=`cat /etc/vz/conf/$uContainer.conf | grep OSTEMPLATE | awk -F= '{print $2}' | awk -F'"' '{print $2}'`;
        #cConfig=`cat /etc/vz/conf/$uContainer.conf | grep ORIGIN_SAMPLE | awk -F= '{print $2}' | awk -F'"' '{print $2}'`;


        #echo cLabel=$cLabel;
        #echo cHostname=$cHostname;
        #echo cIP=$cIP;
        #echo cOSTemplate=$cOSTemplate;
        #echo cConfig=$cConfig;

	if [ "$2" != "" ];then
		if [[ ! "$cHostname" =~ $2 ]];then
			continue;
		fi
	fi

        echo $uContainer:$cHostname;
        /usr/sbin/vzctl exec2 $uContainer $1 < /dev/tty;

done

exit 0;
