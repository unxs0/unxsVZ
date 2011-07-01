#!/bin/bash

#FILE
#	tools/mcs/createhtml.sh
#PURPOSE
#	create hash based file for
#	poll control of remote mcs remote agents


if [ "$2" == "" ];then
	echo "usage: $0 <ra fqdn> <shared secret>";
	exit;
fi

cFile=`echo "$1$2" | md5sum | cut -f 1 -d " "`;

touch /var/www/html/$cFile;
if [ $? != 0 ];then
	echo "could not create or touch existing file /var/www/html/$cFile";
	exit 1;
fi
echo "/var/www/html/$cFile";
exit 0;
