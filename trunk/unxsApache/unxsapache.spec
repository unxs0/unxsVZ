Summary: Webfarm manager (this is the technical back-office interface.) Uses flexible configuration templates sets and types that can be crafted for almost any scenario and any text file configured httpd server.
Name: unxsapache
Version: 1.0
Release: 1
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsapache-1.0.tar.gz
URL: http://openisp.net/openisp/unxsVZ
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: mysql >= 5.0.45, httpd, mod_ssl

%description
Webfarm manager (this is the technical back-office interface.) Uses flexible configuration templates sets and types that can be crafted for almost any scenario and any text file configured httpd server. Will manage from a central location n websites on m servers. Supports extra httpd daemon on same server for better SSL performance.
Will soon support replicated webfarm servers and end user and reseller template based dynamic interfaces for 24x7 self-help and self-up-sell.
unxsApache manages this cluster of webfarm servers (or a single simple server) via an asynchronous job queue.
Will soon support deployment check jobs for quick dashboard status/health overview from external deployment/changes confirmation.

%prep
%setup

%build
make
cd $RPM_BUILD_DIR

%install
export CGIDIR=/var/www/cgi-bin/
make install
mkdir -p /var/www/html/images/
cp ./images/*.gif /var/www/html/images/
mkdir /var/local/apache
mkdir /var/local/apache/bin
mkdir /var/local/apache/cgi-bin
mkdir /var/local/apache/conf
mkdir /var/local/apache/etc
mkdir /var/local/apache/logs
mkdir /var/local/apache/newwebsite
ln -s /usr/lib/httpd/modules/ /var/local/apache/modules
mkdir /var/local/apache_ssl
mkdir /var/local/apache_ssl/bin
mkdir /var/local/apache_ssl/cgi-bin
mkdir /var/local/apache_ssl/conf
mkdir /var/local/apache_ssl/etc
mkdir /var/local/apache_ssl/htdocs
mkdir /var/local/apache_ssl/logs
ln -s /usr/lib/httpd/modules/ /var/local/apache_ssl/modules
cp newwebsite.tar /var/local/apache/newwebsite/

%clean

%files
%doc INSTALL LICENSE
/var/www/cgi-bin/unxsApache.cgi
/var/www/html/images/calendar.gif
/var/www/html/images/hairline.gif
/var/www/html/images/left.gif
/var/www/html/images/left_on.gif
/var/www/html/images/right.gif
/var/www/html/images/right_last.gif
/var/www/html/images/right_on.gif
/var/local/apache
/var/local/apache/bin
/var/local/apache/cgi-bin
/var/local/apache/conf
/var/local/apache/etc
/var/local/apache/logs
/var/local/apache/newwebsite
/var/local/apache_ssl
/var/local/apache_ssl/bin
/var/local/apache_ssl/cgi-bin
/var/local/apache_ssl/conf
/var/local/apache_ssl/etc
/var/local/apache_ssl/htdocs
/var/local/apache_ssl/logs
/var/local/apache_ssl/modules
/var/local/apache/modules

%changelog
* Mon Apr 11 2009 - Hugo Urquiza <support2@unixservice.com>
- Initial RPM release


