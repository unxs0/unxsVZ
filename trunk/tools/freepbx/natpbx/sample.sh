#!/bin/bash
#sample.sh to ilustrate use

#example scenario:
#	you setup 16 PBX containers
#	after they are all created you run 
#	this script.


if [ "$1" == "" ];then
	echo "usage: $0 <cSourceIPv4>";
	exit;
fi

function valid_ip()
{
    local  ip=$1
    local  stat=1

    if [[ $ip =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
        OIFS=$IFS
        IFS='.'
        ip=($ip)
        IFS=$OIFS
        [[ ${ip[0]} -le 255 && ${ip[1]} -le 255 \
            && ${ip[2]} -le 255 && ${ip[3]} -le 255 ]]
        stat=$?
    fi
    return $stat
}

if ! valid_ip $1;then
	echo "Error: cSourceIPv4 $1 is not a valid dotted quad";
	exit 1;
fi


#iptables
#top part
cat  > /etc/sysconfig/iptables.test << EOF
# Generated by iptables-save v1.3.5 on Tue Apr 17 16:45:03 2012
*mangle
:PREROUTING ACCEPT [1811653:374879873]
:INPUT ACCEPT [63233:19133407]
:FORWARD ACCEPT [1748420:355746466]
:OUTPUT ACCEPT [66372:91163993]
:POSTROUTING ACCEPT [1814792:446910459]
COMMIT
# Completed on Tue Apr 17 16:45:03 2012
# Generated by iptables-save v1.3.5 on Tue Apr 17 16:45:03 2012
*nat
:PREROUTING ACCEPT [5817:543731]
:POSTROUTING ACCEPT [7538:667031]
:OUTPUT ACCEPT [1830:129241]
#
#
EOF
#middle part
/usr/sbin/unxsNATPBX CreateIptablesData $1 >> /etc/sysconfig/iptables.test ;
#end part
cat  >> /etc/sysconfig/iptables.test << EOF
#
#for all pbx
-A POSTROUTING -s 172.16.0.0/255.255.255.0 -j SNAT --to-source $1
COMMIT
# Completed on Tue Apr 17 16:45:03 2012
# Generated by iptables-save v1.3.5 on Tue Apr 17 16:45:03 2012
*filter
:INPUT ACCEPT [63233:19133407]
:FORWARD ACCEPT [1748420:355746466]
:OUTPUT ACCEPT [66372:91163993]
COMMIT
# Completed on Tue Apr 17 16:45:03 2012
EOF

#iptables-restore < /etc/sysconfig/iptables;

#top part
cat  > /etc/squid/squid.conf.test << EOF
https_port 443 cert=/etc/squid/unixservice.crt key=/etc/squid/callingcloud.key defaultsite=unixservice.com vhost
forwarded_for on
EOF
#middle part
/usr/sbin/unxsNATPBX CreateSquidData $1 >> /etc/squid/squid.conf.test;
#end part
cat  >> /etc/squid/squid.conf.test << EOF
acl all src 0.0.0.0/0.0.0.0
acl manager proto cache_object
acl localhost src 127.0.0.1/255.255.255.255
acl to_localhost dst 127.0.0.0/8
acl SSL_ports port 443
acl Safe_ports port 443
acl CONNECT method CONNECT

http_access allow our_sites

http_access allow manager all
http_access allow manager
http_access deny !Safe_ports
http_access deny CONNECT !SSL_ports
http_access deny all

access_log /var/log/squid/access.log
cache_mgr support@voicecarrier.com
EOF

#service squid restart;

#per container rtp.conf file in one command
#does not save previous version or data!
#is also restarts all natpbx containers asterisk service
/usr/sbin/unxsNATPBX CreateRTPData;

