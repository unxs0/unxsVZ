#!/bin/bash
#FILE
#	importCSV.sh
#PURPOSE
#	Read a CSV file and import user data after parsing it
#	via the riedelReg.cgi web interface.
#AUTHOR
#	(C) 2009 Hugo Urquiza for Unixservice.
#USAGE
#	Place the CSV data in a file named csvdata.txt
#	and run this script.
#

while read line
do
	cFirstname=`echo $line | cut -d ";" -f 1`;
	cLastname=`echo $line | cut -d ";" -f 2`;
	cFunction=`echo $line | cut -d ";" -f 4`;
	Company=`echo $line | cut -d ";" -f 5`;
	cEmail=`echo $line | cut -d ";" -f 6`;
	cMAC=`echo $line | cut -d ";" -f 7`;

	#firstname;lastname;team_cap;function;company;email;mac_address;notes;persontype

	uProduct=0;

	echo $cFunction | grep -i "Team Captain" > /dev/null;
	if [ "$?" = "0" ]; then
		uProduct=31;
	fi

	echo $cFunction | grep -i "Event Management" > /dev/null;
	if [ "$?" = "0" ]; then
		uProduct=21;
	fi

	echo $cFunction | grep -i "Media" > /dev/null;
	if [ "$?" = "0" ]; then
		uProduct=41;
	fi
	
	echo $cFunction | grep -i "Mediastation" > /dev/null;
	if [ "$?" = "0" ]; then
		uProduct=51;
	fi

	#Default: staff
	if [ $uProduct = 0 ]; then 
		uProduct=1;
	fi
	#echo "cFirstname=$cFirstname cLastname=$cLastname cEmail=$cEmail cMAC=$cMAC cFunction=$cFunction Company=$Company (uProduct=$uProduct)";

	cPost="cLastName=$cLastname&cFirstName=$cFirstname&cCompany=$Company&uProduct=$uProduct&cAddr1=Edit Me&cCity=Edit Me&cZip=Edit Me&cCountry=Edit Me&cTelephone=Edit Me&cEmail=$cEmail&cMAC=$cMAC&uVPN=0&cHostName=&gcFunction=Confirm+Register&gcPage=Registration";
	wget -O /dev/null -q --post-data="$cPost" http://192.168.0.8/cgi-bin/riedelReg.cgi;

done < csvdata.txt 


