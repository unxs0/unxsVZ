%define name unxsscripts
%define version 1.0
%define release 1

Summary: unxsScripts provides initial unxsVZ system scripts.
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
Provides: %{name}

%description
unxScripts provides initial unxsVZ system scripts and basic BASH environment.

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
/etc/unxsvz

%changelog
* Tue Feb 3 2015 Gary Wallis <support@unixservice.com> 
- Initial RPM release of new CentOS6 version
