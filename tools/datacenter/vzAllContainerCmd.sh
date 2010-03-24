#!/bin/bash
#FILE
#	vzAllContainerCmd.sh
#PURPOSE
#	Using vzlist run a command after vzctl enter into each one.
#AUTHOR
#	(C) 2010 Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies. See LICENSE file.
#
#NOTES
#
#TODO
#	

#Set these constants, make sure any tables they refer to have the correct data.
cVEIDStatus="running";
#      CTID      NPROC STATUS  IP_ADDR         HOSTNAME                        
#       181         48 running 174.34.136.100 ns1.singtone.com  
/usr/sbin/vzlist | \
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
	echo cIP=$cIP;
	echo cOSTemplate=$cOSTemplate;
	echo cConfig=$cConfig;

	/usr/sbin/vzctl exec $uContainer ls; 

done

exit 0;
