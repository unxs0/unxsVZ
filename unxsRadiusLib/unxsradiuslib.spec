Summary: A small static RADIUS C API library and dictionary
Name: unxsradiuslib
Version: 1.0
Release: 1
License: LGPL
Group: System Environment/Libraries
Source: http://unixservice.com/source/libs/unxsradiuslib-1.0.tar.gz
URL: http://unixservice.com
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>

%description
unxsradiuslib is a small static library and a dictionary file.
unxsradiuslib provides a simple but reliable C RADIUS API for both
authorization and accounting operations. The authorization
function allows for reading returned extra authorization data.
This package deprecates uradius.

%prep
%setup

%build
make

%install
make install

%clean

%files
/usr/lib/openisp/liburadius.a 
/usr/local/etc/raddb/dictionary 
/usr/include/openisp/uradius.h 

%changelog
* Tue Apr 20 2010 Gary Wallis <support@unixservice.com> 
- Initial RPM release (of renamed package, used to be uradius)
