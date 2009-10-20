#!/bin/bash

#simple example needs to be expanded for ssh port etc.

if [ ! $1 ] || [ ! $2 ] || [ ! $3 ]; then
        echo "usage $0: <fq path> <start node num> <end node num>";
        exit;
fi



for N in $(seq $2 $3); do
	echo node$N:;
		scp $1 node$N:$1
done
