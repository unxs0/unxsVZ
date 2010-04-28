Summary: DNS BIND 9 telco quality manager with admin and end-user web interfaces. Integrated rrdtool graphics.
Name: unxsbind
Version: 3.0
Release: 1
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsbind-3.0.tar.gz
URL: http://openisp.net/openisp/unxsBind
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: unxsadmin >= 1.2 , mysql-server >= 5.0.45 , bind >= 9.3.4 , bind-utils >= 9.3.4-10 , rrdtool , chkconfig, unxstemplatelib >= 1.0 , unxscidrlib >= 1.0

%description
unxsBind iDNS provides a SaaS DNS BIND9 manager.

Main features supported:

 1-.	Manages unlimited DNS data, for an unlimited number of NS sets, for an unlimited number of DNS servers.
 2-.	Unlimited BIND views (with no BIND master/slave xfer issues or extra IPs required.)
 3-.	Database can fail and DNS services continue.
 4-.	Supports hidden masters, external masters, all masters clusters, forward zones, secondary only zones.
 5-.	Engineering and development friendly web interface backend (visible schema and variables.)
 6-.	Organization/Company contact web interfaces included. Allows non skilled zone owners to modify RRs.
 7-.	User friendly admin web interface included.
 8-.	Bulk zone and resource record operations.
 9-.	Traffic monitoring per zone or per NS set (per NS or aggregated across cluster) 
	with rrdtool graphics.
 10-. 	Wizard for CIDR based downstream delegation of arpa zones.
 11-.	Uses company-contact-role model for allowing management partitioning. Perfect for DNS as a
	service (SaaS/ASP) deployments. The root ASP manages the infrastructure, clients that are 
	companies, NGOs or other organizations have contacts (staff) that can be of diverse
	permission levels for operating on their companies zones.
 12-.	Optional automatic creation of rev dns PTR records.
 13-.	Support for upstream delegated sub class C delegated rev dns zones.
 14-.	Migration and import tools.
 15-.	Support for large SPF/DK/DKIM and other TXT like RRs.
 16-.	Support for SRV and NAPTR RRs.
 17-.	Support for IPv6 AAAA and rev dns for IPv6.
 18-.	Comprehensive DNSSEC support (including key management) will be available as soon as BIND and
	at least three TLD deployments prove stable.

%prep
%setup -q

%build
make

%pre 
if [ "$1" = "1" ]; then
	echo "pre: Initial install";
elif [ "$1" = "2" ]; then
	echo "pre: Update";
fi


%install
make install
#mkdir section
mkdir -p /usr/local/idns
mkdir -p /usr/local/share/iDNS/data
mkdir -p /usr/local/share/iDNS/setup9
mkdir -p /usr/local/share/iDNS/admin/templates
mkdir -p /usr/local/share/iDNS/org/templates
mkdir -p /usr/local/share/iDNS/vorg/templates
mkdir -p /var/log/named
#cp files section
cp -u images/* /var/www/unxs/html/images/
cp -u interfaces/admin/templates/images/* /var/www/unxs/html/images/
cp -u interfaces/admin/templates/css/* /var/www/unxs/html/css/
cp `find ./interfaces/admin/templates/ -type f -print` /usr/local/share/iDNS/admin/templates/
cp `find ./interfaces/org/templates/ -type f -print` /usr/local/share/iDNS/org/templates/
cp `find ./interfaces/vorg/templates/ -type f -print` /usr/local/share/iDNS/vorg/templates/
cp data/*.txt /usr/local/share/iDNS/data/
cp data/*.sql /usr/local/share/iDNS/data/
chown -R mysql:mysql /usr/local/share/iDNS/data
cp setup9/rndc.key /etc/unxsbind-rndc.key
cp setup9/rndc.conf /etc/unxsbind-rndc.conf
cp setup9/unxsbind.init /etc/init.d/unxsbind
chmod 755 /etc/init.d/unxsbind
cp setup9/* /usr/local/share/iDNS/setup9/
cp -u setup9/DejaVuSansMono-Roman.ttf /usr/share/fonts/
cp agents/mysqlcluster/mysqlcluster.sh /usr/sbin/
/usr/bin/dig @e.root-servers.net . ns > /usr/local/share/iDNS/setup9/root.cache
#permissions section
chmod 755 /etc/init.d/unxsbind
#make section
cd interfaces/admin
make install
cd ../org
make install
cd ../vorg
make install
cd ../thit
cp bind9-genstats.sh /usr/sbin/bind9-genstats.sh
chmod 500 /usr/sbin/bind9-genstats.sh
make install
#things we can do with no data loaded
export ISMROOT=/usr/local/share
/var/www/unxs/cgi-bin/iDNS.cgi installbind 0.0.0.0
chmod -R og+x /usr/local/idns
chmod 644 /usr/local/idns/named.conf
chown -R named:named /usr/local/idns
cd $RPM_BUILD_DIR

%post
if [ "$1" = "1" ]; then
	echo "post: Initial install";
	chmod -R og+x /usr/local/idns
	chmod 644 /usr/local/idns/named.conf
	chown -R named:named /usr/local/idns
	if [ -x /sbin/chkconfig ];then
		if [ -x /etc/init.d/named ];then
			/sbin/chkconfig --level 3 named off;
		fi
		if [ -x /etc/init.d/unxsbind ];then
			/sbin/chkconfig --level 3 unxsbind on
			/etc/init.d/unxsbind restart > /dev/null 2>&1
			if [ $? == 0 ];then
				cUnxsBindStart="1";
			fi
		fi
		if [ -x /etc/init.d/httpd ];then
			/sbin/chkconfig --level 3 httpd on
			/etc/init.d/httpd restart > /dev/null 2>&1
			if [ $? == 0 ];then
				cHttpdStart="1";
			fi
		fi
		if [ -x /etc/init.d/mysqld ];then
			/sbin/chkconfig --level 3 mysqld on
			/etc/init.d/mysqld restart > /dev/null 2>&1
			if [ $? == 0 ];then
				cMySQLStart="1";
			fi
		fi
	fi
	#if mysqld has no root passwd and we started it then we will set it and finish the data initialize
	if [ -x /usr/bin/mysql ];then
		if [ "$cMySQLStart" == "1" ];then
			echo "quit" | /usr/bin/mysql  > /dev/null 2>&1;
			if [ $? == 0 ];then
				/usr/bin/mysqladmin -u root password 'ultrasecret' > /dev/null 2>&1;
				if [ $? == 0 ];then
					echo "mysqld root password set to 'ultrasecret' change ASAP!";
					export ISMROOT=/usr/local/share;
					/var/www/unxs/cgi-bin/iDNS.cgi Initialize ultrasecret > /dev/null 2>&1;
					if [ $? == 0 ];then
						cInitialize="1";
					fi
					/var/www/unxs/cgi-bin/iDNS.cgi allfiles master ns1.yourdomain.com 127.0.0.1 \								> /dev/null 2>&1;
					if [ $? == 0 ];then
						cAllfiles="1";
					fi
				fi
			fi
		fi
	fi
	#let installer know what was done.
	if [ "$cUnxsBindStart" == "1" ] && [ "$cHttpdStart" == "1" ] && [ "$cMySQLStart" == "1" ] \
				&& [ "$cInitialize" == "1" ];then
		echo "unxsBind has been installed, intialized and httpd and named have been started.";	
		echo "You can proceed to login to your unxsBind interfaces with your browser.";	
	else 
			echo "It appears that one or more manual operations may be needed to finish";
			echo "your unxsBind installation.";
		if [ "$cUnxsBindStart" != "1" ]; then
			echo "";
			echo "WARNING: Your unxsBind named was not started, run:";	
			echo "named-checkconf /usr/local/idns/named.conf";
			echo "And:";
			echo "rndc -c /etc/unxsbind.conf status";
			echo "Fix any problems, then run:";
			echo "/etc/init.d/unxsbind start";
		fi
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
			echo "WARNING: Your unxsBind database was not initialized, run:";
			echo "export ISMROOT=/usr/local/share";
			echo "/var/www/unxs/cgi-bin/iDNS.cgi Initialize <mysql-root-passwd>";	
			echo "Debug any problems, check via the mysql CLI, then if needed try again:";
			echo "/var/www/unxs/cgi-bin/iDNS.cgi Initialize <mysql-root-passwd>";	
		fi
	fi
	#cat unxsbind crontab into root crontab
	if [ -f /usr/local/share/iDNS/setup9/root-crontab ] && [ -d /var/spool/cron ] && [ -x /usr/sbin/tHitCollector ];then
		#initialize main stats rrd
		/usr/sbin/tHitCollector Initialize --cZone allzone.stats > /dev/null 2>&1;
		#new version of rrdtool needs fontconfig font, it was installed
		#but we need to load into cache
		if [ -x /usr/bin/fc-cache ];then
			/usr/bin/fc-cache > /dev/null 2>&1;
		fi
		#do not add again
		grep "iDNS" /var/spool/cron/root > /dev/null 2>&1
		if [ $? != 0 ];then
			cat /usr/local/share/iDNS/setup9/root-crontab >> /var/spool/cron/root;
		fi
		#setup main stats graph
		if [ -f /var/www/unxs/html/images/allzone.stats.png ] && [ -d /var/log/named ];then
			rm /var/www/unxs/html/images/allzone.stats.png;
			ln -s /var/log/named/allzone.stats.png /var/www/unxs/html/images/allzone.stats.png;
		fi
	fi
elif [ "$1" = "2" ]; then
	echo "post: Update";
	#update schema
	if [ -x /var/www/unxs/cgi-bin/iDNS.cgi ];then
		/var/www/unxs/cgi-bin/iDNS.cgi UpdateSchema > /dev/null 2>&1;
		if [ $? == 0 ];then
			cUpdateSchema="1";
		fi
	fi
	#update tables (fixed type and template tables)
	if [ -x /var/www/unxs/cgi-bin/iDNS.cgi ];then
		/var/www/unxs/cgi-bin/iDNS.cgi UpdateTables > /dev/null 2>&1;
		if [ $? == 0 ];then
			cUpdateTables="1";
		fi
	fi
	#let installer know what was done.
	if [ "$cUpdateSchema" == "1" ] && [ "$cUpdateTables" == "1" ];then
		echo "unxsBind progams have been updated, your MySQL schema and fixed table contents";	
		echo " have also been upgraded. Existing templates have been saved.";	
	else 
			echo "It appears that one or more manual operations may be needed to finish";
			echo "your unxsBind upgrade.";
		if [ "$cUpdateSchema" != "1" ]; then
			echo "";
			echo "WARNING: Your unxsBind schema was not updated!";	
		fi
		if [ "$cUpdateTables" != "1" ]; then
			echo "";
			echo "WARNING: Your unxsBind fixed tables and templates have not been updated!";	
		fi
	fi
fi

%preun
if [ "$1" = "0" ]; then
	echo "preun: Uninstall";
elif [ "$1" = "1" ]; then
	echo "preun: Update";
fi

%postun
if [ "$1" = "0" ]; then
	echo "postun: Uninstall";
elif [ "$1" = "1" ]; then
	echo "postun: Update";
fi

%clean

%files
%doc LICENSE INSTALL
/usr/local/share/iDNS
%config(noreplace) /usr/local/idns/named.conf
%config(noreplace) /etc/unxsbind-rndc.key
%config(noreplace) /etc/unxsbind-rndc.conf
/etc/init.d/unxsbind
#/usr/local/idns/named.d
#/usr/local/idns/named.d/master
%config(noreplace) /usr/local/idns/named.d/master.zones
/usr/local/idns/named.d/master/0
/usr/local/idns/named.d/master/1
%config(noreplace) /usr/local/idns/named.d/master/127.0.0
%dir /usr/local/idns/named.d/master/2
%dir /usr/local/idns/named.d/master/3
%dir /usr/local/idns/named.d/master/4
%dir /usr/local/idns/named.d/master/5
%dir /usr/local/idns/named.d/master/6
%dir /usr/local/idns/named.d/master/7
%dir /usr/local/idns/named.d/master/8
%dir /usr/local/idns/named.d/master/9
%dir /usr/local/idns/named.d/master/a
%dir /usr/local/idns/named.d/master/b
%dir /usr/local/idns/named.d/master/c
%dir /usr/local/idns/named.d/master/d
%dir /usr/local/idns/named.d/master/e
%dir /usr/local/idns/named.d/master/f
%dir /usr/local/idns/named.d/master/g
%dir /usr/local/idns/named.d/master/h
%dir /usr/local/idns/named.d/master/i
%dir /usr/local/idns/named.d/master/j
%dir /usr/local/idns/named.d/master/k
%dir /usr/local/idns/named.d/master/l
%config(noreplace) /usr/local/idns/named.d/master/localhost
%dir /usr/local/idns/named.d/master/m
%dir /usr/local/idns/named.d/master/n
%dir /usr/local/idns/named.d/master/o
%dir /usr/local/idns/named.d/master/p
%dir /usr/local/idns/named.d/master/q
%dir /usr/local/idns/named.d/master/r
%dir /usr/local/idns/named.d/master/s
%dir /usr/local/idns/named.d/master/t
%dir /usr/local/idns/named.d/master/u
%dir /usr/local/idns/named.d/master/v
%dir /usr/local/idns/named.d/master/w
%dir /usr/local/idns/named.d/master/x
%dir /usr/local/idns/named.d/master/y
%dir /usr/local/idns/named.d/master/z
%config(noreplace) /usr/local/idns/named.d/root.cache
#/usr/local/idns/named.d/slave
%config(noreplace) /usr/local/idns/named.d/slave.zones
%dir /usr/local/idns/named.d/slave/0
%dir /usr/local/idns/named.d/slave/1
%dir /usr/local/idns/named.d/slave/2
%dir /usr/local/idns/named.d/slave/3
%dir /usr/local/idns/named.d/slave/4
%dir /usr/local/idns/named.d/slave/5
%dir /usr/local/idns/named.d/slave/6
%dir /usr/local/idns/named.d/slave/7
%dir /usr/local/idns/named.d/slave/8
%dir /usr/local/idns/named.d/slave/9
%dir /usr/local/idns/named.d/slave/a
%dir /usr/local/idns/named.d/slave/b
%dir /usr/local/idns/named.d/slave/c
%dir /usr/local/idns/named.d/slave/d
%dir /usr/local/idns/named.d/slave/e
%dir /usr/local/idns/named.d/slave/f
%dir /usr/local/idns/named.d/slave/g
%dir /usr/local/idns/named.d/slave/h
%dir /usr/local/idns/named.d/slave/i
%dir /usr/local/idns/named.d/slave/j
%dir /usr/local/idns/named.d/slave/k
%dir /usr/local/idns/named.d/slave/l
%dir /usr/local/idns/named.d/slave/m
%dir /usr/local/idns/named.d/slave/n
%dir /usr/local/idns/named.d/slave/o
%dir /usr/local/idns/named.d/slave/p
%dir /usr/local/idns/named.d/slave/q
%dir /usr/local/idns/named.d/slave/r
%dir /usr/local/idns/named.d/slave/s
%dir /usr/local/idns/named.d/slave/t
%dir /usr/local/idns/named.d/slave/u
%dir /usr/local/idns/named.d/slave/v
%dir /usr/local/idns/named.d/slave/w
%dir /usr/local/idns/named.d/slave/x
%dir /usr/local/idns/named.d/slave/y
%dir /usr/local/idns/named.d/slave/z
/var/www/unxs/cgi-bin/iDNS.cgi
/var/www/unxs/cgi-bin/idnsAdmin.cgi
/var/www/unxs/cgi-bin/idnsOrg.cgi
/var/www/unxs/cgi-bin/vdnsOrg.cgi
/usr/sbin/tHitCollector
%config(noreplace) /usr/sbin/bind9-genstats.sh
/var/www/unxs/html/images/allzone.stats.png
/var/www/unxs/html/images/green.gif
/var/www/unxs/html/images/null.gif
/var/www/unxs/html/images/red.gif
/var/www/unxs/html/images/unxsbind.jpg
/var/www/unxs/html/images/yellow.gif
%dir /var/log/named
/usr/share/fonts/DejaVuSansMono-Roman.ttf
%config(noreplace) /usr/sbin/mysqlcluster.sh

%changelog
* Tue Apr 27 2010 Gary Wallis <support@unixservice.com>
- Fixed upgrade and requirements. Using new unxstemplate lib for interfaces. Corrected multiple DB server support. Corrected version number change. Added support for update and uninstall.
* Wed Mar 17 2010 Hugo Urquiza <support2@unixservice.com>
- New rpm release, added support for AAAA and NAPTR RRs, major version number change.
* Mon Aug 02 2009 Hugo Urquiza <support2@unixservice.com>
- Fixed directories permission and ownership, and init.d script minor update.
* Mon Jul 13 2009 Gary Wallis <support@unixservice.com>
- Adding allfiles master post install command
* Sun Jul 12 2009 Gary Wallis <support@unixservice.com>
- Fixed man install issues while adding install post script
* Sat Jul 11 2009 Gary Wallis <support@unixservice.com>
- Fixed conflict with BIND regarding /etc/rndc.key and added post install and rrdtool issue fixes
* Sat Jul 11 2009 Gary Wallis <support@unixservice.com>
- Added missing rndc.key and /etc/init.d/unxsbind among other related issues.
* Fri Jul 10 2009 Hugo Urquiza <support2@unixservice.com>
- More minor idnsOrg updates
* Fri Jul 10 2009 Gary Wallis <support@unixservice.com>
- Added config file directives for correct update behavior. This is under alpha checking for initial install and update.
* Thu Jul 09 2009 Hugo Urquiza <support2@unixservice.com>
- Added 'Delegation Tools' feature to idnsOrg. Online RR check at idnsOrg minor updates. Backend list filter queries updated.
* Thu Jul 09 2009 Gary Wallis <support@unixservice.com>
- iDNS.cgi CLI ImportZone command and tZoneImport and tResourceImport backend bug fixes part-2.
* Thu Jul 09 2009 Gary Wallis <support@unixservice.com>
- iDNS.cgi CLI ImportZone command and tZoneImport and tResourceImport backend bug fixes.
* Thu Jul 2 2009 Hugo Urquiza <support2@unixservice.com>
- Minor fixes.
* Fri Jun 26 2009 Hugo Urquiza <support2@unixservice.com>
- Fixed idnsOrg bulk importer bug.
* Tue Jun 16 2009 Hugo Urquiza <support2@unixservice.com>
- Backend 'Delegation Tools' update.
* Thu Jun 11 2009 Hugo Urquiza <support2@unixservice.com>
- Minor idnsAdmin and idnsOrg updates.
* Tue Jun 02 2009 Hugo Urquiza <support2@unixservice.com>
- Several fixes and updates
* Thu May 28 2009 Hugo Urquiza <support2@unixservice.com>
- Minor code fixes, added mysqlcluster.sh
* Mon May 18 2009 Hugo Urquiza <support2@unixservice.com>
- Added rrdtool as required package. Updated file list and mkdir section.
* Thu May 14 2009 Hugo Urquiza <support2@unixservice.com>
- Minor fixes and updates.
* Thu May 14 2009 Gary Wallis <support@unixservice.com> 
- Initial rpm release

