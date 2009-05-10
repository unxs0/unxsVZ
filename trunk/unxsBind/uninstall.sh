#!/bin/bash
#FILE	uninstall.sh
#
#PURPOSE
#	Uninstalls the iDNS system
#AUTHOR
#	(C) 2008-2009 Hugo Urquiza for Unixservice.
#


cWhoAmI=`whoami`;

if [ "$cWhoAmI" != "root" ]; then
	echo "Error: You can only run this script as root.";
	exit 1;
fi


source ./script-include.sh

ARCH=`uname -m`;

if [ "$CGIDIR" = "" ]; then
	BashInput "Please specify your cgi-bin directory location" "/var/www/cgi-bin/";
	CGIDIR=$cBashInput;
fi

if [ "$cWebdir" = "" ]; then
	BashInput "Please specify your web server root directory" "/var/www/html/";
	cWebdir=$cBashInput;
fi

#
#Uninstall BIND if iDNS installed it
grep installed_bind install.log > /dev/null 2>&1;
if [ "$?" = "0" ]; then
	#Stop BIND
	killall -TERM named
	echo -n "Uninstalling BIND... ";
	DISTRO=`grep installed_bind install.log | cut -d ":" -f 2`;
	cWarning="Currently Debian, CentOS, RedHat and Gentoo supported only.\nPlease contribute on how to uninstall BIND for the other distributions.";
	if [ "$DISTRO" = "SuSe" ]; then
       		echo $cWarning;
	elif [ "$DISTRO" = "RedHat" ]; then
	        yum remove bind;
		yum remove bind-utils;
	elif [ "$DISTRO" = "Slackware" ]; then
        	echo $cWarning;
	elif [ "$DISTRO" = "Debian" ]; then
        	apt-get remove bind;
		apt-get remove bind-utils;
	elif [ "$DISTRO" = "Mandrake" ]; then
        	echo $cWarning;
	elif [ "$DISTRO" = "Gentoo" ]; then
        	emerge --unmerge bind;
		emerge --unmerge bind-utils;
	elif [ "$DISTRO" = "Unknown" ]; then
        	echo "WARNING: I don't know how to uninstall BIND. You will have to do it by yourself.";
	fi
	if [ "$?" = "0" ]; then
		echo "BIND uninstalled OK";
	else
		echo "BIND uninstall failed";
	fi
fi

#
#Uninstall Unixservice's libraries
echo -n "Uninstalling iDNS libraries... "
grep installed_ucidr install.log > /dev/null 2>&1;
if [ "$?" = "0" ]; then
	rm /usr/lib/openisp/libucidr.a;
	rm /usr/include/openisp/ucidr.h;
fi

grep installed_template install.log > /dev/null 2>&1;
if [ "$?" = "0" ]; then
	rm /usr/lib/openisp/libtemplate.a;
	rm /usr/include/openisp/template.h;
fi
echo "OK";

echo -n "Uninstalling iDNS binary and image files... ";
rm -f $CGIDIR/iDNS.cgi
rm -f $cWebdir/images/calendar.gif;
rm -f $cWebdir/images/calendar_mo.gif;
rm -f $cWebdir/images/hairline.gif;
rm -f $cWebdir/images/left.gif;
rm -f $cWebdir/images/left_on.gif;
rm -f $cWebdir/images/right.gif;
rm -f $cWebdir/images/right_last.gif;
rm -f $cWebdir/images/right_on.gif;
rm -f $cWebdir/images/topleft.gif;
rm -f $cWebdir/images/topright.gif;

echo "OK";

echo -n "Removing iDNS database... ";

DBIP=`grep "^#define[* /s]DBIP0" local.h | awk '{print $3}' | sed s/\"//g`;
DBLOGIN=`grep "^#define[* /s]DBLOGIN" local.h | awk '{print $3}' | sed s/\"//g`;
DBPASSWD=`grep "^#define[* /s]DBPASSWD" local.h | awk '{print $3}' | sed s/\"//g`;
if [ "$DBIP" != "NULL" ]; then
	echo "drop database idns" | mysql -h $DBIP -u $DBLOGIN -p$DBPASSWD > /dev/null 2>&1;
else
	echo "drop database idns" | mysql -u $DBLOGIN -p$DBPASSWD > /dev/null 2>&1;
fi

if [ "$?" != "0" ]; then
	echo "Warning: Could not drop iDNS database. Already deleted?";
else
	echo "OK";
fi

echo -n "Removing iDNS directory... ";
rm -Rf /usr/local/idns;
echo "OK";

#
#Uninstall the MySQL server if iDNS installed it
grep installed_mysql-server install.log > /dev/null 2>&1;
if [ "$?" = "0" ]; then
	echo "Uninstalling MySQL server...";
	/etc/init.d/mysqld stop
	DISTRO=`grep installed_mysql-server install.log | cut -d ":" -f 2`;
	cWarning="Currently Debian, CentOS, RedHat and Gentoo supported only.\nPlease contribute on how to uninstall MySQL server for the other distributions.";
	if [ "$DISTRO" = "SuSe" ]; then
       		echo $cWarning;
	elif [ "$DISTRO" = "RedHat" ]; then
	        yum remove mysql-server.$ARCH;
		yum remove mysql-devel.$ARCH;
		yum remove mysql.$ARCH;
	elif [ "$DISTRO" = "Slackware" ]; then
        	echo $cWarning;
	elif [ "$DISTRO" = "Debian" ]; then
        	apt-get remove mysql-server.$ARCH;
		apt-get remove mysql-devel.$ARCH;
		apt-get remove mysql.$ARCH;
	elif [ "$DISTRO" = "Mandrake" ]; then
        	echo $cWarning;
	elif [ "$DISTRO" = "Gentoo" ]; then
        	emerge --unmerge mysql-server.$ARCH;
		emerge --unmerge mysql-devel.$ARCH;
		emerge --unmerge mysql.$ARCH;
	elif [ "$DISTRO" = "Unknown" ]; then
        	echo "WARNING: I don't know how to uninstall MySQL server. You will have to do it by yourself.";
	fi
	if [ "$?" = "0" ]; then
		echo "MySQL server uninstalled OK";
	else
		echo "MySQL server uninstall failed";
	fi
	
	#Remove the MySQL dir
	rm -Rf /var/lib/mysql;
fi

echo -n "Cleaning up... ";
rm -f install.log
echo "OK";

echo "Uninstall completed OK";

