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
#TODO
#	We need to get the actual MySQL query results to determine
#	if something wrong or unexpected happened.
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
/usr/sbin/vzlist -a  | \
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

	cQuery="SELECT uIP FROM tIP WHERE cLabel='$cIP' AND uAvailable=1";
	uIPv4=`echo $cQuery | /usr/bin/mysql -pwsxedc -u unxsvz unxsvz | tail -n 1 | awk -F' ' '{print $1}'`;
	if [ $? != 0 ];then
		echo "$cQuery failed";
		exit 1;
	fi
	echo uIPv4=$uIPv4;

	cQuery="SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='$cOSTemplate'";
	uOSTemplate=`echo $cQuery | /usr/bin/mysql -pwsxedc -u unxsvz unxsvz | tail -n 1 | awk -F' ' '{print $1}'`;
	if [ $? != 0 ];then
		echo "$cQuery failed";
		exit 1;
	fi
	echo uOSTemplate=$uOSTemplate;

	cQuery="SELECT uConfig FROM tConfig WHERE cLabel='$cConfig'";
	uConfig=`echo $cQuery | /usr/bin/mysql -pwsxedc -u unxsvz unxsvz | tail -n 1 | awk -F' ' '{print $1}'`;
	if [ $? != 0 ];then
		echo "$cQuery failed";
		exit 1;
	fi
	echo uConfig=$uConfig;

	cQuery="SELECT uContainer FROM tContainer WHERE cLabel='$cLabel' AND uDatacenter=$uDatacenter AND uNode=$uNode";
	uCheckContainer=`echo $cQuery | /usr/bin/mysql -pwsxedc -u unxsvz unxsvz | tail -n 1 | awk -F' ' '{print $1}'`;
	if [ $? != 0 ];then
		echo "$cQuery failed";
		exit 1;
	fi
	echo uCheckContainer=$uCheckContainer;

	if [ "$uCheckContainer" != "" ];then
		echo "Skipping container cLabel=$cLabel cHostname=$cHostname already in tContainer";
		echo "";
		continue;
	fi

	#this is a nested read see do above
	read -e -p "Commit $cHostname [y/n/s/q]?: " cReply < /dev/tty;
	if [ "$cReply" == "n" ];then
		echo "Aborting commit and exiting $0";
		exit 0;
	fi	
	if [ "$cReply" == "s" ];then
		echo "Skipping $cHostname";
		echo "";
		continue;
	fi	
	if [ "$cReply" != "y" ];then
		echo "Aborting commit and exiting $0";
		exit 0;
	fi	

	echo "Commiting to tContainer";

	cQuery="INSERT INTO tContainer SET uContainer=$uContainer,cLabel='$cLabel',cHostname='$cHostname',uIPv4=$uIPv4,uOSTemplate=$uOSTemplate,uConfig=$uConfig,uNameserver=$uNameserver,uSearchdomain=$uSearchdomain,uDatacenter=$uDatacenter,uNode=$uNode,uStatus=$uStatus,uOwner=$uOwner,uCreatedBy=$uCreatedBy,uCreatedDate=UNIX_TIMESTAMP(NOW())";
	cResult=`echo $cQuery | /usr/bin/mysql -pwsxedc -u unxsvz unxsvz | tail -n 1`;
	if [ $? != 0 ];then
		echo "$cQuery failed";
		continue;
	fi
	echo $cResult;

	cQuery="UPDATE tIP SET uAvailable=0 WHERE uIP=$uIPv4";
	cResult=`echo $cQuery | /usr/bin/mysql -pwsxedc -u unxsvz unxsvz | tail -n 1`;
	if [ $? != 0 ];then
		echo "$cQuery failed";
		exit 1;
	fi
	echo $cResult;
	echo "Commit attempted";
	echo "";
done

exit 0;
