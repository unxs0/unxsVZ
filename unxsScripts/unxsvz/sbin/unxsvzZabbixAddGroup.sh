#!/bin/bash

#FILE
#	/usr/sbin/unxsvzZabbixAddGroup.sh
#PURPOSE
#	Very simple script to add a container group to Zabbix.
#AUTHOR
#	(C) 2011-2016 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE
#NOTES
#	Using reverse exit value logic. We return the group ID

if [ "$1" == "" ];then
	echo "usage: $0 <cGroup>";
	exit 0;
fi
if [ -f "/etc/unxsvz/zabbix.local.sh" ];then
	source /etc/unxsvz/zabbix.local.sh;
else
	echo "no /etc/unxsvz/zabbix.local.sh";
	exit 0;
fi

if [ "$cZabbixPassword" == "" ];then
	echo "no cZabbixPassword";
	exit 0;
fi
if [ "$cZabbixServer" == "" ];then
	echo "no cZabbixServer";
	exit 0;
fi


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
	exit 0;
fi
if [ "$cAuth" == "" ] || [ "$cAuth" == "code" ];then
	echo "Could not login";
	exit 0;
fi
#echo $cAuth;

cat << EOF > /tmp/hostgroup.json
{
	"jsonrpc": "2.0",
	"method": "hostgroup.get",
	"params": {
        "output": "extend",
	"filter": {
	"name": [
			"$1"
		]
	  }
	},
	"id":1,
	"auth":"$cAuth"
}
EOF

uHostGroupID=`/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/hostgroup.json --output-document=-\
	 --header='Content-Type: application/json-rpc'\
	 https://$cZabbixServer/zabbix/api_jsonrpc.php | cut -f 4 -d : | cut -f 2 -d \"`;
if [ $? != 0 ];then
	echo "wget error 1";
	exit 1;
fi
#debug only
#echo $uHostGroupID;
#exit;
if [ "$uHostGroupID" == "" ] || [ "$uHostGroupID" == "message" ] || [ "$uHostGroupID" == "1}" ];then

	cat << EOF > /tmp/hostgroup.json
{
	"jsonrpc": "2.0",
	"method": "hostgroup.create",
	"params": {
		"name": "$1"
	},
	"id":1,
	"auth":"$cAuth"
}
EOF

	uHostGroupID=`/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/hostgroup.json --output-document=-\
		 --header='Content-Type: application/json-rpc'\
		 https://$cZabbixServer/zabbix/api_jsonrpc.php | cut -f 4 -d : | cut -f 2 -d \"`;
	if [ $? != 0 ];then
		echo "wget error 1";
		exit 1;
	fi
	#debug only
	#echo $uHostGroupID;
	#exit;
	if [ "$uHostGroupID" == "" ] || [ "$uHostGroupID" == "message" ] || [ "$uHostGroupID" == "1}" ];then
		echo "Could not get host group id for $1";
		rm -f /tmp/hostgroup.json;
		if [ $? != 0 ];then
			echo "/tmp/hostgroup.json";
		fi
		rm -f /tmp/login.json;
			if [ $? != 0 ];then
			echo "/tmp/login.json";
		fi
		exit 0;
	fi
fi

rm -f /tmp/hostgroup.json;
if [ $? != 0 ];then
	echo "/tmp/hostgroup.json";
	exit 0;
fi
rm -f /tmp/login.json;
if [ $? != 0 ];then
	echo "/tmp/login.json";
	exit 0;
fi

exit $uHostGroupID;
