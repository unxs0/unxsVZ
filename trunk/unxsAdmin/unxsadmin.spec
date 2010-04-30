Summary: unxsadmin provides the http shared content and httpd conf.d file for all unxsVZ web admins
Name: unxsadmin
Version: 1.6
Release: 1
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsadmin-1.6.tar.gz
URL: http://unixservice.com
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: httpd, mod_ssl, rrdtool

%description
unxsadmin provides the http shared content and conf for all unxsVZ web admins.
It provides the dir layout in /var/www/unxs and all the shared css, js and image
content needed by unxsVZ family of web administration interfaces like
unxsMail, unxsApache, unxsVZ, unxsBind and unxsISP.
It also provides some common binary utilities like lastmonth.

%prep
%setup

%build
make

%install
#lastmonth binary
make install
mkdir -p /var/www/unxs/cgi-bin
mkdir -p /var/www/unxs/logs
mkdir -p /var/www/unxs/html/images
mkdir -p /var/www/unxs/html/js
mkdir -p /var/www/unxs/html/css
cd images
cp *.gif /var/www/unxs/html/images/
cp favicon.ico /var/www/unxs/html/
cd ../js
cp *.js /var/www/unxs/html/js
cd ../css
cp *.css /var/www/unxs/html/css
cd ..
cp unxs.conf /etc/httpd/conf.d/unxs.conf
#force local build test of unxs.conf
/etc/init.d/httpd restart
cd $RPM_BUILD_DIR

%clean

%files
%doc README
%dir /var/www/unxs/logs/
%dir /var/www/unxs/cgi-bin/
%dir /var/www/unxs/html/images/
%dir /var/www/unxs/html/js/
%dir /var/www/unxs/html/css/
%config(noreplace) /etc/httpd/conf.d/unxs.conf
/usr/bin/lastmonth
/var/www/unxs/html/favicon.ico
/var/www/unxs/html/images/calendar.gif
/var/www/unxs/html/images/calendar_mo.gif*
/var/www/unxs/html/images/hairline.gif
/var/www/unxs/html/images/left.gif
/var/www/unxs/html/images/left_on.gif
/var/www/unxs/html/images/right.gif
/var/www/unxs/html/images/right_last.gif
/var/www/unxs/html/images/right_on.gif
/var/www/unxs/html/images/topleft.gif
/var/www/unxs/html/images/topright.gif
/var/www/unxs/html/images/unxslogo.gif
/var/www/unxs/html/css/calendar-blue.css
/var/www/unxs/html/js/calendar-en.js
/var/www/unxs/html/js/calendar-setup.js
/var/www/unxs/html/js/calendar.js



%changelog
* Fri Apr 30 2010 Gary Wallis <support@unixservice.com>
- Added the Unxs X favicon.ico.
* Fri Sep 22 2009 Gary Wallis <support@unixservice.com>
- Clean up and modernization, added an img for unxsVZ.
* Fri May 15 2009 Hugo Urquiza <support2@unixservice.com>
- RPM rebuild removing extra files
* Wed Apr 11 2009 Gary Wallis <support@unixservice.com> 
- Fixed unxs.conf 

