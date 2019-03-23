#!/bin/bash

git status | head -n 1 | grep "branch dev" > /dev/null;
if [ "$?" != "0" ];then
	echo "not allowed for non dev branch";
	exit 0;
fi

cwd=`pwd`
./utils/importTemplatesOnlyNew.sh spanish dev;
/usr/sbin/unxsRAD ProcessJobQueue;


cd /var/local/unxsRAD/apps/unxsAK;
export CGIDIR=/var/www/cgi-bin;
export unxsAK=/var/local/unxsRAD/apps/unxsAK;
ln -s /var/local/unxsRAD/apps/unxsAK/templates/default /var/local/unxsRAD/apps/unxsAK/templates/dev;
./utils/importTemplates.sh dev;
cd interface;
make install-test;
cd $cwd;
