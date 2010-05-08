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
#Notes
#	This new version returns a non zero value if any of the
#	node commands returns a non zero value.
#

#Set this
cNamePrefix="node";
cNameSuffix="vm";
uPort="22";
uRet=0;
cEncAlg="blowfish";
#for internal private lan connections try:
#cEncAlg="none";
#-C the compression option is not useful

if [ ! "$1" ]; then
	echo "Must specify a valid command";
	exit;
fi

#Set the seq range for your datacenter. See man seq for more info.
for N in $(seq 1 2 ); do
	echo $cNamePrefix$N$cNameSuffix;
	/usr/bin/ssh -c $cEncAlg -p $uPort $cNamePrefix$N$cNameSuffix "$1";
	uRet=$(($uRet + $?));
done

exit $uRet;
