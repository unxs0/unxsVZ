#!/bin/bash
#FILE
#	buildcheck.sh
#PURPOSE
#	Run build enviroment checks before compiling and installing the iDNS suite.
#AUTHOR
#	Hugo Urquiza for Unixservice. (C) 2008-2009
#

source ./script-include.sh
#
#C compiler flags for buildtest.c compilation. Please review and update for your system
export CFLAGS="-DLinux -Wall"

#
#Please don't modify the variables below unless you are a developer
libucidrMajor=2;
libucidrMinor=0;

libtemplateMajor=2;
libtemplateMinor=0;

uBindMajor="9";
uMySQLMajor="5";


function CheckMySQLSetup
{
	#This function will check if MySQL is installed
	#and which version is running.
	#If not found, will try to install it
	#If an older version is found, will try to update.
	
	cMySQLBin=`which mysqld`;
	if [ "$cMySQLBin" = "" ]; then
		if [ ! -x /usr/libexec/mysqld ]; then
			if [ ! -x /usr/local/libexec/mysqld ]; then
				uMySQLServerInstall=1;
			else
				cMySQLBin="/usr/local/libexec/mysqld";
			fi
		else
			cMySQLBin="/usr/libexec/mysqld";
		fi
	fi

	if [ "$cMySQLBin" != "" ]; then
		CheckMySQLVersion;
	else
		echo "I could not determine the location of your MySQL server binary.";
		BashInput "Please enter it for checking the version number or press <Enter> to install MySQL server." "";
		cMySQLBin=$cBashInput;

		if [ "$cMySQLBin" != "" ]; then 
			CheckMySQLVersion; 
		else
			#Install MySQL-server
			cWarning="Currently Debian, CentOS, RedHat and Gentoo supported only.\nPlease contribute on how to install MySQL for the other distributions.";
			if [ "$DISTRO" = "SuSe" ]; then
				echo $cWarning;
			elif [ "$DISTRO" = "RedHat" ]; then
				yum install mysql.$ARCH;
				yum install mysql-server.$ARCH;
				yum install mysql-devel.$ARCH;
			elif [ "$DISTRO" = "Slackware" ]; then
				echo $cWarning;
			elif [ "$DISTRO" = "Debian" ]; then
				apt-get install mysql.$ARCH;
				apt-get install mysql-server.$ARCH;
				apt-get install mysql-devel.$ARCH;
			elif [ "$DISTRO" = "Mandrake" ]; then
				echo $cWarning;
			elif [ "$DISTRO" = "Gentoo" ]; then
				emerge -av mysql.$ARCH;
				emerge -av mysql-server.$ARCH;
				emerge -av mysql-devel.$ARCH;
			elif [ "$DISTRO" = "Unknown"]; then
				echo "WARNING: I don't know how to install MYSQL. You will have to do it by yourself.";
			fi
			#Commoon for all distros? At least supported ones yes
			echo "Starting MySQL server...";
			/etc/init.d/mysqld start
			if [ "$?" != 0 ]; then
				echo "Warning: Could not start the MySQL server! Installation will fail.";
			else
				#Set MySQL root user password
				echo -n "Please enter the MySQL root user password you want to use: ";
				while [ "$cPass" = "" ]
				do
					read cPass;
				done
				cMySQLAdmin=`which mysqladmin`;
				$cMySQLAdmin -u root password $cPass;
				if [ "$?" != 0 ]; then
					echo "Warning: Could not set MySQL root user password.";
				else
					echo "MySQL root user password set OK.";
				fi
			fi
			echo "installed_mysql-server:$DISTRO" >> install.log;
		fi
	fi
}

function CheckMySQLVersion
{
	uMySQLVersion=`$cMySQLBin --version | cut -d " " -f 4 | cut -d . -f 1`;
	if [ "$?" = "0" ]; then
		if [ $uMySQLVersion -gt $uMySQLMajor ] || [ $uMySQLVersion -eq $uMySQLMajor ]; then 
			echo "Your MySQL server major version is $uMySQLVersion. That's OK for iDNS";
		else
			echo "Your MySQL server major version is $uMySQLVersion. iDNS requires at least version 5.0";
			BashInput "Do you want me to update it? (Y/N)" "yes";
			choice=$cBashInput;
			if [ "$choice" = "Y" ] || [ "$choice" = "y" ] || [ "$choice" = "yes" ]; then
				cWarning="Currently Debian, CentOS, RedHat and Gentoo supported only.\nPlease contribute on how to update MySQL for the other distributions.";
				if [ "$DISTRO" = "SuSe" ]; then
					echo $cWarning;
				 elif [ "$DISTRO" = "RedHat" ]; then
				 	yum update mysql.$ARCH;
				 	yum update mysql-server.$ARCH;
					yum update mysql-devel.$ARCH;
				elif [ "$DISTRO" = "Slackware" ]; then
					echo $cWarning;
				elif [ "$DISTRO" = "Debian" ]; then
					apt-get update mysql.$ARCH;
					apt-get update mysql-server.$ARCH;
					apt-get update mysql-devel.$ARCH;
					elif [ "$DISTRO" = "Mandrake" ]; then
					echo $cWarning;
				elif [ "$DISTRO" = "Gentoo" ]; then
					emerge -av --update mysql.$ARCH;
					emerge -av --update mysql-server.$ARCH;
					emerge -av --update mysql-devel.$ARCH;
				elif [ "$DISTRO" = "Unknown"]; then
					echo "WARNING: I don't know how to update MySQL. You will have to do it by yourself.";
				fi

				#Restart MySQL server if we updated it
				/etc/init.d/mysqld restart
				if [ "$?" != "0" ]; then
					echo "Warning: Could not restart the MySQL server";
				fi
			else
				echo "Warning: Some iDNS/idnsAdmin functions won't work with your current MySQL version. You've been warned.";
			fi
		fi
	else
		echo "Warning: I could not run the MySQL server binary at $cMySQLBin";
		echo "Warning: Could not determine MySQL server version number";
	fi
}


function CheckBINDSetup
{
	#
	#This function will look for the BIND binary
	#if not found, it will try to figure out which distribution 
	#the user has installed. If successful it will use the correct
	#package manager to fetch the BIND package and will install it.
	#

	if [ "$cBindLocation" = "" ]; then 
		cBindLocation=`which named`; 
		if [ "$?" != "0" ]; then cBindLocation='_install_'; fi;
	fi
	
	if [ "$cBindLocation" = "_install_" ]; then
		echo "It seems you don't have BIND installed.";
		BashInput "I can try installing BIND for you. Do you want me to do so? (Y/N)" "yes";
		choice=$cBashInput;
		if [ "$choice" = "N" ] || [ "$choice" = "n" ] || [ "$choice" = "no" ]; then return; fi

		#
		#Now based on the distribution information, we will istall BIND
		cWarning="Currently Debian, CentOS, RedHat and Gentoo supported only.\nPlease contribute on how to install BIND for the other distributions.";
		
		if [ "$DISTRO" = "SuSe" ]; then
			echo $cWarning;
		elif [ "$DISTRO" = "RedHat" ]; then
			yum install bind;
			yum install bind-utils;
		elif [ "$DISTRO" = "Slackware" ]; then
			echo $cWarning;
		elif [ "$DISTRO" = "Debian" ]; then
			apt-get install bind;
			apt-get install bind-utils;
		elif [ "$DISTRO" = "Mandrake" ]; then
			echo $cWarning;
		elif [ "$DISTRO" = "Gentoo" ]; then
			emerge -av bind;
			emerge -av bind-utils;
		elif [ "$DISTRO" = "Unknown"]; then
			echo "WARNING: I don't know how to install BIND. You will have to do it by yourself.";
		fi
		echo installed_bind:$DISTRO >> install.log
	else
		#Check BIND version
		echo "BIND found at $cBindLocation";
		echo "Checking your BIND version...";

		uMajor=`$cBindLocation -v | cut -f 2 -d " " | cut -f 1 -d .`;
		if [ "$uMajor" -lt "$uBindMajor" ]; then
			echo "WARNING: Your BIND major version number is $uMajor";
			echo "WARNING: iDNS requires at least version $uBindMajor. You should upgrade your BIND server.";
			BashInput "I will try to update it if you want (Y/N)" "yes";

			if [ "$choice" = "N" ] || [ "$choice" = "n" ] || [ "$choice" = "no" ]; then return; fi
		
			DISTRO=`WhatDistro`;
			echo "It seems you are running $DISTRO Linux.";

			 if [ "$DISTRO" = "SuSe" ]; then
			 	echo $cWarning;
			elif [ "$DISTRO" = "RedHat" ]; then
				yum update bind;
			elif [ "$DISTRO" = "Slackware" ]; then
				echo $cWarning;
			elif [ "$DISTRO" = "Debian" ]; then
				apt-get update bind;
			elif [ "$DISTRO" = "Mandrake" ]; then
				echo $cWarning;
			elif [ "$DISTRO" = "Gentoo" ]; then
				emerge --update -av bind;
			elif [ "$DISTRO" = "Unknown" ]; then
				echo "WARNING: I don't know how to update BIND. You will have to do it by yourself.";
			fi
		else
			echo "Your BIND major version is $uMajor. That's OK for iDNS";
		fi
	fi
	
}


function WhatDistro
{
	#This function will try to figure out which Linux distribution are you using.

	if [ -r "/etc/SuSE-release" ]; then
		DISTRO="SuSe";
	elif [ -r "/etc/redhat-release" ]; then
		DISTRO="RedHat";
	elif [ -r "/etc/slackware-release" ]; then
		DISTRO="Slackware";
	elif [ -r "/etc/debian_version" ]; then
		DISTRO="Debian";
	elif [ -r "/etc/mandrake-release" ]; then
		DISTRO="Mandrake";
	elif [ -r "/etc/gentoo-release" ]; then
		DISTRO="Gentoo";
	elif [ "1" ]; then
		DISTRO="Unknown";
	fi

	echo $DISTRO;

}
function libucidrInstall
{
	if [ ! -d ../libs/ucidr/ ]; then
		#
		#This could be handled more gracefully,
		#like doing a wget to the openisp.net site ;)
		#
		echo "Could not find libucidr in repository";
		return;
	fi
	cd ../libs/ucidr/ && make install
	cd -
	echo "installed_ucidr" >> install.log
}

function libtemplateInstall
{
	if [ ! -d ../libs/template ]; then
		echo "Could not find libtemplate in repository";
		return;
	fi
	cd ../libs/template && make install
	cd -
	echo "installed_template" >> install.log
}


function libCryptCheck
{
	if [ ! -r /usr/lib/libcrypt.a ]; then
		if [ ! -r /usr/lib64/libcrypt.a ]; then
			echo "libcrypt.a was not found in your system.";
			echo "If you have it at your system, please create a symbolic link to it at /usr/lib/libcrypt.a and retry the installation.";
			exit 1;
		else
			ln -s /usr/lib64/libcrypt.a /usr/lib/libcrypt.a;
		fi
	fi
		
}


#Main program start point

DISTRO=`WhatDistro`;
ARCH=`uname -m`;

echo "It seems you are running $DISTRO Linux.";

libCryptCheck;
CheckMySQLSetup;
CheckBINDSetup;

#Test 1 - MySQL libs and header files.

#Check for MySQL library. Try to guess where is it if not set by the env var iDNS_MYSQL_LIB

if [ "$iDNS_MYSQL_LIB" = "" ]; then

if [ ! -r /usr/lib/mysql/libmysqlclient.a ]; then
        echo "MySQL lib not found at /usr/lib/mysql/libmysqlclient.a";
        if [ ! -r /usr/local/lib/mysql/libmysqlclient.a ]; then
                echo "MySQL lib not found at /usr/local/lib/mysql/libmysqlclient.a";
                if [ ! -r /usr/lib64/mysql/libmysqlclient.a ]; then
                        echo "MySQL lib not found at /usr/lib64/libmysqlclient.a";
                        if [ ! -r /usr/local/lib64/mysql/libmysqlclient.a ]; then
                                #No MySQL lib found
                                echo "This script could not find the required MySQL library libmysqlclient.a";
                                echo "Please set the iDNS_MYSQL_LIB environment variable with the full path to this file.";
                                exit 1;
                        else
                                export iDNS_MYSQL_LIB=/usr/local/lib64/mysql/libmysqlclient.a;
                        fi
                else
                        export iDNS_MYSQL_LIB=/usr/lib64/mysql/libmysqlclient.a;
                fi
        else
                export iDNS_MYSQL_LIB=/usr/local/lib/mysql/libmysqlclient.a;
        fi
else
        export iDNS_MYSQL_LIB=/usr/lib/mysql/libmysqlclient.a;
fi

fi
#Check for MySQL include file location. Unfortunately there's no env var to set with that value.
if [ ! -r /usr/include/mysql/mysql.h ]; then
	if [ ! -r /usr/local/include/mysql/mysql.h ]; then
		echo "It seems you don't have the MySQL development package installed.";
		echo "That package is required for building iDNS. Can't continue";
		exit 1;
	fi
fi

echo "Your MySQL lib file file is at ${iDNS_MYSQL_LIB}";
echo "You have the MySQL devel package installed OK";

SVN=0;

#Pre-test 2 and 3, get latest libs version if needed
#Are you using svn?
if [ -r ./.svn/entries ]; then
	echo "You are using SVN";
	SVN=1;
else
	echo "You seem to be using a tarball";
fi
		
#Test 2 - OpenISP libraries (non fatal errors, makefile will take care of install them if not present.)

if [ ! -r /usr/lib/openisp/libucidr.a ] || [ ! -r /usr/include/openisp/ucidr.h ] ; then
	BashInput "OpenISP libucidr is not installed. Do you want me to install it? (Y/N)" "yes";
	if [ "$cBashInput" = "Y" ] || [ "$cBashInput" = "y" ] || [ "$cBashInput" = "yes" ]; then
		libucidrInstall;
	else
		DONT_TEST=1;
		echo "Warning: your building environment won't be checked before running the make command";
	fi
else
	#Check version, if outdated reinstall
	strings /usr/lib/openisp/libucidr.a | grep -m 1 Version > /tmp/libucidr.version;
	uMajor=`cat /tmp/libucidr.version | cut -f 2 -d _`;
	uMinor=`cat /tmp/libucidr.version | cut -f 3 -d _`;

	if [ "$uMajor" != "" ] && [ "$uMinor" != "" ]; then
		if [ $uMajor -lt $libucidrMajor ] && [ $uMinor -lt $libucidrMinor]; then
			echo "You are runing libucidr version $uMajor.$uMinor";
			BashInput "You may want to install latest version (Y/N)" "yes";
			if [ "$cBashInput" = "Y" ] || [ "$cBashInput" = "y" ] || [ "$cBashInput" = "yes" ] ; then
				libucidrInstall;
			else
				echo "Warning: You have chosen to NOT update libucidr. Compilation might fail!";
			fi
		else
			echo "libucidr Version $uMajor.$uMinor OK";
		fi
	else
		echo "I could not determine libucidr version number.";
		BashInput "Do you may want to install latest version (Y/N)" "yes";
		if [ "$cBashInput" = "Y" ] || [ "$cBashInput" = "y" ] || [ "$cBashInput" = "yes"]; then
			libucidrInstall;
		else
			DONT_TEST=1;
			echo "Warning: your building environment won't be checked before running the make command";
		fi
	fi

	echo "OpenISP libucidr OK";
fi

if [ ! -r /usr/lib/openisp/libtemplate.a ] || [ ! -r /usr/include/openisp/template.h ] ; then
	BashInput "OpenISP libtemplate is not installed. Do you want me to install it? (Y/N)" "yes";
	if [ "$cBashInput" = "Y" ] || [ "$cBashInput" = "y" ] || [ "$cBashInput" = "yes" ]; then
		libtemplateInstall;
	else
		DONT_TEST=1;
		echo "Warning: your building environment won't be checked before running the make command";
	fi
else
	strings /usr/lib/openisp/libtemplate.a | grep -m 1 Version > /tmp/libtemplate.version;
	uMajor=`cat /tmp/libtemplate.version | cut -f 2 -d _`;
	uMinor=`cat /tmp/libtemplate.version | cut -f 3 -d _`;

	if [ "$uMajor" != "" ] && [ "$uMinor" != "" ]; then
		if [ $uMajor -lt $libtemplateMajor ] && [ $uMinor -lt $libtemplateMinor]; then
			echo "You are runing libtemplate version $uMajor.$uMinor";
			BashInput "You may want to install latest version (Y/N)" "yes";
			if [ "$cBashInput" = "Y" ] || [ "$cBashInput" = "y" ] || [ "$cBashInput" = "yes"]; then
				libtemplateInstall;
			else
				echo "Warning: You have chosen to NOT update libtemplate. Compilation might fail!";
			fi
		else
			echo "libtemplate Version $uMajor.$uMinor OK";
		fi
	else
		echo "I could not determine libtemplate version number.";
		BashInput "Do you may want to install latest version (Y/N)" "yes";
		if [ "$cBashInput" = "Y" ] || [ "$cBashInput" = "y" ] || [ "$cBashInput" = "yes"]; then
			libtemplateInstall;
		else
			DONT_TEST=1;
			echo "Warning: your building environment won't be checked before running the make command";
		fi
	fi

	echo "OpenISP libtemplate OK";
fi

#Test 3 - Compile a small test program

if [ "$DONT_TEST" != 1 ]; then
	if [ -r ./buildtest.c ]; then
		gcc $CFLAGS -c buildtest.c;
		if [ ! -r ./buildtest.o ]; then
			echo "I could not compile buildtest.c Is your C compiler working?";
			exit 1;	
		fi
		gcc -o delme buildtest.o -lz -lm /usr/lib/libcrypt.a /usr/lib/openisp/libucidr.a $iDNS_MYSQL_LIB /usr/lib/openisp/libtemplate.a;
		if [ -x ./delme ]; then
			echo "Your build environment appears to be correct for iDNS suite compilation and install.";
			echo "Enjoy!";
			rm ./delme;
		else
			echo "I could not link buildtest.o with the required libraries. Your iDNS compile is probably to fail.";
			exit 1;
		fi
	else
		echo "I could not compile buildtest.c because the file is missing. Check your downloaded copy of the software.";
		exit 1;
	fi
else
	echo "Warning: I won't try to run a compilation test as some libraries are missing. Your compilation might fail!";
fi


#
#This replaces the exit 0
export BUILDOK=1;


