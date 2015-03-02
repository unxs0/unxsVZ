%define name unxstemplatelib
%define version 2.0
%define release 1


Summary: A small static library for unxsVZ templates
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
A library used by many unxsVZ C cgi programs for file and stream
creation based on templates with simple {{var}} substitution and
complex and multiline char data that is provided by externally
defined functions that replace {{funcX}} in templates. These
functions can recursively call other templates. Templates are 
stored in unxsVZ application MySQL tTemplate tables.
This package deprecates libtemplate. This new library provides
support for template sets (skins) and template types (interface types.)

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
/usr/lib/openisp/libtemplate.a
/usr/include/openisp/template.h


%changelog
* Sun Mar 1 2015 Gary Wallis <support@unixservice.com>
- First alpha version of 2.x series.
