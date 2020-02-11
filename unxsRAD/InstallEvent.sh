#!/bin/bash

cwd=`pwd`

###
#create backend app
#
# import new templates for backend
./utils/importTemplatesOnlyNew.sh event;
# create source files
/usr/sbin/unxsRAD ProcessJobQueue;
cd /var/local/unxsRAD/apps/unxsEV;
export CGIDIR=/var/www/cgi-bin;
export unxsEV=/var/local/unxsRAD/apps/unxsEV;
make install;
# first time running you need to run the setup
mysql -pZorBexin unxsev -e "describe tAuthorize" > /dev/null;
if [ "$?" != "0" ];then
	echo "Initial db setup";
	export cInstallDir=/var/local/unxsRAD/apps;
	/var/www/cgi-bin/unxsEV.cgi Initialize ZorBexin;
fi
#
###

###
#create interface app
# import interface templates into backend app db
./utils/importTemplates.sh;
cd $cwd;
###

