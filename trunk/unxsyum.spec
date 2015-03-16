%define name unxsyum
%define version 2.0
%define release 1

Summary: unxsyum provides the unxs yum repo config file
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
Provides: The /etc/yum.repos.d/unxs.repo file.

%description
This rpm adds the unxs.repo file to your CentOS yum configuration. This allows you to use
yum for unxsVZ provided packages.

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
/etc/yum.repos.d/unxs.repo

%changelog
* Mon Mar 16 2015 Gary Wallis <support@unixservice.com> 
- Initial RPM release of new CentOS6 version
