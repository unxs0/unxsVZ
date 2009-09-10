#!/bin/bash

/usr/sbin/nodescmd.sh 1 2 "echo 'PURGE MASTER LOGS BEFORE DATE_SUB( NOW( ), INTERVAL 3 DAY);' | mysql -pUltraSecret"
