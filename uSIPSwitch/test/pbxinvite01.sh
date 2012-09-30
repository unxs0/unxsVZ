#!/bin/bash

if [ -d test ];then
	cd test;
fi
sipsak -vvv -s sip:101@65.49.53.120 -l 5062 -S -f pbxinvite01.sip.src;
