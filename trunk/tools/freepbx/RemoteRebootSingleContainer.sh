#!/bin/bash

#based on Ricardo's script /usr/sbin/PostDNSNode.sh

if [ "$1" == "" ];then
	echo "usage: $0 <uVEID>";
	exit;
fi

vid="$1";

extensions=` /usr/sbin/vzctl exec2 $vid 'asterisk -rx "sip show peers"|grep OK|cut  -f1 -d/' `
for e in $extensions
do
	useragents[$e]=` /usr/sbin/vzctl exec2 $vid "asterisk -rx 'sip show peer $e'|grep Useragent" `
	echo $e ${useragents[$e]}
	case ${useragents[$e]} in
		*Yealink*) /usr/sbin/vzctl exec2 $vid "asterisk -rx 'sip notify yealink-check-cfg $e'";;
	esac
done
