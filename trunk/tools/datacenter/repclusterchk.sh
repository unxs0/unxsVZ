#!/bin/bash

/usr/sbin/allnodecmd.sh "echo 'show slave status\G' | mysql -pZorBexin | grep Seconds_Behind_Master"
#/usr/sbin/nodescmd.sh 1 4 "echo 'show slave status\G' | mysql -pZorBexin | grep Seconds_Behind_Master"
