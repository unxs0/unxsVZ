#!/bin/bash

#FILE
#       tools/mcs/getcommand.sh
#PURPOSE
#       Poll remote http hash based file for
#       control of local mcs remote agent



cSharedSecret="topsecret";
cMCSServerFQDN="mcs.example.com";
cMCSServerPort="80";

if [ "$1" == "" ];then
	echo "usage: $0 run";
	exit;
fi

cHostname=`/bin/hostname -f`;
if [ $? != 0 ];then
        echo "cHostname error";
        exit 1;
fi

cFile=`echo "$cHostname$cSharedSecret" | /usr/bin/md5sum | /bin/cut -f 1 -d " "`;
if [ $? != 0 ];then
        echo "cFile error";
        exit 1;
fi

cResponse=`/usr/bin/wget --quiet --no-check-certificate --output-document=-\
         https://$cMCSServerFQDN:$cMCSServerPort/$cFile`;
#cResponse=`/usr/bin/wget --no-check-certificate --output-document=-\
#         https://$cMCSServerFQDN:$cMCSServerPort/$cFile`;
if [ $? != 0 ];then
        echo "wget error 0";
        exit 1;
fi

echo $cResponse;
