%define name unxsautossh
%define version 1.0
%define release 3

Summary: unxsAutoSSH provides default ssh encrypted mysql communications for unxsVZ.
Name: %{name}
Version: %{version}
Release: %{release}
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/%{name}-%{version}.tar.gz
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
BuildArch: noarch
Requires: autossh
Provides: %{name}

%description
Provides fast and low overhead encrypted MySQL communications for OpenVZ private cloud manager unxsVZ.

Also solves problem for yum install of default unxsVZ installation regarding configuration of
MySQL database connectivity. It does this by providing this middleware that can be easily and securely
configured for any pair of MySQL servers, local or external.

You will need to configure /etc/init.d/unxsautossh

You need a cert based user to create ssh tunnel connect with no password.

You need to adjust MySQL permissions for the tunnel.

%prep
mkdir -p ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.noarch/etc/init.d/
cp -i unxsVZ/unxsAutoSSH/unxsautossh ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.noarch/etc/init.d/
cp -i unxsVZ/unxsAutoSSH/unxsautossh2 ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.noarch/etc/init.d/
exit 0

%build
exit 0

%install
exit 0

%clean
exit 0

%files
%defattr(-,root,root)
/etc/init.d/unxsautossh
/etc/init.d/unxsautossh2

%changelog
* Sat Apr 18 2015 Gary Wallis <support@unixservice.com> 
- Added a second init.d script for dual tunnel support for HA MySQL server connections.
* Mon Feb 2 2015 Gary Wallis <support@unixservice.com> 
- Initial RPM release of new CentOS6 version
