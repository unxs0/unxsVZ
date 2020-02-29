#!/bin/bash
#NOTES
#


cMysql="/usr/bin/mysql -uunxsev -pwsxedc unxsev -e ";

while true;do

	cAvgWind=`echo -n $(( 20 + RANDOM % 8 ))`;
	cMinWind="20";
	cMaxWind="27";

	#cDirWind=`echo -n $(( 90 + RANDOM % 16 ))`;
	#degToCompass $cDirWind;
	cDirWind="E";


	$cMysql "UPDATE tConfiguration SET cValue='$cAvgWind',uModDate=UNIX_TIMESTAMP(NOW()) WHERE uConfiguration=6";
	$cMysql "UPDATE tConfiguration SET cValue='$cMinWind',uModDate=UNIX_TIMESTAMP(NOW()) WHERE uConfiguration=7";
	$cMysql "UPDATE tConfiguration SET cValue='$cMaxWind',uModDate=UNIX_TIMESTAMP(NOW()) WHERE uConfiguration=8";
	$cMysql "UPDATE tConfiguration SET cValue='$cDirWind',uModDate=UNIX_TIMESTAMP(NOW()) WHERE uConfiguration=9";

	sleep 10;

done
