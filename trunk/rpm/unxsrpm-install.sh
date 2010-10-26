#!/bin/bash
#

if [ "$1" == "" ];then
	echo "usage $0 <rpm-prefix example: unxsvz-1.0-4> [<tar.gz-prefix example: unxsvz-1.0>]";
	exit 0;
fi

if [ ! -f $1.i386.rpm ];then
	echo "no $1.i386.rpm";
	c386RPM="no";
else
	c386RPM="yes";
fi

if [ ! -f $1.x86_64.rpm ];then
	echo "no $1.x86_64.rpm";
	c64RPM="no";
else
	c64RPM="yes";
fi

if [ ! -f $1.src.rpm ];then
	echo "error no $1.src.rpm";
	exit 1;
fi

if [ -f $2.tar.gz ];then
	mv $2.tar.gz /var/www/html/source/
	if [ $? != 0 ];then
		echo "error tar.gz mv";
	fi
	/usr/bin/md5sum /var/www/html/source/$2.tar.gz > /var/www/html/source/$2.tar.gz.md5sum
	if [ $? != 0 ];then
		echo "error tar.gz md5sum";
	fi
	chown root:root /var/www/html/source/$2.tar.gz
fi

if [ "$c386RPM" == "yes" ];then
	mv $1.i386.rpm /var/www/html/rpm/i386/
	if [ $? != 0 ];then
		echo "error 1 i386";
	fi
fi

if [ "$c64RPM" == "yes" ];then
	mv $1.x86_64.rpm /var/www/html/rpm/x86_64/
	if [ $? != 0 ];then
		echo "error 1 x86_64";
	fi
fi

mv $1.src.rpm /var/www/html/rpm/src/
if [ $? != 0 ];then
	echo "error 2";
	exit 1;
fi

if [ "$c386RPM" == "yes" ];then
	cd /var/www/html/rpm/i386/
	chown root:root $1.i386.rpm
	if [ $? != 0 ];then
		echo "error 3 i386";
	fi
	/usr/bin/md5sum $1.i386.rpm > $1.i386.rpm.md5sum
	if [ $? != 0 ];then
		echo "error 4 i386";
	fi
fi

if [ "$c64RPM" == "yes" ];then
	cd /var/www/html/rpm/x86_64/
	chown root:root $1.x86_64.rpm
	if [ $? != 0 ];then
		echo "error 3 x86_64";
	fi
	/usr/bin/md5sum $1.x86_64.rpm > $1.x86_64.rpm.md5sum
	if [ $? != 0 ];then
		echo "error 4 x86_64";
	fi
fi

cd /var/www/html/rpm/src/
chown root:root $1.src.rpm
if [ $? != 0 ];then
	echo "error 5";
	exit 1;
fi
/usr/bin/md5sum $1.src.rpm > $1.src.rpm.md5sum
if [ $? != 0 ];then
	echo "error 6";
	exit 1;
fi

/usr/bin/createrepo /var/www/html/rpm
if [ $? != 0 ];then
	echo "error 7";
	exit 1;
fi
