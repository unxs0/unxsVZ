#!/bin/bash

#build rpm

cUser=`whoami`;
if [ "$cUser" != "unxs" ];then
	echo "run as unxs error";
	exit 1;
else
	echo "user unxs ok";
fi

rpmbuild -bb /home/unxs/unxsVZ/unxsAdmin/unxsadmin.spec
if [ $? != 0 ];then
	echo "rpmbuild error";
	exit 1;
else
	echo "rpmbuild ok";
fi

cVersion=`grep 'Version:' /home/unxs/unxsVZ/unxsAdmin/unxsadmin.spec | cut -f 2 -d ' '`;
cRelease=`grep 'Release:' /home/unxs/unxsVZ/unxsAdmin/unxsadmin.spec | cut -f 2 -d ' '`;
if [ "$cVersion" != "" ] && [ "$cRelease" != "" ];then
	echo "$cVersion-$cRelease";
	sudo cp -i /home/unxs/rpm/RPMS/noarch/unxsadmin-$cVersion-$cRelease.noarch.rpm /var/www/html/rpm/noarch/
	sudo /usr/bin/createrepo /var/www/html/rpm/
else
	echo "cVersion or cRelease parse error";
	exit 1;
fi

#debug exit
#exit 0;

#create the source file for distribution as per .spec

#note the 6 for the /src dir
cFile=`grep 'Source:' /home/unxs/unxsVZ/unxsAdmin/unxsadmin.spec | cut -f 6 -d /`;

if [ "$cFile" != "" ];then
	tar -czv -C /home/unxs/rpm/BUILD/unxsadmin-root -f /home/unxs/rpm/SOURCES/$cFile .
	if [ $? == 0 ];then
		sudo cp -i /home/unxs/rpm/SOURCES/$cFile /var/www/html/rpm/src/$cFile;
	else
		echo "tar error";
		exit 1;
	fi
else
	echo "cFile parse error";
	exit 1;
fi
