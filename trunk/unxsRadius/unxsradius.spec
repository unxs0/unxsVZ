Summary: Mission critical RADIUS server central administration system. 
Name: unxsradius
Version: 1.0
Release: 1
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsradius-1.0.tar.gz
URL: http://openisp.net/openisp/unxsVZ
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: mysql-server >= 5.0.45, unxsadmin, freeradius

%description
Mission critical RADIUS server central administration system. With company-contact-role model and support for disjoint sets of RADIUS server configuration and users. Can be controlled via master ISP unxsISP/ispAdmin.

%prep
%setup

%build
make
cd unxsRadacct
make
cd $RPM_BUILD_DIR

%install
install -s unxsRadius.cgi /var/www/unxs/cgi-bin/unxsRadius.cgi
install -s unxsRadacct/unxsRadacct.cgi /var/www/unxs/cgi-bin/unxsRadacct.cgi
mkdir -p /usr/local/share/unxsRadius/data
mkdir -p /usr/local/share/unxsRadius/tools
mkdir -p /usr/local/share/unxsRadacct/data/
cp data/*.txt /usr/local/share/unxsRadius/data/
cp -r tools/* /usr/local/share/unxsRadius/tools
cp -r setupradius/ /usr/local/share/unxsRadius/
cp init.d/unxsradius.init /etc/init.d/
cp init.d/unxsradius.sysconfig /etc/sysconfig
cp unxsRadacct/data/*.txt /usr/local/share/unxsRadacct/data/

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
* Mon May 25 2009 - Hugo Urquiza <support2@unixservice.com>
- Initial RPM release


