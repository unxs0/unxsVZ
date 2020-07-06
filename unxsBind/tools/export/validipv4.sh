#!/bin/bash

# Test an IP address for validity:
#
function valid_ip()
{
    local  ip=$1

    if [[ ! "$ip" =~ ^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])$ ]];then
	echo "$ip invalid";
    else
	echo "$ip valid";
    fi
}

# If run directly, execute some tests.
if [ "$1" != "" ];then
	ip=$1;
	valid_ip $ip
else
    if [[ "$(basename $0 .sh)" == 'validipv4' ]]; then
        ips='
            4.2.2.2
            a.b.c.d
            192.168.1.1
            0.0.0.0
            255.255.255.255
            255.255.255.256
            192.168.0.1
            192.168.0
            1234.123.123.123
            '
        for ip in $ips
        do
   	 	valid_ip $ip
        done
    	exit;
fi
fi
