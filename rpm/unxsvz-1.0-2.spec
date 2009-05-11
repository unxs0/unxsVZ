Summary: unxsVZ (CentOS5 yum version) is a multiple datacenter and hardware node, OpenVZ manager with autonomics.
Name: unxsvz
Version: 1.0
Release: 2
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsvz-1.0.tar.gz
URL: http://openisp.net/openisp/unxsVZ
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: mysql >= 5.0.45, httpd, mod_ssl, ovzkernel, vzctl

%description
unxsVZ is a multiple datacenter, multiple hardware node, OpenVZ
container manager with autonomics and advanced migration.

unxsVZ supports and will support more and more HA and load balancing
functions across your private internet cloud.

unxsVZ is an ambitious FOSS "yet another cloud infrastructure manager."

unxsVZ is already in commercial production and has proven itself very stable
and extensible.

unxVZ solves numerous internet infrastructure provisioning problems including
cost, maintenance and monitoring. 


%prep
%setup

%build
make
cd agents/ubc
make
cd $RPM_BUILD_DIR

%install
make install
cd agents/ubc
make install
cd ../../images
mkdir -p /var/www/html/images/
cp *.gif /var/www/html/images/
cd ../data
mkdir -p /usr/local/share/unxsVZ/data/
cp *.txt /usr/local/share/unxsVZ/data/
cd ../tools/rrdtool/
install -m 400 vz_traffic_log.sh /usr/local/sbin/vz_traffic_log.sh
mkdir -p /var/www/html/traffic/
cd $RPM_BUILD_DIR


%clean

%files
%doc INSTALL LICENSE
/usr/sbin/unxsUBC
/var/www/cgi-bin/unxsVZ.cgi
/var/www/html/images/calendar.gif
/var/www/html/images/hairline.gif
/var/www/html/images/left.gif
/var/www/html/images/left_on.gif
/var/www/html/images/right.gif
/var/www/html/images/right_last.gif
/var/www/html/images/right_on.gif
/usr/local/share/unxsVZ
/usr/local/sbin/vz_traffic_log.sh
/var/www/html/traffic

%changelog
* Mon Apr 11 2009 Gary Wallis <support@unixservice.com> 
- Added rrdtool traffic graph script install
* Sun Apr 10 2009 Dilva Garmendia <support@unixservice.com> 
- Initial RPM release

