Summary: unxsadmin provides the http shared content and conf for all unxsVZ web admins
Name: unxsadmin
Version: 1.0
Release: 1
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsadmin-1.0.tar.gz
URL: http://openisp.net/openisp/unxsAdmin
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: httpd, mod_ssl

%description
unxsadmin provides the http shared content and conf for all unxsVZ web admins.
It provides the dir layout in /var/www/unxs and all the shared css, js and image
content needed by unxsVZ family of web administration interfaces like
unxsMail, unxsApache, unxsVZ, unxsBind and unxsISP.

%prep
%setup

%build

%install
mkdir -p /var/www/unxs/cgi-bin
mkdir -p /var/www/unxs/logs
mkdir -p /var/www/unxs/html/images
mkdir -p /var/www/unxs/html/js
mkdir -p /var/www/unxs/html/css
cd images
cp *.gif /var/www/unxs/html/images/
cd ../js
cp *.js /var/www/unxs/html/js
cd ../css
cp *.css /var/www/unxs/html/css
cd ..
cp unxs.conf /etc/httpd/conf.d/unxs.conf
/etc/init.d/httpd restart
cd $RPM_BUILD_DIR

%clean

%files
%doc README
/var/www/unxs
/etc/httpd/conf.d/unxs.conf

%changelog
* Tue Apr 11 2009 Gary Wallis <support@unixservice.com> 
- Initial RPM release

