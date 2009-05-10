#!/bin/bash
#FILE
#	installradius.sh
#PURPOSE
#	Do the FreeRADIUS install quickly.
#AUTHOR
#	Hugo Urquiza for Unixservice. (C) 2008
#

#Please try to keep this variable updated ;)
VERSION="2.0.4"
URL="ftp://ftp.freeradius.org/pub/freeradius/"

if [ ! -r "./freeradius-server-$VERSION.tar.bz2" ]; then
	echo "You don't have the FreeRADIUS tarball. Trying to get one...";
	WGET=`which wget`;
	if [ ! -x $WGET ]; then
		echo "It seems you don't have wget instaled or I can't find it. I can't continue";
		exit 1;
	fi
	
	$WGET $URL/freeradius-server-$VERSION.tar.bz2;

fi

#Unpack
tar jxf freeradius-server-$VERSION.tar.bz2;

cd ./freeradius-server-$VERSION
./configure; 
make;
make install;

cd ..

#Warning: raddb dir location is the default
cp -R ./setupradius/raddb/ /usr/local/etc/raddb

echo "You have your FreeRadius server installed and ready to run"



