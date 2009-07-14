Summary: sendmail and postfix controller as well as configuration generator for all mail services across multiple servers and multiple mail domains. 
Name: unxsmail
Version: 1.3
Release: 3
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsmail-1.3.tar.gz
URL: http://openisp.net/openisp/unxsVZ
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: mysql-server >= 5.0.45, unxsadmin

%description
New RAD3 based sendmail and postfix controller as well as configuration generator for all mail services across multiple servers and multiple mail domains. For ESMTP, pop3, imap, webmail, clamAV, Spamassassin, procmail, etc. Replaces older tech mysqlSendmail and mysqlPostfix.

%prep
%setup

%build
make
cd interfaces/user
make
cd $RPM_BUILD_DIR

%install
install -s unxsMail.cgi /var/www/unxs/cgi-bin/unxsMail.cgi
install -s interfaces/user/interface.cgi /var/www/unxs/cgi-bin/unxsMaiUser.cgi
mkdir -p /usr/local/share/unxsMail/data
cp data/*.txt /usr/local/share/unxsMail/data/

%post
#Post install stuff, initialize database and setup crontab
if [ -x /sbin/chkconfig ];then
	if [ -x /etc/init.d/sendmail ]; then
		/sbin/chkconfig --level 3 sendmail on
	fi

	if [ -x /etc/init.d/mysqld ];then
		/sbin/chkconfig --level 3 mysqld on
		/etc/init.d/mysqld restart > /dev/null 2>&1
		if [ $? == 0 ];then
			cMySQLStart="1"
		fi
	fi
	if [ -x /etc/init.d/httpd ];then
		/sbin/chkconfig --level 3 httpd on
		/etc/init.d/httpd restart > /dev/null 2>&1
		if [ $? == 0 ];then
			cHttpdStart="1"
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
				/var/www/unxs/cgi-bin/unxsMail.cgi Initialize ultrasecret > /dev/null 2>&1
				if [ $? == 0 ];then
					cInitialize="1"
				fi
			fi
		fi
	fi
fi
#let installer now what was done.
if [ "$cMySQLStart" == "1" ] && [ "$cHttpdStart" == "1" ] \
			&& [ "$cInitialize" == "1" ];then
	echo "unxsMail has been installed, intialized and httpd has been started.";	
	echo "You can proceed to login to your unxsMail interfaces with your browser.";	
else 
		echo "It appears that one or more manual operations may be needed to finish";
		echo "your unxsMail installation.";
fi
%clean

%files
%doc INSTALL LICENSE
/var/www/unxs/cgi-bin/unxsMail.cgi
/var/www/unxs/cgi-bin/unxsMailUser.cgi
/usr/local/share/unxsMail/

%changelog
* Tue Jul 14 2009 - Hugo Urquiza <support2@unixservice.com>
- Major .spec file update.
* Thu Jun 25 2009 - Hugo Urquiza <support2@unixservice.com>
- Minor data file update
* Wed Jun 24 2009 - Hugo Urquiza <support2@unixservice.com>
- New unxsMailUser interface, spec file fixes
* Thu May 28 2009 - Hugo Urquiza <support2@unixservice.com>
- Initial RPM release


