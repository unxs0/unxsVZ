#!/bin/bash
#NOTES
#	requires pre-existing hardcoded uConfiguration values
#	this api is for our arduino anenometer network box
#


cMysql="/usr/bin/mysql -uunxsev -pwsxedc unxsev -e ";
#intial setup
if [ "$1" == "SetupDb" ];then
	$cMysql "DELETE FROM tConfiguration WHERE uConfiguration>=6 AND uConfiguration<=9";
	$cMysql "INSERT INTO tConfiguration SET cLabel='cAvgWind',uCreatedDate=UNIX_TIMESTAMP(NOW()),uConfiguration=6,uCreatedBy=1,uModBy=1,uOwner=1";
	$cMysql "INSERT INTO tConfiguration SET cLabel='cMinWind',uCreatedDate=UNIX_TIMESTAMP(NOW()),uConfiguration=7,uCreatedBy=1,uModBy=1,uOwner=1";
	$cMysql "INSERT INTO tConfiguration SET cLabel='cMaxWind',uCreatedDate=UNIX_TIMESTAMP(NOW()),uConfiguration=8,uCreatedBy=1,uModBy=1,uOwner=1";
	$cMysql "INSERT INTO tConfiguration SET cLabel='cDirWind',uCreatedDate=UNIX_TIMESTAMP(NOW()),uConfiguration=9,uCreatedBy=1,uModBy=1,uOwner=1";
	exit 0;
fi


exit 0;

echo "Content-type: text"
echo ""
echo ""
cQuery=$QUERY_STRING;
cAvgWind=`echo $cQuery | cut -f 5 -d '&' | cut -f 2 -d '='`;
cMinWind=`echo $cQuery | cut -f 6 -d '&' | cut -f 2 -d '='`;
cMaxWind=`echo $cQuery | cut -f 7 -d '&' | cut -f 2 -d '='`;
cDirWind=`echo $cQuery | cut -f 8 -d '&' | cut -f 2 -d '='`;

echo cAvgWind:$cAvgWind;
echo cMinWind:$cMinWind;
echo cMaxWind:$cMaxWind;
echo cDirWind:$cDirWind;

$cMysql "UPDATE tConfiguration SET cValue='$cAvgWind',uModDate=UNIX_TIMESTAMP(NOW()) WHERE uConfiguration=6";
$cMysql "UPDATE tConfiguration SET cValue='$cMinWind',uModDate=UNIX_TIMESTAMP(NOW()) WHERE uConfiguration=7";
$cMysql "UPDATE tConfiguration SET cValue='$cMaxWind',uModDate=UNIX_TIMESTAMP(NOW()) WHERE uConfiguration=8";
$cMysql "UPDATE tConfiguration SET cValue='$cDirWind',uModDate=UNIX_TIMESTAMP(NOW()) WHERE uConfiguration=9";
