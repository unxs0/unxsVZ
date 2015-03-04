%define name unxscron
%define version 1.0
%define release 1

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
BuildArch: x86_64
Provides: /etc/cron.d/ files.

%description
unxsCron provides initial unxsVZ system /etc/cron.d/ contents.

%prep
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
%defattr(-,root,root)
/etc/cron.d/

%changelog
* Wed Mar 4 2015 Gary Wallis <support@unixservice.com> 
- Initial RPM release of new CentOS6 version
