#!/bin/bash
#
#FILE
# $Id: install.sh 2942 2008-05-08 17:26:13Z jpetersson2 $
#PURPOSE
#       Make a full install of iDNS easy.
#AUTHOR
#	Jonathan Petersson for Unixservice (C) 2008
#	Minor updates by Hugo Urquiza
#

cWhoAmI=`whoami`;

if [ "$cWhoAmI" != "root" ]; then
	echo "Error: You can only run this script as root.";
	exit 1;
fi

source ./script-include.sh

BashInput "Is this a first time install? (Y/N)" "yes";
choice=$cBashInput;
if [ "$choice" = "Y" ] || [ "$choice" = "y" ] || [ "$choice" = "yes" ]; then
	echo "If you run this install script in a server which already has iDNS installed,";
	echo "it will DROP YOUR iDNS DATABASE and REMOVE your /usr/local/idns directory.";
	BashInput "Are you sure this a first time install? (Y/N)" "yes";
	choice=$cBashInput;
	if [ "$choice" = "Y" ] || [ "$choice" = "y" ] || [ "$choice" = "yes" ]; then DELETE=1; fi
fi
# Predefined variables for mysql username and password, and BIND host, these can be entered with ARGV as well
#cUser=
#cPass=
#cHost=

# CGIDIR is identified automatically but can be set manually here or with ARGV
#export CGIDIR=

# Default variables for files and directories, DO NOT change!
iDNSCGI="iDNS.cgi"
cNamedDir="/usr/local/idns"

if [ `dirname $(dirname $0)` = "." ]; then
    export ISMROOT=`dirname $(pwd)`
else
    export ISMROOT=`dirname $(dirname $0)`
fi


# Check that all variables are set
if [ "$DELETE" = 1 ]; then
	if [ "${cUser}" = "" ] ; then
		BashInput "Enter the MySQL user name to be used for the installation" "root";
		cUser=$cBashInput;
	fi

	if [ "${cPass}" = "" ] ; then
		while [ "$cPass" = "" ]
		do
			echo -n "Enter the MySQL password to be used for the installation : ";
			read cPass;
		done
	fi

	if [ "${cHost}" = "" ]; then
		while [ "$cHost" = "" ]
		do
			echo -n "Enter the server IP address where you want BIND to listen: ";
			read cHost;
		done
	fi

	if [ "${cWebdir}" = "" ] ; then
		BashInput "Enter your web server root directory" "/var/www/html/";
		cWebdir=$cBashInput;
	fi
fi

# Try to locate default cgi-bin folder
if [ "$CGIDIR" = "" ] || [ ! -e "$CGIDIR" ]; then
    if [ -r "cgi-bin" ]; then
        CGIDIR=cgi-bin/
    elif [ -r "/var/www/cgi-bin" ]; then
        export CGIDIR=/var/www/cgi-bin/
        DISTRO=Fedora
    elif [ -r "/usr/lib/cgi-bin" ]; then
        export CGIDIR=/usr/lib/cgi-bin/
        DISTRO=Ubuntu
    elif [ -r "/usr/local/www/cgi-bin" ]; then
        export CGIDIR=/usr/local/www/cgi-bin
        DISTRO=FreeBSD
    else
        echo -n "Unable to locate your cgi-bin directory. Please specify it: ";
	read CGIDIR;
    fi
fi

install -s iDNS.cgi $CGIDIR"iDNS.cgi";
if [ "$?" != "0" ]; then
	echo "Fatal: Could not install iDNS.cgi at ${CGIDIR}iDNS.cgi";
	exit 1;
fi

rm iDNS.cgi

# Removes all old data
if [ "$DELETE" = 1 ]; then
    echo "drop database idns;" | mysql -u$cUser -p$cPass > /dev/null 2>&1;
    rm -Rf $cNamedDir
fi

cd $ISMROOT/iDNS

# Install images

if [ "$DELETE" = 1 ]; then

	if [ ! -x "${cWebdir}/images/" ]; then
		mkdir ${cWebdir}/images/;
		if [ ! -x "${cWebdir}/images/" ]; then
			echo "Could not create ${cWebdir}/images directory. Check your permissions";
			exit 1;
		fi
	fi

	cp ./images/* ${cWebdir}/images/

	if [ "$?" != "0" ]; then echo "Warning: Could not install images at ${cWebdir}/images/"; fi

#Debug only
#echo $CGIDIR
#echo $ISMROOT

    $CGIDIR/$iDNSCGI Initialize $cPass
    $CGIDIR/$iDNSCGI installbind $cHost
    $CGIDIR/$iDNSCGI allfiles master ns1.yourdomain.com 12
else
	if [ ! -r ./install.log ]; then
	    echo "You've chosen not to remove any old data. Any updates of the database that might've been added with this release has not been applied, iDNS.cgi is the only thing that has been updated!";
	else
		echo "Your iDNS.cgi binary has been updated";
	fi	  
fi

#Install interfaces
#-n flags skips re-running buildcheck.sh

if [ "$DELETE" = 1 ]; then
	cd interfaces/admin
	./install.sh -w $cWebdir -c $CGIDIR -n

	cd ../organization
	./install.sh -w $cWebdir -c $CGIDIR -n

	cd ..

#Make sure /usr/local/idns permissions are correctly set
	chown -R named /usr/local/idns;
	chmod -R 755 /usr/local/idns;
	#Start BIND
	cBindLocation=`which named`;
	$cBindLocation -u named -c /usr/local/idns/named.conf;

	if [ "$?" != "0" ]; then
		echo "Warning: could not start BIND";
		echo "Command was: $cBindLocation -u named -c /usr/local/idns/named.conf;"
	fi

	echo "Congratulations! You have iDNS installed at your server and ready for a test run.";
	echo "Don't forget to setup crontab for job queue processing (Please see the INSTALL file for details.)";
	echo;
fi

exit 0;
