#!/bin/bash

for cKey in `memdump --servers=127.0.0.1 2>/dev/null`;do
	echo $cKey;
	memcat --servers=127.0.0.1 $cKey;
	echo "---";
done
