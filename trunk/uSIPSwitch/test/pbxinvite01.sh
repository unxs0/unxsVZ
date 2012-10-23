#!/bin/bash

if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ];then
	echo "usage: $0 <proxy ip[:port]> <listen ip> <listen port>";
	exit;
fi

if [ -d test ];then
	cd test;
fi

cCallID=`head -c 256 /dev/urandom | md5sum | /bin/cut -c1-32`;
if [ $? != 0 ] || [ "$cCallID" == "" ];then
	echo "random Call-ID creation failed";
	exit 1;
fi

echo $cCallID;

cat pbxinvite01.sip.src | sed -e "s/_sipnum/$1/g" -e "s/_lipnum/$2/g" -e "s/_lport/$3/g" -e "s/_callid/$cCallID/g"> pbxinvite01.sip.src.tmp;
sipsak -vvv -s sip:101@$1 -l $3 -S -f pbxinvite01.sip.src.tmp;
rm -f pbxinvite01.sip.src.tmp;
