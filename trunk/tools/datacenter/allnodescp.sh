#!/bin/bash
#
#Simple example for an all node scp script.
#
#In this example all your hardware nodes are called $cNameX
#where N is in the hardcoded sequence of the for loop.
#       Example node1lax-f2, node2lax-f2....node7lax-f2 are the
#       hostnames of your hardware nodes.
#
#Prerequisites
#       ssh-keygen all node to all node ssh passwordless setup.
#
#Required by jobqueue.c
#

#Set these to match your setup
cNamePrefix="node";
cNameSuffix="lax-f2";
#uPort="2322";
uPort="22";
#
#you may need to remove higher level domains
cThisHost=`/bin/hostname | cut -f 1 -d "."`;
#if not use this
#cThisHost=`/bin/hostname`;

if [ ! $1 ]; then
        echo "Must specify fully qualified path of file to replace";
        exit;
fi

#Set the seq range for your datacenter. See man seq for more options.
for N in $(seq 1 7 ); do
        if [ "$cNamePrefix$N$cNameSuffix" != "$cThisHost" ];then
                echo $cNamePrefix$N$cNameSuffix;
                /usr/bin/scp -C -c blowfish -P $uPort "$1" $cNamePrefix$N$cNameSuffix:"$1";
        fi
done
