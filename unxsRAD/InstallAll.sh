#!/bin/bash

cwd=`pwd`
./utils/importTemplates.sh;
./utils/importTemplates.sh new;
make install;
/usr/sbin/unxsRAD ProcessJobQueue;


cd /var/local/unxsRAD/apps/unxsAK;
make install;
export CGIDIR=/var/www/cgi-bin;
export unxsAK=/var/local/unxsRAD/apps/unxsAK;
./utils/importTemplates.sh;
cd interface;
make install;
cd $cwd;
