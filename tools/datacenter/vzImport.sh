#!/bin/bash
#FILE
#	vzImport.sh
#PURPOSE
#	Using vzlist -a output populate the tContainer table.
#AUTHOR
#	(C) 2010 Hugo Urquiza and Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies. See LICENSE file.
#
#NOTES
#	This is a very basic script, written to perform
#	a specific migration task.
#	The migration is from another unxsVZ system to a new one.
#	The first step was faking the new node on the source system
#	and running clone wizard after setting up the ssh stuff.
#	

#Set these constants, make sure any tables they refer to have the correct data.
cVEIDStatus="stopped";
uStatus=31;#must match above via tStatus;
uDatacenter=1;	
uNode=1;
uNameserver=1;
uSearchdomain=1;
uOwner=1;
uCreatedBy=1;
#      CTID      NPROC STATUS  IP_ADDR         HOSTNAME                        
#       181         48 running 174.34.136.100 ns1.singtone.com  
vzlist -a  | \
while read cvzlist
do
	cVZStatus=`echo $cvzlist | awk -F' ' '{print $3}'`;
	if [ "$cVZStatus" != "$cVEIDStatus" ]; then
		continue;
	fi
	uContainer=`echo $cvzlist | awk -F' ' '{print $1}'`;
	cIP=`echo $cvzlist | awk -F' ' '{print $4}'`;
	cHostname=`echo $cvzlist | awk -F' ' '{print $5}'`;
	cLabel=`echo $cHostname | awk -F. '{print $1}'`;
	cOSTemplate=`cat /etc/vz/conf/$uContainer.conf | grep OSTEMPLATE | awk -F= '{print $2}' | awk -F'"' '{print $2}'`;
	cConfig=`cat /etc/vz/conf/$uContainer.conf | grep ORIGIN_SAMPLE | awk -F= '{print $2}' | awk -F'"' '{print $2}'`;


	echo uContainer=$uContainer;
	echo cLabel=$cLabel;
	echo cHostname=$cHostname;
	#uVeth=0; default is 0 so we can even omit this from insert query.
	#uIPv4= from select Must preload tIP with the clone range from source unxsVZ db.
	echo cIP=$cIP;
	#uOSTemplate from select. Must preload tOSTemplate from mysqldump of source unxsVZ db.
	echo cOSTemplate=$cOSTemplate;
	#uConfig from select. Must preload tConfig from mysqldump of source unxsVZ db.
	echo cConfig=$cConfig;
	#uNameserver fixed above. tNameserver must have correct info.
	echo uNameserver=$uNameserver;
	#uSearchdomain. Same as above.
	echo uSearchdomain=$uSearchdomain;
	#uDatacenter. Same as above.
	echo uDatacenter=$uDatacenter;
	#uNode. Same as above.
	echo uNode=$uNode;
	echo uStatus=$uStatus;
	echo uOwner=$uOwner;
	echo uCreatedBy=$uCreatedBy;
	echo "";
	
	#cQuery="INSERT INTO tContainer SET uContainer=$uContainer,cLabel='$cLabel',uIPv4=(SELECT uIP FROM tIP WHERE cLabel='$cIP' AND uAvailable=1),uOSTemplate=(SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='$cOSTemplate'),uConfig=(SELECT uConfig FROM tConfig WHERE cLabel='$cConfig',uNameserver=$uNameserver,uSearchdomain=$uSearchdomain,uDatacenter=$uDatacenter,uNode=$uNode,uStatus=$uStatus,uOwner=$uOwner,uCreatedBy=$uCreatedBy,uCreatedDate=UNIX_TIMESTAMP(NOW())";
	#echo $cQuery;
done

