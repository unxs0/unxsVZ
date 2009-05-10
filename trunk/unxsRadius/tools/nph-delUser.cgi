#!/bin/bash

PATH="/sbin:/usr/local/bin:/bin:/usr/bin:/usr/local/sbin:/usr/sbin"
export PATH

echo "HTTP/1.0 200 Transaction ok"
echo "Content-Type: text/plain"
echo ""
echo ""

if [ $1 .eq. ""]; then
	echo "usage: nph-delUser.cgi?login+passwd"
	exit
fi

echo "Login: $1"

radiusUsers.sh --del $1 

