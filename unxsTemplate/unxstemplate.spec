Summary: A small static library for unxsVZ templates
Name: unxstemplate
Version: 1.2
Release: 1
License: LGPL
Group: System Environment/Libraries
Source: http://unixservice.com/source/libs/unxstemplate-1.2.tar.gz
URL: http://unixservice.com/unxstemplate
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>

%description
A library used by many unxsVZ C cgi programs for file and stream
creation based on templates with simple {{var}} substitution and
complex and multiline char data that is provided by externally
defined functions that replace {{funcX}} in templates. These
functions can recursively call other templates. Templates are 
stored in unxsVZ application MySQL tTemplate tables.

%prep
%setup

%build
make

%install
make install

%clean

%files
%doc README LICENSE
/usr/lib/openisp/libtemplate.a
/usr/include/openisp/template.h

%changelog
* Fri Apr 09 2010 Gary Wallis <support@unixservice.com>
- New version with support for template set and type.
* Tue Nov 17 2009 Dilva Garmendia <support@unixservice.com> 
- New version with improved docs, makefile. Removed textarea hack.
* Sat Apr 09 2009 Dilva Garmendia <support@unixservice.com> 
- Initial RPM release
