#!/bin/bash
#
#FILE
#	collector.sh
#	$Id: collector.sh 604 2007-06-04 21:58:29Z ggw $
#PURPOSE
#	crontab based script to send daily iDNS cluster node query hit data
#	to master db for it's later aggregation on that server in to tHit.
#iDNS Trac USE CASE
#	https://unixservice.com:9370/projects/iDNS/wiki/UseCase1-3-2
#AUTHOR/LEGAL
#	Gary Wallis (C) 2007 for Unixservice.
#

#Setup vars
cRemoteDBIP="212.71.240.11";
cDBNAME="idns";
cDBLOGIN="idns";
cDBPASSWD="wsxedc";
cThisNodeID="lon7";
cDate=`/bin/date  +"%Y%m%d"`;
cTableName="tHit_"$cDate"_"$cThisNodeID;
cWorkingDir="/tmp";

echo "collector.sh started for $cTableName...";

cd $cWorkingDir;
if [ $? -gt 0 ]; then
	echo "Could not change to $cWorkingDir. Aborting";
	exit 1;
fi

nice /usr/bin/mysql -u $cDBLOGIN -p$cDBPASSWD $cDBNAME <<EOF
DROP TABLE IF EXISTS $cTableName;
CREATE TABLE $cTableName LIKE tHit;
INSERT INTO $cTableName (uHit,cZone,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate,uHitCount) SELECT uHit,cZone,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate,uHitCount FROM tHit;
EOF
if [ $? -gt 0 ]; then
	echo "local mysqld creation of $cTableName failed. Aborting";
	exit 3;
fi

nice /usr/bin/mysqldump -u $cDBLOGIN -p$cDBPASSWD $cDBNAME $cTableName > $cTableName.mysqldump;
if [ $? -gt 0 ]; then
	echo "mysqldump of $cTableName.mysqldump failed. Aborting";
	exit 4;
fi

nice /usr/bin/mysql -u $cDBLOGIN -p$cDBPASSWD $cDBNAME <<EOF
TRUNCATE TABLE tHit;
EOF
if [ $? -gt 0 ]; then
	echo "Warning local mysqld truncation of tHit failed.";
fi

echo "transferring to master...";

nice /usr/bin/mysql -h $cRemoteDBIP -u $cDBLOGIN -p$cDBPASSWD $cDBNAME < $cTableName.mysqldump;
if [ $? -gt 0 ]; then
	echo "mysql transfer of $cTableName.mysqldump failed. Aborting";
	exit 5;
fi

nice /usr/bin/mysql -u $cDBLOGIN -p$cDBPASSWD $cDBNAME <<EOF
DROP TABLE IF EXISTS $cTableName;
EOF
if [ $? -gt 0 ]; then
	echo "local mysqld removal of $cTableName failed. Aborting";
	exit 3;
fi


rm $cTableName.mysqldump;
if [ $? -gt 0 ]; then
	echo "Warning could not remove $cWorkingDir/$cTableName.mysqldump.";
fi

echo "collector.sh ended no errors";
exit 0;
