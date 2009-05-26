Summary: Manage ISP customers, resellers and their resources. Centralize resource and product usage.
Name: unxsisp
Version: 1.0
Release: 1
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsisp-1.0.tar.gz
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

%clean

%files
%doc INSTALL LICENSE
/var/www/unxs/cgi-bin/unxsISP.cgi
/var/www/unxs/cgi-bin/ispAdmin.cgi

%changelog
* Tue May 26 2009 - Hugo Urquiza <support2@unixservice.com>
- Initial RPM release


