#!/bin/bash

#FILE
#	/usr/sbin/UpdateZabbixSetProxy.sh
#PURPOSE
#	Very simple script to update a given zabbix host's proxy.
#AUTHOR
#	(C) 2011,2013 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

if [ "$2" == "" ];then
	echo "usage: $0 <cHost> <valid zabbix proxy_hostid number>";
	exit 0;
fi

cHost="$1";
uProxyHostID="$2";

#set for your zabbix server and port
cZabbixServer="zabbix.isp.net";

#Set for your user and password
#login
cat << EOF > /tmp/login.json
{
	"jsonrpc":"2.0",
	"method":"user.login",
	"params":{
			"user":"apiuser",
			"password":"crapola"
	},
	"id":1
}
EOF
#you should not have to change anything under this line

cAuth=`/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/login.json --output-document=-\
	 --header='Content-Type: application/json-rpc'\
	 https://$cZabbixServer/zabbix/api_jsonrpc.php | cut -f 3 -d : | cut -f 2 -d \"`;

#debug only
#echo "$cAuth";
#exit;

if [ $? != 0 ];then
	echo "wget error 0";
	exit 1;
fi
if [ "$cAuth" == "" ] || [ "$cAuth" == "code" ];then
	echo "Could not login";
	exit 1;
fi

#first get host id
cat << EOF > /tmp/gethost.json
{
	"jsonrpc":"2.0",
	"method":"host.get",
	"params":{
			"output":"shorten",
			"sortfield":"name",
			"filter":{
				"host":["$cHost"]
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
if [ "$uHostID" == "" ] || [ "$uHostID" == "message" ] || [ "$uHostID" == "1}" ];then
	echo "Could not get host id";
	exit 1;
fi

#then update the host
echo "uInterfaceID:$uInterfaceID";
echo "uHostID:$uHostID";
cat << EOF > /tmp/hostupdateport.json
{
	"jsonrpc":"2.0",
	"method":"host.update",
	"params":[{"hostid":$uHostID,
        	"proxy_hostid":"$uProxyHostID"
        }],
	"id":1,
	"auth":"$cAuth"
}
EOF
/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/hostupdateport.json --output-document=-\
	 --header='Content-Type: application/json-rpc'\
	 https://$cZabbixServer/zabbix/api_jsonrpc.php > /dev/null 2>&1;
if [ $? != 0 ];then
	echo "wget error 2";
	exit 1;
fi

exit 0;
