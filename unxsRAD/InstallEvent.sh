#!/bin/bash

cwd=`pwd`

./utils/importTemplatesOnlyNew.sh event;
cd /var/local/unxsRAD/apps/unxsEV;

###
#create backend app
#
# create source files
/usr/sbin/unxsRAD ProcessJobQueue;
if [ "$1" != "AppOnly" ];then
make install;
# first time running you need to run the setup
mysql -pZorBexin unxsev -e "describe tAuthorize" > /dev/null;
if [ "$?" != "0" ];then
	echo "Initial db setup";
	export cInstallDir=/var/local/unxsRAD/apps;
	mysql -pZorBexin -e "drop database unxsev" > /dev/null;
	mysql -pZorBexin -e "create database unxsev" > /dev/null;
	/var/www/cgi-bin/unxsEV.cgi Initialize ZorBexin;
fi
#
###
fi

if [ "$1" != "BackendOnly" ];then
###
# import new templates for interface
#create interface app
# import interface templates into backend app db
export CGIDIR=/var/www/cgi-bin;
export unxsEV=/var/local/unxsRAD/apps/unxsEV;
./utils/importTemplates.sh;
cd interface;
make install;
cd $cwd;
###
fi

