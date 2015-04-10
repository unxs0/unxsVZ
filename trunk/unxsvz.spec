%define name unxsvz
%define version 5.0
%define release 2

Summary: unxsVZ (CentOS 6 yum version) is a multiple datacenter and hardware node, OpenVZ manager with autonomics.
Name: %{name}
Version: %{version}
Release: %{release}
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/%{name}-%{version}.tar.gz
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: unxsadmin
BuildArch: x86_64
Provides: unxsVZ unxsUBC unxsOVZ

%description
unxsVZ is a multiple datacenter, multiple hardware node, OpenVZ
container manager with autonomics, failover, auto rsync'd clones, and advanced migration.

Provides /usr/sbin binaries unxsVZ unxsUBC unxsOVZ.

unxsVZ has internally managed rrdtool traffic graphics per datacenter, node and container.

unxsVZ supports and will support more and more HA and load balancing
functions across your private internet cloud.

unxsVZ is already in commercial production and has proven itself very stable
and extensible.

unxsVZ solves numerous internet infrastructure provisioning problems including
cost, maintenance and monitoring. 

This binary install uses -uunxsvz -pwsxedc 127.0.0.1:3306 for MySQL db connection.
Use unxsauthssh for secure ssh tunnel to actual remote database.
Or compile from sources for local skip-networking local socket connection.

You will most likely need unxsscripts and unxscron installed also for any real functionality.


%prep
#test just moving stuff already built
mkdir -p ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/sbin/
mkdir -p ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/var/www/unxs/cgi-bin
mkdir -p ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/var/www/unxs/html/traffic
mkdir -p ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/local/unxsVZ/data/
mkdir -p ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/local/unxsVZ/setup/
mkdir -p ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/var/lib/rrd/
cp /usr/sbin/unxsVZ ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/sbin/
cp /usr/sbin/unxsUBC ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/sbin/
cp /usr/sbin/unxsOVZ ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/sbin/
cp /var/www/unxs/cgi-bin/unxsVZ.cgi ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/var/www/unxs/cgi-bin/
exit 0

%build
exit 0

%install
exit 0

%clean
rm -rf ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64
exit 0

%files
%defattr(-,root,root)
/var/www/unxs/html/traffic/
/usr/local/unxsVZ/data/
/usr/local/unxsVZ/setup/
/var/lib/rrd/
/usr/sbin/unxsVZ
/usr/sbin/unxsUBC
/usr/sbin/unxsOVZ
/var/www/unxs/cgi-bin/unxsVZ.cgi

%changelog
* Wed Mar 30 2015 Gary Wallis <support@unixservice.com> 
- Added unxsUBX and unxsOVZ binaries.
* Wed Mar 27 2015 Gary Wallis <support@unixservice.com> 
- New unxsVZ AddContainers command.
* Wed Mar 25 2015 Gary Wallis <support@unixservice.com> 
- New unxsVZ AddHardwareNode command.
* Tue Mar 10 2015 Gary Wallis <support@unixservice.com> 
- Alpha releases
* Tue Feb 3 2015 Gary Wallis <support@unixservice.com> 
- Initial RPM release of new CentOS 6 version
