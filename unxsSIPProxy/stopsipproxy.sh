#!/bin/bash

if [ -f /var/run/unxsSIPProxy.pid ];then
	/bin/kill -SIGINT `cat /var/run/unxsSIPProxy.pid` > /dev/null 2>&1;
	if [ "$?" != "0" ];then
		echo "/var/run/unxsSIPProxy.pid file exists but kill failed";
	fi
else
	echo "/var/run/unxsSIPProxy.pid does not exist";
	ps -ef | grep unxsSIPProxy | grep -v grep> /dev/null;
	if [ "$?" == "0" ];then
		echo "unxsSIPProxy is still running";
	fi
fi
