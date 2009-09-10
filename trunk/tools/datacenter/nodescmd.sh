#!/bin/bash

if [ ! "$1" ]; then
        echo "Must specify start node number";
        exit;
fi

if [ ! "$2" ]; then
        echo "Must specify end node number";
        exit;
fi

if [ ! "$3" ]; then
        echo "Must specify remote command";
        exit;
fi

#make sure you customize for your node naming scheme
uPort="22";
cPrefix="node";
cSuffix="vm";
for N in $(seq $1 $2); do
        echo $cPrefix$N$cSuffix;
        ssh -p $uPort $cPrefix$N$cSuffix "$3";

        if [ $? != 0 ]; then
                echo "node$N:$3 failed!";
        fi
done
