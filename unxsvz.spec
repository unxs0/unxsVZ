Summary: unxsVZ (CentOS5 yum version) is a multiple datacenter and hardware node, OpenVZ manager with autonomics.
Name: unxsvz
Version: 2.9
Release: 1
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsvz-2.9.tar.gz
URL: http://openisp.net/openisp/unxsVZ
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: mysql >= 5.0.45, mysql-server >= 5.0.45, httpd, mod_ssl, ovzkernel, vzctl, cstream, unxsadmin, rrdtool, vzdump

%description
unxsVZ is a multiple datacenter, multiple hardware node, OpenVZ
container manager with autonomics and advanced migration.

unxsVZ supports and will support more and more HA and load balancing
functions across your private internet cloud.

unxsVZ is already in commercial production and has proven itself very stable
and extensible.

unxVZ solves numerous internet infrastructure provisioning problems including
cost, maintenance and monitoring. 


%prep
%setup

%build
make
cd agents/ubc
make
cd $RPM_BUILD_DIR

%install
#mkdir section
mkdir -p /var/www/unxs/html/traffic/
mkdir -p /usr/local/share/unxsVZ/data/
mkdir -p /usr/local/share/unxsVZ/setup/
#make section
make install
cd agents/ubc
make install
#cp files section
cd ../../data
cp *.txt /usr/local/share/unxsVZ/data/
cd ../tools/rrdtool/
install README /usr/local/share/unxsVZ/README.rrdtool
install -m 500 vz_traffic_log.sh /usr/local/sbin/vz_traffic_log.sh
install -m 500 node_traffic_log.sh /usr/local/sbin/node_traffic_log.sh
install -m 500 datacenter_traffic_log.sh /usr/local/sbin/datacenter_traffic_log.sh
cp -u DejaVuSansMono-Roman.ttf /usr/share/fonts/
cd ../datacenter/
install -m 500 allnodescp.sh /usr/sbin/allnodescp.sh
install -m 500 allnodecmd.sh /usr/sbin/allnodecmd.sh
cd ../cron/
cp root-crontab /usr/local/share/unxsVZ/setup/root-crontab
cd ../openvz/
cd $RPM_BUILD_DIR

%post
#todo this can be improved upon for version comparison. Also mainfunc.h UpdateSchema
#	can be made smarter.
cUpdate="0";
grep "unxsVZ" /var/spool/cron/root > /dev/null 2>&1;
if [ $? == 0 ];then
#	if [ -x /var/www/unxs/cgi-bin/unxsVZ.cgi ]; then
		cUpdate="1";
#	fi
fi

if [ -x /sbin/chkconfig ] && [ "$cUpdate" == "0" ];then
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
#
if [ -x /var/www/unxs/cgi-bin/unxsVZ.cgi ] && [  -x /bin/rpm ] && [ "$cUpdate" == "1" ];then
	/var/www/unxs/cgi-bin/unxsVZ.cgi UpdateSchema > /dev/null 2>&1
fi
#if mysqld has no root passwd and we started it then we will set it and finish the data initialize
if [ -x /usr/bin/mysql ];then
	if [ "$cMySQLStart" == "1" ] && [ "$cUpdate" == "0" ];then
		echo "quit" | /usr/bin/mysql  > /dev/null 2>&1
		if [ $? == 0 ];then
			/usr/bin/mysqladmin -u root password 'ultrasecret' > /dev/null 2>&1
			if [ $? == 0 ];then
				echo "mysqld root password set to 'ultrasecret' change ASAP!"
				export ISMROOT=/usr/local/share
				/var/www/unxs/cgi-bin/unxsVZ.cgi Initialize ultrasecret > /dev/null 2>&1
				if [ $? == 0 ];then
					cInitialize="1"
				fi
			fi
		fi
	fi
fi
#let installer know what was done.
if [ "$cHttpdStart" == "1" ] && [ "$cMySQLStart" == "1" ] \
			&& [ "$cInitialize" == "1" ] && [ "$cUpdate" == "0" ];then
	echo "unxsVZ has been installed, initialized and, httpd and mysqld have been started.";	
	echo "You can proceed to login to your unxsVZ interfaces with your browser.";	
elif [ "$cUpdate" == "1" ]; then
	echo "unxsVZ has been upgraded.";	
else 
	echo "It appears that one or more manual operations may be needed to finish";
	echo "your unxsVZ installation.";
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
		echo "WARNING: Your unxsVZ database was not initialized, run:";
		echo "export ISMROOT=/usr/local/share";
		echo "/var/www/unxs/cgi-bin/unxsVZ.cgi Initialize <mysql-root-passwd>";	
		echo "Debug any problems, check via the mysql CLI, then if needed try again.";
	fi
fi
#cat unxsVZ crontab into root crontab
if [ -f /usr/local/share/unxsVZ/setup/root-crontab ] && [ -d /var/spool/cron ];then
	#do not add again
	grep "unxsVZ" /var/spool/cron/root > /dev/null 2>&1
	if [ $? != 0 ] && [ "$cUpdate" == "0" ];then
		cat /usr/local/share/unxsVZ/setup/root-crontab >> /var/spool/cron/root;
	fi
fi
#new version of rrdtool needs fontconfig ttf font, it was installed
#but we need to load into cache
if [ -x /usr/bin/fc-cache ];then
	/usr/bin/fc-cache > /dev/null 2>&1
fi

%clean

%files
%doc INSTALL LICENSE
/usr/sbin/unxsUBC
/var/www/unxs/cgi-bin/unxsVZ.cgi
%dir /usr/local/share/unxsVZ
%config(noreplace) /usr/local/sbin/vz_traffic_log.sh
%config(noreplace) /usr/local/sbin/node_traffic_log.sh
%config(noreplace) /usr/local/sbin/datacenter_traffic_log.sh
%config(noreplace) /usr/sbin/allnodescp.sh
%config(noreplace) /usr/sbin/allnodecmd.sh
%dir /var/www/unxs/html/traffic/
%dir /usr/local/share/unxsVZ/data/
/usr/local/share/unxsVZ/data/tAuthorize.txt
/usr/local/share/unxsVZ/data/tClient.txt
/usr/local/share/unxsVZ/data/tConfig.txt
/usr/local/share/unxsVZ/data/tConfiguration.txt
/usr/local/share/unxsVZ/data/tGlossary.txt
/usr/local/share/unxsVZ/data/tGroupType.txt
/usr/local/share/unxsVZ/data/tJobStatus.txt
/usr/local/share/unxsVZ/data/tLogType.txt
/usr/local/share/unxsVZ/data/tNameserver.txt
/usr/local/share/unxsVZ/data/tOSTemplate.txt
/usr/local/share/unxsVZ/data/tSearchdomain.txt
/usr/local/share/unxsVZ/data/tStatus.txt
/usr/local/share/unxsVZ/data/tTemplate.txt
/usr/local/share/unxsVZ/data/tTemplateSet.txt
/usr/local/share/unxsVZ/data/tTemplateType.txt
/usr/local/share/unxsVZ/data/tType.txt
%dir /usr/local/share/unxsVZ/setup/
/usr/local/share/unxsVZ/setup/root-crontab
/usr/share/fonts/DejaVuSansMono-Roman.ttf


%changelog
* Thu Aug 27 2009 Gary Wallis <supportgrp@unixservice.com>
- Bug fix and vzdump vzmigrate enhancements
* Tue Aug 26 2009 Hugo Urquiza <support2@unixservice.com>
- Minor spec file updates.
* Mon Aug 24 2009 Gary Wallis <supportgrp@unixservice.com>
- Major spec file update, based on unxsbind.spec file
* Fri Jul 24 2009 Hugo Urquiza <support2@unixservice.com>
- Updated spec file for correct rpm package building
* Fri Jul 24 2009 Hugo Urquiza <support2@unixservice.com>
- Updated spec file for correct rpm package building
* Mon Apr 11 2009 Gary Wallis <support@unixservice.com> 
- Fixed perm install error for vz_traffic_log.sh
* Mon Apr 11 2009 Gary Wallis <support@unixservice.com> 
- Added rrdtool traffic graph script install
* Sun Apr 10 2009 Dilva Garmendia <support@unixservice.com> 
- Initial RPM release

