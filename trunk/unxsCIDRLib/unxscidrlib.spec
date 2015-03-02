%define name unxscidrlib
%define version 2.0
%define release 1


Summary: A small static library for unxsVZ IPv4 CIDR operations.
Name: %{name}
Version: %{version}
Release: %{release}
License: LGPL
Group: System Environment/Libraries
Source: http://unixservice.com/rpm/src/%{name}-%{version}.tar.gz
URL: http://unixservice.com
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Obsoletes: unxstemplate
BuildArch: x86_64
Provides: %{name}


%description
The unxscidrlib library provides functions for determining
if a given IPv4 is in a given IPv4 CIDR specified block
among other basic CIDR operations.
This package deprecates ucidr.

Current version provides these functions:

unsigned ExpandCIDR4(const char *cCIDR4, char *cIPs[]);
unsigned uIpv4InCIDR4(const char *cIPv4, const char *cCIDR4);
unsigned uInCIDR4Format(const char *cCIDR4,unsigned *uIPv4,unsigned *uCIDR4Mask);
unsigned uInIpv4Format(const char *cIPv4,unsigned *uIPv4);

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
%doc README LICENSE
/usr/lib/openisp/libucidr.a
/usr/include/openisp/ucidr.h


%changelog
* Sun Mar 1 2015 Gary Wallis <support@unixservice.com>
- First alpha version of 2.x series.
