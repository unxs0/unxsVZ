#!/bin/bash
#FILE
#       vzAllContainerInstall.sh
#PURPOSE
#       Using vzlist copy a file into every container
#AUTHOR
#       (C) 2010 Gary Wallis for Unixservice, LLC.
#       GPLv2 license applies. See LICENSE file.
#
#NOTES
#
#TODO
#

if [ "$1" == "" ] || [ "$2" == "" ];then
        echo "usage: $0 <src file> <container file dest>";
        exit 0;
fi

/usr/sbin/vzlist | grep running | while read cvzlist
do
        uContainer=`echo $cvzlist | awk -F' ' '{print $1}'`;

        echo $uContainer;
	cp $1 /vz/private/$uContainer$2;

done

exit 0;
