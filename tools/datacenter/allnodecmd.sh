#!/bin/bash
#
#Simple example for an all node ssh based command script.
#
#In this example all your hardware nodes are called $cNameX
#where N is in the hardcoded sequence of the for loop.
#       Example node1lax-f2, node2lax-f2....node7lax-f2 are the
#       hostnames of your hardware nodes.
#
#Prerequisites
#       ssh-keygen all node to all node ssh passwordless setup.
#

#Set this
cNamePrefix="node";
cNameSuffix="vm";
uPort="22";

if [ ! "$1" ]; then
	echo "Must specify a valid command";
	exit;
fi

#Set the seq range for your datacenter. See man seq for more info.
for N in $(seq 1 2 ); do
	echo $cNamePrefix$N$cNameSuffix;
	/usr/bin/ssh -C -c blowfish -p $uPort $cNamePrefix$N$cNameSuffix "$1";
done
