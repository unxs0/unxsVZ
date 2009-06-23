#!/bin/bash
#
#Simple example for an all node scp script.
#
#In this example all your hardware nodes are called $cNameX
#where N is in the hardcoded sequence of the for loop.
#	Example node1lax-f2, node2lax-f2....node7lax-f2 are the
#	hostnames of your hardware nodes.
#
#Prerequisites
#	ssh-keygen all node to all node ssh passwordless setup.
#

#Set this
cNameSuffix="lax-f2";
#cNameSuffix="vm";
cNamePrefix="node";

cThisHost=`/bin/hostname`;

if [ ! $1 ]; then
        echo "Must specify fully qualified path of file to replace";
        exit;
fi

#Set the seq range for your datacenter
for N in $(seq 1 7); do
	if [ "$cNamePrefix$N$cNameSuffix" == "$cThisHost" ];then
        	echo skipping this host $cThisHost;
	else
        	echo $cNamePrefix$N$cNameSuffix;
        	/usr/bin/scp "$1" $cNamePrefix$N$cNameSuffix:"$1";
	fi
done
