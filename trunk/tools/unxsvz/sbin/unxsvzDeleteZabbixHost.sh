#!/bin/bash

#FILE
#	unxsvzDeleteZabbixHost.sh
#PURPOSE
#	Very simple script to delete a given zabbix host.
#AUTHOR
#	(C) 2011-2014 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

if [ "$1" == "" ];then
	echo "usage: $0 <cHost>";
	exit 0;
fi
if [ -f "/etc/unxsvz/zabbix.local.sh" ];then
	source /etc/unxsvz/zabbix.local.sh;
else
	echo "no /etc/unxsvz/zabbix.local.sh";
	exit 1;
fi

if [ "$cZabbixPassword" == "" ];then
	echo "no cZabbixPassword";
	exit 1;
fi
if [ "$cZabbixServer" == "" ];then
	echo "no cZabbixServer";
	exit 1;
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
	exit 1;
fi
if [ "$cAuth" == "" ] || [ "$cAuth" == "code" ];then
	echo "Could not login";
	exit 1;
fi
#echo $cAuth;

#first get host id
#it seems that unless the host belongs to the discovered hosts groups
#this fails
cat << EOF > /tmp/gethost.json
{
	"jsonrpc":"2.0",
	"method":"host.get",
	"params":{
			"output":"shorten",
			"sortfield":"name",
			"filter":{
				"host":["$1"]
			},
			"limit":8
	},
	"id":1,
	"auth":"$cAuth"
}
EOF

uHostID=`/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/gethost.json --output-document=-\
	 --header='Content-Type: application/json-rpc'\
	 https://$cZabbixServer/zabbix/api_jsonrpc.php | cut -f 4 -d : | cut -f 2 -d \"`;
if [ $? != 0 ];then
	echo "wget error 1";
	exit 1;
fi
#debug only
#echo $uHostID;
#exit;
if [ "$uHostID" == "" ] || [ "$uHostID" == "message" ] || [ "$uHostID" == "1}" ];then
	echo "Could not get host id for $1";
	exit 1;
fi

#then delete the host
echo "uHostID:$uHostID ($1)";
cat << EOF > /tmp/hostdelete.json
{
	"jsonrpc":"2.0",
	"method":"host.delete",
	"params":[{"hostid":$uHostID}],
	"id":1,
	"auth":"$cAuth"
}
EOF

/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/hostdelete.json --output-document=-\
	 --header='Content-Type: application/json-rpc'\
	 https://$cZabbixServer/zabbix/api_jsonrpc.php > /dev/null 2>&1;
if [ $? != 0 ];then
	echo "wget error 2";
	exit 1;
fi

exit 0;
