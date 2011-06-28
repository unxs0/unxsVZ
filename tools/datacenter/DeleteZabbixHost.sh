#!/bin/bash

#FILE
#	DeleteZabbixHost.sh
#PURPOSE
#	Very simple script to delete a given zabbix host.
#AUTHOR
#	(C) 2011 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

if [ "$1" == "" ];then
	echo "usage: $0 <cHost>";
	exit;
fi

#Set for your user and password
#login
cat << EOF > /tmp/login.json
{
	"jsonrpc":"2.0",
	"method":"user.login",
	"params":{
			"user":"apiuser",
			"password":"whisky"
	},
	"id":1
}
EOF

cAuth=`/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/login.json --output-document=-\
	 --header='Content-Type: application/json-rpc'\
	 https://rc3.callingcloud.net:9333/zabbix/api_jsonrpc.php | cut -f 3 -d : | cut -f 2 -d \"`;
if [ $? != 0 ];then
	echo "wget error 0";
	exit;
fi
if [ "$cAuth" == "" ] || [ "$cAuth" == "code" ];then
	echo "Could not login";
	exit;
fi
#echo $cAuth;

#first get host id
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
	 https://rc3.callingcloud.net:9333/zabbix/api_jsonrpc.php | cut -f 4 -d : | cut -f 2 -d \"`;
if [ $? != 0 ];then
	echo "wget error 1";
	exit;
fi
if [ "$uHostID" == "" ] || [ "$uHostID" == "message" ];then
	echo "Could not get host id";
	exit;
fi

#then delete the host
echo "$uHostID";
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
	 https://rc3.callingcloud.net:9333/zabbix/api_jsonrpc.php > /dev/null;
if [ $? != 0 ];then
	echo "wget error 2";
	exit;
fi
