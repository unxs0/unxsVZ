Summary: A small static library for unxsVZ templates
Name: unxstemplatelib
Version: 1.0
Release: 2
License: LGPL
Group: System Environment/Libraries
Source: http://unixservice.com/source/libs/unxstemplatelib-1.0.tar.gz
URL: http://unixservice.com
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Obsoletes: unxstemplate

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
%setup -q

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
* Fri Apr 20 2010 Gary Wallis <support@unixservice.com>
- Initial RPM release (of renamed and upgraded template.)
