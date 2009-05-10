#!/bin/bash

#Edit for correct IPs

/usr/sbin/mysql-proxy --proxy-lua-script=/etc/failover.lua --proxy-address=192.168.0.4:3306\
	--proxy-backend-addresses=192.168.0.1:3306\
	--proxy-backend-addresses=192.168.0.2:3306 --proxy-backend-addresses=196.25.27.3:3306\
	--admin-address=127.0.0.1:4041\
	--proxy-skip-profiling >> /var/log/mysql-proxy.log 2>&1 &
disown %1;
sleep 1;
echo $(pidof mysql-proxy) > /var/run/mysql-proxy.pid;
