#!/bin/bash

#FILE
#	boot-cluster.sh
#	$Id$
#LEGAL
#	Copyright by Unixservice, LLC. 2010.
#	GPLv2 license applies.
#NOTES
#	Assume fast internal LAN cluster interconnect with passwordless ssh set up.

#Configuration section
cMasterIP="192.168.22.128";
cTimeoutSecs="600";
cWarnEmail="supportgrp@unixservice.com";
#end

#our standard bash logging function.

#wait for master to come up. If does not come up after cTimeoutSecs send warning email, exit error.

#master is up, get master status.

#parse master status.

#make sure slave mode is off.

#SQL "change master to" parsed data on local slave MySQL server.


#start slave

#check status. if slave did not catch up in max cTimeoutSecs send warning email, exit error.
