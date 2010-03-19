#!/bin/bash
#FILE
#	vzImport.sh
#PURPOSE
#	Using vzlist -a output populate the tContainer table
#AUTHOR
#	(C) 2010 Hugo Urquiza for Unixservice, LLC.
#
#NOTES
#	This is a very basic script, written to perform
#	a migration task. Therefore initially its command
#	line options and arguments will be replaced by 
#	code variables.
#	

uStartVEID=100;
cVEIDStatus="stopped";
uDatacenter=1;	
uNode=1;
uNameserver=1;
uSkip=1;
#      CTID      NPROC STATUS  IP_ADDR         HOSTNAME                        
#       181         48 running 174.121.136.100 ns1.ringcarrier.com  
vzlist -a  | \
while read cvzlist
do
#	if [ "$uSkip"=="1" ]; then
#		uSkip=0;
#		continue;
#	fi

	uContainer=`echo $cvzlist | awk -F' ' '{print $1}'`;
	cStatus=`echo $cvzlist | awk -F' ' '{print $3}'`;
	cIP=`echo $cvzlist | awk -F' ' '{print $4}'`;
	cHostname=`echo $cvzlist | awk -F' ' '{print $5}'`;
	cLabel==`echo $cHostname | awk -F. '{print $1}'`;
	cOSTemplate=`cat /etc/vz/conf/$uContainer.conf | grep OSTEMPLATE | awk -F= '{print $2}' | awk -F'"' '{print $2}'`;
	cConfig=`cat /etc/vz/conf/$uContainer.conf | grep ORIGIN_SAMPLE | awk -F= '{print $2}' | awk -F'"' '{print $2}'`;
	uStatus=31;
	
cQuery="INSERT INTO tContainer SET uContainer=$uContainer,cLabel='$cLabel',uIPv4=(SELECT uIP FROM tIP WHERE cLabel='$cIP'),uOSTemplate=(SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='$cOSTemplate'),uConfig=(SELECT uConfig FROM tConfig WHERE cLabel='$cConfig',uNameserver=$uNameserver,uSearchdomain=(SELECT uSearchdomain FROM tSearchdomain WHERE cLabel='$cSearchdomain',uDatacenter=$uDatacenter,uNode=$uNode,uStatus=$uStatus,uOwner=$uOwner,uCreatedBy=$uCreatedBy,uCreatedDate=UNIX_TIMESTAMP(NOW())";
	echo $cQuery;
done

