#!/bin/bash

cwd=`pwd`
./utils/importTemplatesOnlyNew.sh new;
/usr/sbin/unxsRAD ProcessJobQueue;


cd /var/local/unxsRAD/apps/unxsAK;
export CGIDIR=/var/www/cgi-bin;
export unxsAK=/var/local/unxsRAD/apps/unxsAK;
./utils/importTemplates.sh;
cd interface;
make install;
cd $cwd;
