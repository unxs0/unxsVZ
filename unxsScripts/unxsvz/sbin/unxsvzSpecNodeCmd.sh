#!/bin/bash
#
#FILE
#	/usr/sbin/unxsvzSpecNodeCmd.sh
#PURPOSE
#	Run a command via ssh for all specified nodes
#	Can provide a sequence list and may use the N macro in the command string
#AUTHOR/LEGAL
#	(C) 2014 Gary Wallis for Unixservice, LLC.

cEncAlg="arcfour";
cNamePrefix="node";
cNameSuffix="";
#
#user conf section
if [ -f "/etc/unxsvz/ssh.local.sh" ];then
	source /etc/unxsvz/ssh.local.sh;
fi
#
#

if [ "$3" == "" ]; then
        echo "usage $0: <start server> <end server> | --sequence <quoted list> <quoted command string, {N} macro available>";
        exit;
fi

if [ "$1" == "--sequence" ]; then
	for N in $2; do
       	 if [ "$cNamePrefix$N$cNameSuffix" != "$cThisHost" ];then
       	         echo $cNamePrefix$N$cNameSuffix;
			cExpandedCommand=`echo $3 | sed -e "s/{N}/$N/g"`;
       	         /usr/bin/ssh -c $cEncAlg $cNamePrefix$N$cNameSuffix "$cExpandedCommand";
       	 fi
	done
else
	for N in $(seq $1 $2 ); do
       	 if [ "$cNamePrefix$N$cNameSuffix" != "$cThisHost" ];then
       	         echo $cNamePrefix$N$cNameSuffix;
			cExpandedCommand=`echo $3 | sed -e "s/{N}/$N/g"`;
       	         /usr/bin/ssh -c $cEncAlg $cNamePrefix$N$cNameSuffix "$cExpandedCommand";
       	 fi
	done
fi

exit;
