#!/bin/bash

#FILE
#	/usr/sbin/UpdateZabbixHostPort.sh
#PURPOSE
#	Very simple script to update a given zabbix host's port.
#AUTHOR
#	(C) 2011,2012 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

if [ "$2" == "" ];then
	echo "usage: $0 <cHost> <uPort>";
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
			"password":"changeme"
	},
	"id":1
}
EOF

cAuth=`/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/login.json --output-document=-\
	 --header='Content-Type: application/json-rpc'\
	 https://c3.cloud.net:933/zabbix/api_jsonrpc.php | cut -f 3 -d : | cut -f 2 -d \"`;
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
	 https://c3.cloud.net:933/zabbix/api_jsonrpc.php | cut -f 4 -d : | cut -f 2 -d \"`;
if [ $? != 0 ];then
	echo "wget error 1";
	exit 1;
fi
if [ "$uHostID" == "" ] || [ "$uHostID" == "message" ];then
	echo "Could not get host id";
	exit 1;
fi

#then delete the host
echo "$uHostID";
cat << EOF > /tmp/hostupdateport.json
{
	"jsonrpc":"2.0",
	"method":"host.update",
	"params":[{"hostid":$uHostID,"port":$2}],
	"id":1,
	"auth":"$cAuth"
}
EOF

/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/hostupdateport.json --output-document=-\
	 --header='Content-Type: application/json-rpc'\
	 https://c3.cloud.net:933/zabbix/api_jsonrpc.php > /dev/null;
if [ $? != 0 ];then
	echo "wget error 2";
	exit 1;
fi

exit 0;
