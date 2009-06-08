#!/bin/bash
#
#KISS script to only download new dhcpd.conf if we have a new version by checking the md5sum.
#
#By support@unixservice.com 6/2009
#

BASEURL="--no-check-certificate https://ex1.example.net:9456";

cd /tmp;

#prime system
if [ ! -f /tmp/dhcpd.conf.md5sum.prev ]; then
        /usr/bin/wget --quiet $BASEURL/dhcpd.conf.md5sum -O dhcpd.conf.md5sum.prev;
        if [ $? != 0 ];then
                echo "fatal error 0";
                exit 1;
        fi
        exit 0;
fi

/usr/bin/wget --quiet $BASEURL/dhcpd.conf.md5sum -O dhcpd.conf.md5sum;
if [ $? != 0 ];then
        echo "fatal error 1";
        exit 1;
fi

if [ -f /tmp/dhcpd.conf.md5sum.prev ] && [ -f /tmp/dhcpd.conf.md5sum ]; then
        diff /tmp/dhcpd.conf.md5sum.prev /tmp/dhcpd.conf.md5sum > /dev/null;
        if [ $? != 0 ];then
                /usr/bin/wget --quiet $BASEURL/dhcpd.conf -O dhcpd.conf;
                if [ $? != 0 ];then
                        echo "fatal error 2";
                        exit 1;
                fi
                cp /etc/dhcpd.conf /etc/dhcpd.conf.bak;
                if [ $? != 0 ];then
                        echo "fatal error 3";
                        exit 1;
                fi
                cp /tmp/dhcpd.conf /etc/dhcpd.conf;
                if [ $? != 0 ];then
                        echo "fatal error 4";
                        exit 1;
                fi
		/usr/bin/md5sum -c /tmp/dhcpd.conf.md5sum > /dev/null 2>&1;
                if [ $? != 0 ];then
			#roll back
			cp /etc/dhcpd.conf.bak /etc/dhcpd.conf;
                        echo "fatal error 5 roll back attempted";
                        exit 1;
                fi
		/etc/init.d/dhcpd configtest;
                if [ $? != 0 ];then
			#roll back
			cp /etc/dhcpd.conf.bak /etc/dhcpd.conf;
                        echo "fatal error 6 roll back attempted";
                        exit 1;
                fi
		/etc/init.d/dhcpd restart > /dev/null 2>&1;
                if [ $? != 0 ];then
                        echo "fatal error 6";
                        exit 1;
                fi
		
                mv /tmp/dhcpd.conf.md5sum /tmp/dhcpd.conf.md5sum.prev;
                if [ $? != 0 ];then
                        echo "fatal error 6";
                        exit 1;
                fi
        fi
fi
