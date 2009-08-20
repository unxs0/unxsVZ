Summary: Manage ISP customers, resellers and their resources. Centralize resource and product usage.
Name: unxsisp
Version: 1.1
Release: 2
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsisp-1.1.tar.gz
URL: http://openisp.net/openisp/unxsVZ
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: mysql-server >= 5.0.45, unxsadmin

%description
Manage ISP customers, resellers and their resources. Centralize resource and product usage. This software also centralizes customer data for all the other Unixservice applications, to avoid unnecessary data duplication. All Unixservice ISP management applications can be controlled with this backend.

%prep
%setup

%build
make
cd interfaces/admin/
make
cd $RPM_BUILD_DIR

%install
install -s unxsISP.cgi /var/www/unxs/cgi-bin/unxsISP.cgi
install -s interfaces/admin/interface.cgi /var/www/unxs/cgi-bin/ispAdmin.cgi
mkdir -p /usr/local/share/unxsISP/data
cp data/*.txt /usr/local/share/unxsISP/data 

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
				/var/www/unxs/cgi-bin/unxsISP.cgi Initialize ultrasecret > /dev/null 2>&1
				if [ $? == 0 ];then
					cInitialize="1"
				fi
			fi
		fi
	fi
fi
#let installer now what was done.
if [ "$cMySQLStart" == "1" ] && [ "$cInitialize" == "1" ];then
	echo "unxsISP has been installed, intialized and httpd have been started.";	
	echo "You can proceed to login to your unxsISP interfaces with your browser.";	
else 
	echo "It appears that one or more manual operations may be needed to finish";
	echo "your unxsISP installation.";
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
		echo "WARNING: Your unxsISP database was not initialized, run:";
		echo "export ISMROOT=/usr/local/share";
		echo "/var/www/unxs/cgi-bin/unxsISP.cgi Initialize <mysql-root-passwd>";	
		echo "Debug any problems, check via the mysql CLI, then if needed try again:";
		echo "/var/www/unxs/cgi-bin/unxsISP.cgi Initialize <mysql-root-passwd>";	
	fi
fi

%clean

%files
%doc INSTALL LICENSE
/var/www/unxs/cgi-bin/unxsISP.cgi
/var/www/unxs/cgi-bin/ispAdmin.cgi
/usr/local/share/unxsISP/

%changelog
* Mon Aug 17 2009 - Hugo Urquiza <support2@unixservice.com>
- Minor spec updates
* Sat Aug 14 2009 - Hugo Urquiza <support2@unixservice.com>
- Major .spec update and lots of code and template updates.
* Tue May 26 2009 - Hugo Urquiza <support2@unixservice.com>
- Initial RPM release


