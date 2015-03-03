#!/bin/bash

#FILE
#	/etc/unxsvz/iptables.local.sh
#PURPOSE
#	To be used by node/datacenter unxsVZ scripts that modify iptables
#EXAMPLE
#	The distributed scripts will have a top section with default settings.
#	The scripts will source this file for localization of said settings.
#AUTHOR
#	Gary Wallis for Unixservice, LLC. Copyright 2014.
#NOTES
#	All /usr/sbin/*.sh scripts need to be changed to use this local.sh file.
#	Need to avoid setting collision. This could be done by prefixing vars with script name
#	in the cases where we need different versions of similar settings per script.
#	Other settings should be global for all scripts.

#This file contains confidential info keep secure!

#/etc/sysconfig/iptables
cNewRange="12.34.56.78";#this is current datacenter proxy class C base
cRefRange="1.2.3.4";#this is what is in the template /etc/sysconfig/iptables file that need to be replaced
cRefRange2="2.3.4.5";#this also maybe found in the template

#datacenter wide
cDomain="isp.net";
cZabbixActiveServer="zabbix-proxy-datacenter0";

#this is the proxy server ID get from zabbix
uZabbixId="12345";
