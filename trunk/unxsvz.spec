Summary: unxsVZ (CentOS5 yum version) is a multiple datacenter and hardware node, OpenVZ manager with autonomics.
Name: unxsvz
Version: 2.0
Release: 3
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsvz-2.0.tar.gz
URL: http://openisp.net/openisp/unxsVZ
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: mysql >= 5.0.45, httpd, mod_ssl, ovzkernel, vzctl, vzdump, cstream, unxsadmin

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
cd ../../data
mkdir -p /usr/local/share/unxsVZ/data/
cp *.txt /usr/local/share/unxsVZ/data/
cd ../tools/rrdtool/
install -m 700 vz_traffic_log.sh /usr/local/sbin/vz_traffic_log.sh
mkdir -p /var/www/html/traffic/
cd $RPM_BUILD_DIR


%clean

%files
%doc INSTALL LICENSE
/usr/sbin/unxsUBC
/var/www/unxs/cgi-bin/unxsVZ.cgi
%dir /usr/local/share/unxsVZ
%config(noreplace) /usr/local/sbin/vz_traffic_log.sh
%dir /var/www/html/traffic
/usr/local/share/unxsVZ/data/

%changelog
* Fri Jul 24 2009 Hugo Urquiza <support2@unixservice.com>
- Updated spec file for correct rpm package building
* Fri Jul 24 2009 Hugo Urquiza <support2@unixservice.com>
- Updated spec file for correct rpm package building..
* Mon Apr 11 2009 Gary Wallis <support@unixservice.com> 
- Fixed perm install error for vz_traffic_log.sh
* Mon Apr 11 2009 Gary Wallis <support@unixservice.com> 
- Added rrdtool traffic graph script install
* Sun Apr 10 2009 Dilva Garmendia <support@unixservice.com> 
- Initial RPM release

