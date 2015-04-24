%define name unxscron
%define version 1.0
%define release 3

Summary: unxsCron provides initial unxsVZ system /etc/cron.d/ contents.
Name: %{name}
Version: %{version}
Release: %{release}
License: GPLv2
Group: System Environment/Applications
Source: http://unixservice.com/source/%{name}-%{version}.tar.gz
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
BuildArch: noarch
Provides: unxsVZ basic /etc/cron.d/ files.

%description
unxsCron provides initial unxsVZ system /etc/cron.d/ contents.

%prep
mkdir -p ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/etc/cron.d/
cp ~/unxsVZ/unxsCron/cron.d/* ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/etc/cron.d/
exit 0

%build
exit 0

%install
exit 0

%post
exit 0

%clean
exit 0

%files
/etc/cron.d/

%changelog
* Wed Apr 15 2015 Gary Wallis <support@unixservice.com> 
- New cleaned up cron files. Prep section added. Changed to correct BuildArch noarch.
* Wed Mar 4 2015 Gary Wallis <support@unixservice.com> 
- Initial RPM release of new CentOS6 version
