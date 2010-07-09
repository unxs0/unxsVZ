#!/bin/bash
for cFile in `/usr/bin/find /var/lib/rrd/*.rrd`; do
	echo $cFile;
	./removespikes.pl $cFile;
done
