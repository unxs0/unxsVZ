Summary: unxsadmin provides the http shared content and httpd conf.d file for all unxsVZ web admins
Name: unxsadmin
Version: 1.4
Release: 2
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsadmin-1.4.tar.gz
URL: http://openisp.net/unxsAdmin
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: httpd, mod_ssl, rrdtool

%description
unxsadmin provides the http shared content and conf for all unxsVZ web admins.
It provides the dir layout in /var/www/unxs and all the shared css, js and image
content needed by unxsVZ family of ISP web administration interfaces like
unxsMail, unxsApache, unxsVZ, unxsBind, unxsRadius and unxsISP.
It also provides some common binary utilities like lastmonth and
rrdtool that are used by more than one unxsVZ included unxsISP software
module.

%prep
%setup

%build
make

%install
make install
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
/usr/bin/lastmonth

%changelog
* Tue May 19 2009 Gary Wallis <support2@unixservice.com>
- Had to clean out /var/www/unxs
* Tue May 19 2009 Gary Wallis <support2@unixservice.com>
- New version.

