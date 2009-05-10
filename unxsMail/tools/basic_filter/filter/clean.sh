#!/bin/bash

echo "clean.sh"
date
for cFile in `find /var/spool/filter/quarantine/ -name "in.*" -ctime +3`; do
	#echo $cFile;
	rm $cFile;
done

for cFile in `find /var/spool/filter/trash/ -name "in.*" -ctime +3`; do
	#echo $cFile;
	rm $cFile;
done

for cFile in `find /var/spool/filter/trash/ -name "vr.*" -ctime +3`; do
	#echo $cFile;
	rm $cFile;
done

for cFile in `find /var/spool/filter/trash/ -name "vm.*" -ctime +3`; do
	#echo $cFile;
	rm $cFile;
done
