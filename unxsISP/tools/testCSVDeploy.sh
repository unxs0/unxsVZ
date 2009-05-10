#!/bin/bash
#FILE
#	importCSV.sh
#PURPOSE
#	Read a CSV file and test if the users there
#	where correctly imported and their products deployed.
#AUTHOR
#	(C) 2009 Hugo Urquiza for Unixservice.
#USAGE
#	Place the CSV data in a file named csvdata.txt
#	and run this script.
#

while read line
do
	cFirstname=`echo $line | cut -d ";" -f 1 | tr "[:upper:]" "[:lower:]" `;
	cLastname=`echo $line | cut -d ";" -f 2 | tr "[:upper:]" "[:lower:]"`; 
	cMAC=`echo $line | cut -d ";" -f 7`;
	cLogin="$cFirstname.$cLastname";

	uCount=`radiususers.sh | grep $cLogin | wc -l`;
	if [ "$uCount" != "3" ]; then
		echo "ERROR: $cLogin not deployed! (uCount=$uCount, must be 3)";
	else
		echo "$cLogin deployed OK";
	fi
	
	uCount=`radiususers.sh | grep -i $cMAC | wc -l`;
	if [ "$uCount" != "6" ]; then
		echo "ERROR: $cMAC not deployed! (uCount=$uCount, must be 6)";
	else
		echo "$cMAC deployed OK";
	fi

	echo;
	echo;

done < csvdata.txt 


