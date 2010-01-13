#/bin/bash
#
#FILE
#	ShutdownOnUPSEvent.sh
#PURPOSE
#	Using snmpget connect to UPS snmpd (card) and if battery is below
#	uBatteryCapacity initiate shutdown sequence.
#AUTHOR
#	Gary Wallis and Hugo Urquiza for Unixservice, LLC. (C) 2009-2010

fLog() { echo "`date +%b' '%d' '%T` $0[$$]: $@"; }

#Set these for your SNMP enabled UPS
cHost="ups0.servicoopsa.com.ar";
cSnmpGet="/usr/bin/snmpget -v 1 -c pkabu $cHost";
cBatCapMIB="mib-2.33.1.2.4.0";
cACInMIB="mib-2.33.1.3.3.1.3.1";

#ping test to ups
ping -c 3 $cHost > /dev/null 2>&1;
if [ "$?" != "0" ]; then
	fLog "$cHost seems down";
	exit 1;
fi

uBatteryCapacity=`$cSnmpGet $cBatCapMIB | cut -f 4 -d : | cut -f 2 -d " "`;
if [ $? != 0 ];then
	fLog "$cBatCapMIB error";
	exit 1;
fi

uACLineVoltage=`$cSnmpGet $cACInMIB | cut -f 4 -d : | cut -f 2 -d " "`;
if [ $? != 0 ];then
	fLog "$cACInMIB error";
	exit 1;
fi

#set this sections hardcoded values to suit your needs
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

