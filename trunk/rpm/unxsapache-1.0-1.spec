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
make install

%clean

%files
%doc INSTALL LICENSE
/var/www/cgi-bin/unxsApache.cgi



