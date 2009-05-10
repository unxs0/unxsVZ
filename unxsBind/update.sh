#!/bin/bash
#FILE
#	update.sh
#PURPOSE
#	Provide the upgrading mechanisms for switching
#	from one iDNS version to the latest.
#AUTHOR
#	(C) 2008 Gary Wallis and Hugo Urquiza for Unixservice
#

echo "New tHit schema";
echo "Schema update easy:";
echo "Login via mysql to idns db and drop table tHit, then click on the tHit tab via the web backend";
echo "Hard but saves existing data:";
echo "Login via mysql to idns db and use alter table to add the new 6 fields in this exact order:";

echo "mysql> alter table tHit modify column uHitCount BIGINT UNSIGNED NOT NULL DEFAULT 0;"

echo "mysql> alter table tHit add column uSuccess BIGINT UNSIGNED NOT NULL DEFAULT 0;"

echo "mysql> alter table tHit add column uReferral BIGINT UNSIGNED NOT NULL DEFAULT 0;"

echo "mysql> alter table tHit add column uNxrrset BIGINT UNSIGNED NOT NULL DEFAULT 0;"

echo "mysql> alter table tHit add column uNxdomain BIGINT UNSIGNED NOT NULL DEFAULT 0;"

echo "mysql> alter table tHit add column uRecursion BIGINT UNSIGNED NOT NULL DEFAULT 0;"

echo "mysql> alter table tHit add column uFailure BIGINT UNSIGNED NOT NULL DEFAULT 0;"

echo "mysql> alter table tHit add column cHost VARCHAR(255) NOT NULL DEFAULT '';"

echo "mysql> alter table tHit add index (cHost);"

echo "mysql> alter table tHit drop index cZone;"

echo "mysql> alter table tHit add index (cZone);"
