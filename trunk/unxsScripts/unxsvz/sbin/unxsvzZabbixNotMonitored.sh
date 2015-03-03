#!/bin/bash

#
#FILE
#       /usr/sbin/unxsvzZabbixNotMonitored.sh
#PURPOSE
#       Report any unxsVZ PBX container that is not
#       in Zabbix hosts table.
#NOTES
#	Must be run in Zabbix server. DBs must have SELECT perms for the MySQL 
#	users.
#AUTHOR
#	(C) 2014 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

if [ "$1" != "run" ];then
	echo "usage: $0 run <uVEID>";
	exit 0;
fi

cMySQLConnect="";
if [ -f "/etc/unxsvz/mysql.local.sh" ];then
	source /etc/unxsvz/mysql.local.sh;
else
	echo "no /etc/unxsvz/mysql.local.sh";
	exit 1;
fi
if [ -f "/etc/unxsvz/unxsvz.functions.sh" ];then
	source /etc/unxsvz/unxsvz.functions.sh;
else
	echo "no /etc/unxsvz/unxsvz.functions.sh";
	exit 1;
fi
if [ "$cMySQLConnect" == "" ];then
	fLog "no cMySQLConnect";
	exit 1;
fi

cMySQLConnectLocal="/usr/bin/mysql -uunxsvz -pwsxedc zabbix";
cGroupStatement="tGroup.cLabel='Production PBXs'";

echo "The following active 'Production PBXs' group members are not in Zabbix hosts table, please check!";

for cHostname in `echo "SELECT tContainer.cHostname FROM tContainer,tGroup,tGroupGlue WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uStatus=1 AND tContainer.cHostname NOT LIKE '%-clone%' ORDER BY tContainer.cHostname"|$cMySQLConnect -B -N`;do

        #echo $cHostname;
        cReply=`$cMySQLConnectLocal -B -N -e "SELECT host FROM hosts WHERE host='$cHostname'"`;
        if [ "$cReply" != "$cHostname" ];then
                echo "$cHostname";
        fi

done
