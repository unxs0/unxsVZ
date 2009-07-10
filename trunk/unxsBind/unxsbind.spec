Summary: DNS BIND 9 telco quality manager with quality admin and end-user web interfaces. Also rrdtool graphics.
Name: unxsbind
Version: 1.19
Release: 1
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsbind-1.19.tar.gz
URL: http://openisp.net/openisp/unxsBind
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: unxsadmin >= 1.2 , mysql-server >= 5.0.45 , bind >= 9.3.4 , bind-utils, rrdtool

%description
unxsBind iDNS provides a professional DNS BIND 9 manager. For 1 to 1000's of NSs.

Main features supported:

 1-.  Multiple disjoint NS sets that can share same NS cluster nodes.
 2-.  Unlimited split horizon views (with no BIND xfer issues.)
 3-.  Bulk zone and resource record operations.
 4-.  Traffic monitoring per zone or per NS set (per NS or aggregated across cluster) 
	with rrdtool graphics.
 5-.  Wizards for CIDR based downstream delegation of arpa zones.
 6-.  Hidden masters, external masters, all masters only clusters (no BIND xfer problems.)
 7-.  Secondary only zone management for customers running their own (hidden or public) masters.
 8-.  Engineering and development friendly tech web interface backend.
 9-.  Organization/Company contact web interface. Allows non skilled zone owners to modify RRs.
 10-. User friendly admin web interface for zone and customer, contact management authorized users.
 11-. Uses company-contact-role model for allowing management partitioning perfect for DNS as a
	service (ASP) deployments. The root ASP manages the infrastructure, clients that are 
	companies, NGOs or other organizations have contacts (staff) that can be of diverse
	permission levels for operating on their companies zones.
 12-. Optional automatic creation of rev dns PTR records.
 13-. Support for upstream delegated sub class C delegated rev dns zones.
 14-. Migration and import tools.
 15-. Database can fail and DNS services continue.
 16-. Support for large DKIM and other txt RRs.
 17-. DNSSEC and international char (IDNS) support available soon with just one "yum update unxsbind" 
	command.

%prep
%setup

%build
make

%install
make install
#mkdir section
mkdir -p /usr/local/idns
mkdir -p /usr/local/share/iDNS/data
mkdir -p /usr/local/share/iDNS/setup9
mkdir -p /usr/local/share/iDNS/admin/templates
mkdir -p /usr/local/share/iDNS/org/templates
mkdir -p /var/log/named
#cp files section
cp -u images/* /var/www/unxs/html/images/
cp -u interfaces/admin/templates/images/* /var/www/unxs/html/images/
cp -u interfaces/admin/templates/css/* /var/www/unxs/html/css/
cp `find ./interfaces/admin/templates/ -type f -print` /usr/local/share/iDNS/admin/templates/
cp `find ./interfaces/org/templates/ -type f -print` /usr/local/share/iDNS/org/templates/
cp data/*.txt /usr/local/share/iDNS/data/
chown -R mysql:mysql /usr/local/share/iDNS/data
cp setup9/rndc.conf /etc/rndc.conf
cp setup9/rndc.key /etc/rndc.key
cp setup9/* /usr/local/share/iDNS/setup9/
cp agents/mysqlcluster/mysqlcluster.sh /usr/sbin/
/usr/bin/dig @e.root-servers.net . ns > /usr/local/share/iDNS/setup9/root.cache
#make section
cd interfaces/admin
make install
cd ../org
make install
cd ../thit
cp bind9-genstats.sh /usr/sbin/bind9-genstats.sh
make install
cd ../errorlog
make install
#things we can do with no data loaded
export ISMROOT=/usr/local/share
/var/www/unxs/cgi-bin/iDNS.cgi installbind 127.0.0.1
chmod -R og+x /usr/local/idns
cd $RPM_BUILD_DIR

%clean

%files
%doc LICENSE INSTALL
/usr/local/share/iDNS
%config(noreplace) /usr/local/idns/named.conf
#/usr/local/idns/named.d
#/usr/local/idns/named.d/master
%config(noreplace) /usr/local/idns/named.d/master.zones
/usr/local/idns/named.d/master/0
/usr/local/idns/named.d/master/1
%config(noreplace) /usr/local/idns/named.d/master/127.0.0
/usr/local/idns/named.d/master/2
/usr/local/idns/named.d/master/3
/usr/local/idns/named.d/master/4
/usr/local/idns/named.d/master/5
/usr/local/idns/named.d/master/6
/usr/local/idns/named.d/master/7
/usr/local/idns/named.d/master/8
/usr/local/idns/named.d/master/9
/usr/local/idns/named.d/master/a
/usr/local/idns/named.d/master/b
/usr/local/idns/named.d/master/c
/usr/local/idns/named.d/master/d
/usr/local/idns/named.d/master/e
/usr/local/idns/named.d/master/f
/usr/local/idns/named.d/master/g
/usr/local/idns/named.d/master/h
/usr/local/idns/named.d/master/i
/usr/local/idns/named.d/master/j
/usr/local/idns/named.d/master/k
/usr/local/idns/named.d/master/l
%config(noreplace) /usr/local/idns/named.d/master/localhost
/usr/local/idns/named.d/master/m
/usr/local/idns/named.d/master/n
/usr/local/idns/named.d/master/o
/usr/local/idns/named.d/master/p
/usr/local/idns/named.d/master/q
/usr/local/idns/named.d/master/r
/usr/local/idns/named.d/master/s
/usr/local/idns/named.d/master/t
/usr/local/idns/named.d/master/u
/usr/local/idns/named.d/master/v
/usr/local/idns/named.d/master/w
/usr/local/idns/named.d/master/x
/usr/local/idns/named.d/master/y
/usr/local/idns/named.d/master/z
%config(noreplace) /usr/local/idns/named.d/root.cache
#/usr/local/idns/named.d/slave
%config(noreplace) /usr/local/idns/named.d/slave.zones
/usr/local/idns/named.d/slave/0
/usr/local/idns/named.d/slave/1
/usr/local/idns/named.d/slave/2
/usr/local/idns/named.d/slave/3
/usr/local/idns/named.d/slave/4
/usr/local/idns/named.d/slave/5
/usr/local/idns/named.d/slave/6
/usr/local/idns/named.d/slave/7
/usr/local/idns/named.d/slave/8
/usr/local/idns/named.d/slave/9
/usr/local/idns/named.d/slave/a
/usr/local/idns/named.d/slave/b
/usr/local/idns/named.d/slave/c
/usr/local/idns/named.d/slave/d
/usr/local/idns/named.d/slave/e
/usr/local/idns/named.d/slave/f
/usr/local/idns/named.d/slave/g
/usr/local/idns/named.d/slave/h
/usr/local/idns/named.d/slave/i
/usr/local/idns/named.d/slave/j
/usr/local/idns/named.d/slave/k
/usr/local/idns/named.d/slave/l
/usr/local/idns/named.d/slave/m
/usr/local/idns/named.d/slave/n
/usr/local/idns/named.d/slave/o
/usr/local/idns/named.d/slave/p
/usr/local/idns/named.d/slave/q
/usr/local/idns/named.d/slave/r
/usr/local/idns/named.d/slave/s
/usr/local/idns/named.d/slave/t
/usr/local/idns/named.d/slave/u
/usr/local/idns/named.d/slave/v
/usr/local/idns/named.d/slave/w
/usr/local/idns/named.d/slave/x
/usr/local/idns/named.d/slave/y
/usr/local/idns/named.d/slave/z
/var/www/unxs/cgi-bin/iDNS.cgi
/var/www/unxs/cgi-bin/idnsAdmin.cgi
/var/www/unxs/cgi-bin/idnsOrg.cgi
/usr/sbin/tHitCollector
%config(noreplace) /usr/sbin/bind9-genstats.sh
/usr/sbin/idns-logerror
/var/www/unxs/html/images/green.gif
/var/www/unxs/html/images/red.gif
/var/log/named
%config(noreplace) /usr/sbin/mysqlcluster.sh

%changelog
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

