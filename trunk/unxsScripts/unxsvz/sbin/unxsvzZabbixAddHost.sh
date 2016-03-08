#!/bin/bash
#FILE
#	/usr/sbin/unxsvzZabbixAddHost.sh
#PURPOSE
#	Very simple script to add a host given hostname and container group to Zabbix.
#	This version also adds a fixed template via Zabbix template id.
#AUTHOR
#	(C) 2011-2016 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE
#NOTES
#	For use with popen

if [ "$2" == "" ];then
	echo "usage: $0 <cHostname> <uGroupID> [<cIPv4>]";
	exit 1;
fi
if [ -f "/etc/unxsvz/zabbix.local.sh" ];then
	source /etc/unxsvz/zabbix.local.sh;
else
	echo "no /etc/unxsvz/zabbix.local.sh";
	exit 2;
fi

if [ "$cZabbixPassword" == "" ];then
	echo "no cZabbixPassword";
	exit 3;
fi
if [ "$cZabbixServer" == "" ];then
	echo "no cZabbixServer";
	exit 4;
fi


#
#
#Set for your user and password
#login
cat << EOF > /tmp/login.json
{
	"jsonrpc":"2.0",
	"method":"user.login",
	"params":{
			"user":"apiuser",
			"password":"$cZabbixPassword"
	},
	"id":1
}
EOF

cAuth=`/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/login.json --output-document=-\
	 --header='Content-Type: application/json-rpc'\
	 https://$cZabbixServer/zabbix/api_jsonrpc.php | cut -f 3 -d : | cut -f 2 -d \"`;
if [ $? != 0 ];then
	echo "wget error 0";
	exit 5;
fi
if [ "$cAuth" == "" ] || [ "$cAuth" == "code" ];then
	echo "Could not login";
	exit 6;
fi
#echo $cAuth;

#
#
#make sure host group exists
cat << EOF > /tmp/host.json
{
	"jsonrpc": "2.0",
	"method": "hostgroup.get",
	"params": {
        "output": "extend",
	"filter": {
	"hostid": [
			"$2"
		]
	  }
	},
	"id":1,
	"auth":"$cAuth"
}
EOF

uHostGroupID=`/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/host.json --output-document=-\
	 --header='Content-Type: application/json-rpc'\
	 https://$cZabbixServer/zabbix/api_jsonrpc.php | cut -f 4 -d : | cut -f 2 -d \"`;
if [ $? != 0 ];then
	echo "wget error 2";
	exit 7;
fi
#debug only
#echo $uHostGroupID;
#exit;
if [ "$uHostGroupID" == "" ] || [ "$uHostGroupID" == "message" ] || [ "$uHostGroupID" == "1}" ];then
	echo "host group $2 does not exist";
	uHostID="0";
else

	cat << EOF > /tmp/host.json
{
	"jsonrpc": "2.0",
	"method": "host.get",
	"params": {
		"output": ["hostid"],
	"filter": {
		"host": [
				"$1"
			]
		}
	},
	"id":1,
	"auth":"$cAuth"
}
EOF
	uHostID=`/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/host.json --output-document=-\
		 --header='Content-Type: application/json-rpc'\
		 https://$cZabbixServer/zabbix/api_jsonrpc.php | cut -f 4 -d : | cut -f 2 -d \"`;
	if [ $? != 0 ];then
		echo "wget error 3";
		exit 8;
	fi
	if [ "$uHostID" == "" ] || [ "$uHostID" == "message" ] || [ "$uHostID" == "1}" ];then


	cat << EOF > /tmp/host.json
{
	"jsonrpc": "2.0",
	"method": "host.create",
	"params": {
		"host": "$1",
		"interfaces": [
		{
			"type": 1,
			"main": 1,
			"useip": 0,
			"ip": "$3",
			"dns": "$1",
			"port": "10050"
		}
		],
		"groups": [
		{
			"groupid": "$2"
		}
		],
		"templates": [
		{
			"templateid": "10104"
		}
		]
	},
	"id":1,
	"auth":"$cAuth"
}
EOF

	uHostID=`/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/host.json --output-document=-\
		 --header='Content-Type: application/json-rpc'\
		 https://$cZabbixServer/zabbix/api_jsonrpc.php | cut -f 4 -d : | cut -f 2 -d \"`;
	if [ $? != 0 ];then
		echo "wget error 4";
		exit 9;
	fi
	#debug only
	#echo $uHostID;
	#exit;
	if [ "$uHostID" == "" ] || [ "$uHostID" == "message" ] || [ "$uHostID" == "1}" ];then
		echo "Could not get host id for $1";
		uHostID="0";
	fi

	fi
fi

rm -f /tmp/host.json;
if [ $? != 0 ];then
	echo "/tmp/host.json";
	exit 10;
fi
rm -f /tmp/login.json;
if [ $? != 0 ];then
	echo "/tmp/login.json";
	exit 11;
fi

echo $uHostID;
exit 0;
