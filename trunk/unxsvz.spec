%define name unxsvz
%define version 4.0
%define release 4

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
Provides: Binary programs and config files for unxsVZ hardware compute node.

%description
unxsVZ is a multiple datacenter, multiple hardware node, OpenVZ
container manager with autonomics, failover, auto rsync'd clones, and advanced migration.

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
exit 0

%build
exit 0

%install
exit 0

%clean
exit 0

%files
%defattr(-,root,root)
/var/www/unxs/html/traffic/
/usr/local/unxsVZ/data/
/usr/local/unxsVZ/setup/
/var/lib/rrd/
#not required
#/usr/lib/openisp/libunxsvz.a
#/usr/lib/oath/liboath.a
/usr/sbin/unxsVZ
/var/www/unxs/cgi-bin/unxsVZ.cgi

%changelog
* Tue Mar 10 2015 Gary Wallis <support@unixservice.com> 
- Alpha releases
* Tue Feb 3 2015 Gary Wallis <support@unixservice.com> 
- Initial RPM release of new CentOS 6 version
