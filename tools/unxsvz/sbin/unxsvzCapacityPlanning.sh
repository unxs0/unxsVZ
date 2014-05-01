#!/bin/bash

#FILE
#	unxsvzCapacityPlanning.sh
#PURPOSE
#	Very simple script to create csv file
#	with capacity planning helpful data.
#AUTHOR
#	(C) 2011-2014 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

if [ "$1" == "" ];then
	echo "usage: $0 <generate>";
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
if [ -f "/etc/unxsvz/unxsvz.mail.sh" ];then
	source /etc/unxsvz/unxsvz.mail.sh;
else
	echo "no /etc/unxsvz/unxsvz.mail.sh";
	exit 1;
fi
if [ "$cCapacityPlanningEmailTo" == "" ];then
	fLog "no cCapacityPlanningEmailTo";
	exit 1;
fi
if [ "$cMySQLConnect" == "" ];then
	fLog "no cMySQLConnect";
	exit 1;
fi

fLog "start";

echo "Available Slots,Datacenter" > /tmp/unxsvzCapacityPlanning.csv;
echo "SELECT COUNT(tContainer.uContainer),tDatacenter.cLabel FROM tContainer,tDatacenter,tNode WHERE tDatacenter.uStatus=1 AND tNode.uStatus=1 AND tContainer.uStatus=1 AND tContainer.uSource=0 AND tContainer.uNode=tNode.uNode AND tNode.uDatacenter=tDatacenter.uDatacenter GROUP BY tDatacenter.uDatacenter" | $cMySQLConnect -N | sed 's/\t/,/g' >> /tmp/unxsvzCapacityPlanning.csv;


echo ""  >> /tmp/unxsvzCapacityPlanning.csv;
echo "Containers created,Last 7 days" >> /tmp/unxsvzCapacityPlanning.csv;
echo "SELECT COUNT(uContainer),\"Last 7 days\" FROM tContainer WHERE uStatus=1 AND uSource=0 AND uCreatedDate > (UNIX_TIMESTAMP(NOW())-(86400*7))" | $cMySQLConnect -N | sed 's/\t/,/g' >> /tmp/unxsvzCapacityPlanning.csv;
echo ""  >> /tmp/unxsvzCapacityPlanning.csv;
echo "Containers created,Last 30 days" >> /tmp/unxsvzCapacityPlanning.csv;
echo "SELECT COUNT(uContainer),\"Last 30 days\" FROM tContainer WHERE uStatus=1 AND uSource=0 AND uCreatedDate > (UNIX_TIMESTAMP(NOW())-(86400*30))" | $cMySQLConnect -N | sed 's/\t/,/g' >> /tmp/unxsvzCapacityPlanning.csv;
echo ""  >> /tmp/unxsvzCapacityPlanning.csv;
echo "Containers created,Last 180 days" >> /tmp/unxsvzCapacityPlanning.csv;
echo "SELECT COUNT(uContainer),\"Last 180 days\" FROM tContainer WHERE uStatus=1 AND uSource=0 AND uCreatedDate > (UNIX_TIMESTAMP(NOW())-(86400*180))" | $cMySQLConnect -N | sed 's/\t/,/g' >> /tmp/unxsvzCapacityPlanning.csv;

echo ""  >> /tmp/unxsvzCapacityPlanning.csv;
echo "Remote backups,Datacenter" >> /tmp/unxsvzCapacityPlanning.csv;
echo "SELECT COUNT(tContainer.uContainer),tDatacenter.cLabel FROM tContainer,tDatacenter,tNode WHERE tDatacenter.uStatus=1 AND tNode.uStatus=1 AND tContainer.uStatus=1 AND tContainer.cLabel LIKE '%%-backup' AND tContainer.uSource!=0 AND tContainer.uNode=tNode.uNode AND tNode.uDatacenter=tDatacenter.uDatacenter GROUP BY tDatacenter.uDatacenter" | $cMySQLConnect -N | sed 's/\t/,/g' >> /tmp/unxsvzCapacityPlanning.csv;

echo ""  >> /tmp/unxsvzCapacityPlanning.csv;
echo "Local backups,Datacenter" >> /tmp/unxsvzCapacityPlanning.csv;
echo "SELECT COUNT(tContainer.uContainer),tDatacenter.cLabel FROM tContainer,tDatacenter,tNode WHERE tDatacenter.uStatus=1 AND tNode.uStatus=1 AND tContainer.uStatus=31 AND tContainer.cLabel LIKE '%%-clone%%' AND tContainer.uSource!=0 AND tContainer.uNode=tNode.uNode AND tNode.uDatacenter=tDatacenter.uDatacenter GROUP BY tDatacenter.uDatacenter" | $cMySQLConnect -N | sed 's/\t/,/g' >> /tmp/unxsvzCapacityPlanning.csv;

echo ""  >> /tmp/unxsvzCapacityPlanning.csv;
echo "Available IP numbers,Datacenter" >> /tmp/unxsvzCapacityPlanning.csv;
echo "SELECT COUNT(tIP.uIP),tDatacenter.cLabel FROM tIP,tDatacenter WHERE tDatacenter.uStatus=1 AND tIP.uAvailable=1 AND tIP.uDatacenter=tDatacenter.uDatacenter AND tIP.cLabel NOT LIKE '10.%%.%%.%%' AND tIP.cLabel NOT LIKE '172.16.%%.%%' AND tIP.cLabel NOT LIKE '172.17.%%.%%' AND tIP.cLabel NOT LIKE '172.18.%%.%%' AND tIP.cLabel NOT LIKE '192.168.%%.%%' AND tDatacenter.uStatus=1 GROUP BY tDatacenter.uDatacenter" | $cMySQLConnect -N | sed 's/\t/,/g' >> /tmp/unxsvzCapacityPlanning.csv;

if [ "$cCapacityPlanningEmailBcc" != "" ];then
	echo "Please find the capacity planning csv report version 0.0 attached" | /usr/bin/mutt -a "/tmp/unxsvzCapacityPlanning.csv" -s "Capacity Planning Report" -b $cCapacityPlanningEmailBcc -- $cCapacityPlanningEmailTo
else
	echo "Please find the capacity planning csv report version 0.0 attached" | /usr/bin/mutt -a "/tmp/unxsvzCapacityPlanning.csv" -s "Capacity Planning Report" -- $cCapacityPlanningEmailTo
fi
if [ $? != 0 ];then
	fLog "mutt email failed";
fi

fLog "end";

exit;
