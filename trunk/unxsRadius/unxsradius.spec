Summary: Mission critical RADIUS server central administration system. 
Name: unxsradius
Version: 1.0
Release: 2
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsradius-1.0.tar.gz
URL: http://openisp.net/openisp/unxsVZ
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: mysql-server >= 5.0.45, unxsadmin, freeradius

%description
Mission critical RADIUS server central administration system. With company-contact-role model and support for disjoint sets of RADIUS server configuration and users. Can be controlled via master ISP unxsRadius/ispAdmin.

%prep
%setup

%build
make
cd unxsRadacct
make
cd $RPM_BUILD_DIR

%post
if [ -x /sbin/chkconfig ];then
	if [ -x /etc/init.d/httpd ];then
		/sbin/chkconfig --level 3 httpd on
		/etc/init.d/httpd restart > /dev/null 2>&1
		if [ $? == 0 ];then
			cHttpdStart="1"
		fi
	fi
	if [ -x /etc/init.d/mysqld ];then
		/sbin/chkconfig --level 3 mysqld on
		/etc/init.d/mysqld restart > /dev/null 2>&1
		if [ $? == 0 ];then
			cMySQLStart="1"
		fi
	fi
fi
#if mysqld has no root passwd and we started it then we will set it and finish the data initialize
if [ -x /usr/bin/mysql ];then
	if [ "$cMySQLStart" == "1" ];then
		echo "quit" | /usr/bin/mysql  > /dev/null 2>&1
		if [ $? == 0 ];then
			/usr/bin/mysqladmin -u root password 'ultrasecret' > /dev/null 2>&1
			if [ $? == 0 ];then
				echo "mysqld root password set to 'ultrasecret' change ASAP!"
				export ISMROOT=/usr/local/share
				/var/www/unxs/cgi-bin/unxsRadius.cgi Initialize ultrasecret > /dev/null 2>&1
				if [ $? == 0 ];then
					cInitialize="1"
				fi
			fi
		fi
	fi
fi
#let installer now what was done.
if [ "$cMySQLStart" == "1" ] && [ "$cInitialize" == "1" ];then
	echo "unxsRadius has been installed, intialized and httpd have been started.";	
	echo "You can proceed to login to your unxsRadius interfaces with your browser.";	
else 
	echo "It appears that one or more manual operations may be needed to finish";
	echo "your unxsRadius installation.";
	if [ "$cHttpdStart" != "1" ]; then
		echo "";
		echo "WARNING: Your httpd server was not started, run:";
		echo "/etc/init.d/httpd configtest";
		echo "Then check your httpd configuration and then:";
		echo "/etc/init.d/httpd start";
	fi
	if [ "$cMySQLStart" != "1" ]; then
		echo "";
		echo "WARNING: Your mysqld server was not started, run:";
		echo "/etc/init.d/mysqld start";
		echo "Debug any problems, then, if you do not already know your MySQL root password:";
		echo "/usr/bin/mysqladmin -u root password '<mysql-root-passwd>'";	
	fi
	if [ "$cInitialize" != "1" ]; then
		echo "";
		echo "WARNING: Your unxsRadius database was not initialized, run:";
		echo "export ISMROOT=/usr/local/share";
		echo "/var/www/unxs/cgi-bin/unxsRadius.cgi Initialize <mysql-root-passwd>";	
		echo "Debug any problems, check via the mysql CLI, then if needed try again:";
		echo "/var/www/unxs/cgi-bin/unxsRadius.cgi Initialize <mysql-root-passwd>";	
	fi
fi

%clean

%files
%doc INSTALL LICENSE
/var/www/unxs/cgi-bin/unxsRadacct.cgi
/var/www/unxs/cgi-bin/unxsRadius.cgi
/usr/local/share/unxsRadius/
/usr/local/share/unxsRadacct/data/
/etc/sysconfig/unxsradius
/etc/init.d/unxsradius

%changelog
* Tue Aug 18 2009 - Hugo Urquiza <support2@unixservice.com>
- Major .spec file update
* Mon May 25 2009 - Hugo Urquiza <support2@unixservice.com>
- Initial RPM release


