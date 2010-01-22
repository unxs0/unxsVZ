#/bin/bash
#
#FILE
#	ShutdownOnUPSEvent.sh
#PURPOSE
#	Using snmpget connect to UPS snmpd (card) and if battery is below
#	uBatteryCapacity initiate shutdown sequence.
#AUTHOR
#	Gary Wallis and Hugo Urquiza for Unixservice, LLC. (C) 2009-2010
#NOTES
#	This is experimental alpha proof of concept code. Use at your
#	own peril. Patch and/or supply your own version and contribute it 
#	to this project.

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

#Set these for your SNMP enabled UPS
cHost="ups0.yourisp.com";
cSnmpGet="/usr/bin/snmpget -v 1 -c publiccommstr $cHost";

cBatCapMIB="mib-2.33.1.2.4.0";
cACInMIB="mib-2.33.1.3.3.1.3.1";

#fast abort if host does not respond
/bin/ping -c 1 $cHost > /dev/null 2>&1;
if [ "$?" != "0" ]; then
	fLog "$cHost seems down";
	exit 1;
fi

#fast abort if snmpget does not work.
$cSnmpGet $cBatCapMIB > /dev/null 2>&1;
if [ $? != 0 ];then
	fLog "$cBatCapMIB error";
	exit 1;
fi

uBatteryCapacity=`$cSnmpGet $cBatCapMIB | cut -f 4 -d : | cut -f 2 -d " "`;
if [ $? != 0 ];then
	fLog "$cBatCapMIB uBatteryCapacity error";
	exit 1;
fi

if [ -z "$uBatteryCapacity" ];then
	fLog "$cBatCapMIB uBatteryCapacity null error";
	exit 1;
fi

uACLineVoltage=`$cSnmpGet $cACInMIB | cut -f 4 -d : | cut -f 2 -d " "`;
if [ $? != 0 ];then
	fLog "$cACInMIB error";
	exit 1;
fi

if [ -z "$uACLineVoltage" ];then
	fLog "$cACInMIB null error";
	exit 1;
fi

#set this section's hardcoded values to suit your needs
uACLineVoltage=$(( $uACLineVoltage / 10 ));
if (( $uACLineVoltage > 180 ));then
	uBattCapThreshold="5";
	uSleep="60";
else
	uBattCapThreshold="10";
	uSleep="30";
fi

#debug only
#echo "uACLineVoltage is $uACLineVoltage";

if (( $uBatteryCapacity > $uBattCapThreshold ));then
	#debug only
	#echo "uBatteryCapacity $uBatteryCapacity is larger than $uBattCapThreshold";
	exit 0;
else
	#initiate shutdown seq
	#debug only
	fLog "uBatteryCapacity $uBatteryCapacity is less than $uBattCapThreshold";

	#Wait to see if it was just a brief incident
	#debug only
	fLog "sleeping for $uSleep seconds";
	sleep $uSleep;

	uBatteryCapacity2=`$cSnmpGet $cBatCapMIB | cut -f 4 -d : | cut -f 2 -d " "`;
	if [ -z "$uBatteryCapacity2" ];then
		fLog "$cBatCapMIB uBatteryCapacity2 null error";
		fLog "shutdown aborted";
		exit 1;
	fi

	if (( $uBatteryCapacity2 >= $uBatteryCapacity ));then
		#debug only
		fLog "uBatteryCapacity2 $uBatteryCapacity2 is larger or equal to $uBatteryCapacity";
		fLog "shutdown aborted";
		exit 0;
	else
		#debug only
		fLog "shutting down via halt -i -h now";
		#we do not want to do a poweroff since the servers are set via bios for power-on to last state.
		/sbin/halt -i -h;

	fi
fi

