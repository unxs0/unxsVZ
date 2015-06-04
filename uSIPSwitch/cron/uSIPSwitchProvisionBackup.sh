#!/bin/bash

#this is for backup non mysql server only need to refresh rules every so often

/usr/bin/kill -HUP `cat /var/run/uSIPSwitch.pid`;
if [ $? != 0 ];then
	echo "Load rules error";
fi
