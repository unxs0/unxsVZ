#!/bin/bash

git status | head -n 1 | grep "branch master" > /dev/null;
if [ "$?" != "0" ];then
	echo "not allowed for non master branch";
	exit 0;
fi

cwd=`pwd`
./utils/importTemplatesOnlyNew.sh spanish;
/usr/sbin/unxsRAD ProcessJobQueue;


cd /var/local/unxsRAD/apps/unxsAK;
export CGIDIR=/var/www/cgi-bin;
export unxsAK=/var/local/unxsRAD/apps/unxsAK;
./utils/importTemplates.sh;
cd interface;
make install;
cd $cwd;
