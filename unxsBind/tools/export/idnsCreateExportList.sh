#!/bin/bash

cMysql="/bin/mysql -uidns -pwsxedc idns --batch -N -e ";

cCREATE="CREATE TABLE IF NOT EXISTS tExportItem ( uExportItem INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0 );";


$cMysql "$cCREATE";

while IFS= read -r cLabel;do
	$cMysql "SELECT uExportItem FROM tExportItem WHERE cLabel='$cLabel';" > /dev/null;
	if [ $? != 0 ];then
		echo INSERT $cLabel;
		$cMysql "INSERT INTO tExportItem SET cLabel='$cLabel';";
	fi
	
done < ./delme.list

echo -n "count ";
$cMysql "SELECT COUNT(*) FROM tExportItem;";
