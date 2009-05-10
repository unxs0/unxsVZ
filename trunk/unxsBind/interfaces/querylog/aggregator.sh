#!/bin/bash
#
#FILE
#	aggregator.sh
#	$Id: aggregator.sh 610 2007-06-05 15:43:03Z ggw $
#PURPOSE
#	crontab based script to process daily iDNS cluster node query hit data
#	tables that were sent via collector.sh to master mySQL server.
#iDNS Trac USE CASE
#	https://unixservice.com:9370/projects/iDNS/wiki/UseCase1-3-2
#AUTHOR/LEGAL
#	Gary Wallis (C) 2007 for Unixservice.
#NOTES/EXAMPLES
#	Every early AM tHit_200070604_lon2/6/7 type tables are sent.
#	/var/lib/mysql/idns/tHit_20070604_lon2.frm
#
#	For every table we aggregate them via cZone best match to tHit.
#	Aggregate means update uHitCount or insert new cZone based row.
#

#Setup vars
cDBNAME="idns";
cDBLOGIN="idns";
cDBPASSWD="wsxedc";
cPathToTables="/var/lib/mysql/idns/";
cDate=`/bin/date  +"%Y%m%d"`;
cWorkingDir="/tmp";

#optional args
if [ $1 ]; then
	cDate=$1;
fi

cTableName="tHit_"$cDate"_*.frm";

echo "aggregator.sh started for $cTableName...";

cd $cPathToTables;
if [ $? -gt 0 ]; then
	echo "Could not change to $PathToTables. Aborting";
	exit 1;
fi

for cTable in `ls $cTableName`; do

	#node id is fixed at 3 chars here dependencies with collector.sh
	cTable=${cTable:0:18};
	echo "processing $cTable";

	#run per table C aggregator program
	nice /usr/sbin/idns-aggregator $cTable;
	if [ $? -gt 0 ]; then
		echo "/usr/sbin/idns-aggregator $cTable failed. Aborting";
		exit 1;
	fi

	nice /usr/bin/mysql -u $cDBLOGIN -p$cDBPASSWD $cDBNAME <<EOF
DROP TABLE IF EXISTS $cTable;
EOF
	if [ $? -gt 0 ]; then
		echo "local mysqld drop of $cTable failed. Aborting";
		exit 3;
	fi

done

echo "aggregator.sh ended no errors";
exit 0;
