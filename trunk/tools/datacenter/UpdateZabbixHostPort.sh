#!/bin/bash

#FILE
#	/usr/sbin/UpdateZabbixHostPort.sh
#PURPOSE
#	Very simple script to update a given zabbix host's port.
#AUTHOR
#	(C) 2011,2013 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

if [ "$2" == "" ];then
	echo "usage: $0 <cHost> <uPort>";
	exit 0;
fi

cHost="$1";
uPort="$2";

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
			"password":"foobar"
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

#now get interface id
cat << EOF > /tmp/gethost.json
{
    "jsonrpc": "2.0",
    "method": "hostinterface.get",
    "params": {
        "output": "extend",
        "hostids": "$uHostID"
    },
    "auth": "$cAuth",
    "id": 1
}
EOF
uInterfaceID=`/usr/bin/wget --quiet --no-check-certificate --post-file=/tmp/gethost.json --output-document=-\
	 --header='Content-Type: application/json-rpc'\
	 https://$cZabbixServer/zabbix/api_jsonrpc.php | cut -f 4 -d : | cut -f 2 -d \"`;
if [ $? != 0 ];then
	echo "wget error 1";
	exit 1;
fi
if [ "$uInterfaceID" == "" ] || [ "$uInterfaceID" == "message" ] || [ "$uInterfaceID" == "1}" ];then
	echo "Could not get interface id";
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
        "interfaces": {
	    "interfaceid":"$uInterfaceID",
            "dns": "$cHost",
            "ip": "127.0.0.1",
            "main": 1,
            "port": "$uPort",
            "type": 1,
            "useip": 0
	}
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
