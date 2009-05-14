Summary: DNS BIND 9 telco quality manager with quality admin and end-user web interfaces. Also rrdtool graphics.
Name: unxsbind
Version: 1.0
Release: 1
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsbind-1.0.tar.gz
URL: http://openisp.net/openisp/unxsBind
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: unxsadmin, mysql-server

%description
unxsBind provides a professional DNS BIND 9 manager. For 1 to 1000's of NSs.

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
mkdir -p /usr/local/share/unxsBind/data
mkdir -p /usr/local/share/unxsBind/admin/templates
mkdir -p /usr/local/share/unxsBind/org/templates
#cp files section
cp -u images/* /var/www/unxs/html/images/
cp -u interfaces/admin/templates/images/* /var/www/unxs/html/images/
cp -u interfaces/admin/templates/css/* /var/www/unxs/html/css/
cp -u interfaces/org/templates/images/* /var/www/unxs/html/images/
cp -u interfaces/org/templates/css/* /var/www/unxs/html/css/
cp `find ./interfaces/admin/templates/ -type f -print` /usr/local/share/unxsBind/admin/templates/
cp `find ./interfaces/org/templates/ -type f -print` /usr/local/share/unxsBind/org/templates/
cp data/*.txt /usr/local/share/unxsBind/data/
#make section
cd interfaces/admin
make install
cd ../org
make install
cd ../thit
make install
cd ../errorlog
make install
cd $RPM_BUILD_DIR

%clean

%files
%doc LICENSE INSTALL
/usr/local/share/unxsBind
/usr/local/idns
/var/www/unxs/cgi-bin/iDNS.cgi
/var/www/unxs/cgi-bin/idnsAdmin.cgi
/var/www/unxs/cgi-bin/idnsOrg.cgi
/usr/sbin/tHitCollector
/usr/sbin/idns-logerror

%changelog
* Wed Apr 11 2009 Gary Wallis <support@unixservice.com> 
- Fixed unxs.conf 

