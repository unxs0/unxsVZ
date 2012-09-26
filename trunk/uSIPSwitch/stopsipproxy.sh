#!/bin/bash

if [ -f /var/run/uSIPSwitch.pid ];then
	/bin/kill -SIGINT `cat /var/run/uSIPSwitch.pid` > /dev/null 2>&1;
	if [ "$?" != "0" ];then
		echo "/var/run/uSIPSwitch.pid file exists but kill failed";
	fi
else
	echo "/var/run/uSIPSwitch.pid does not exist";
	ps -ef | grep uSIPSwitch | grep -v grep> /dev/null;
	if [ "$?" == "0" ];then
		echo "uSIPSwitch is still running";
	fi
fi
