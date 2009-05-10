Summary: A small static RADIUS C API library and dictionary
Name: uradius
Version: 1.0
Release: 1
License: LGPL
Group: System Environment/Libraries
Source: http://unixservice.com/source/libs/uradius-1.0.tar.gz
URL: http://unixservice.com/uradius
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>

%description
uradius is a small static library and a dictionary file.
uradius provides a simple but reliable C RADIUS API for both
authorization and accounting operations. The authorization
function allows for reading returned extra authorization data.

%prep
%setup

%build
make

%install
make install

%clean

%files
/usr/lib/liburadius.a 
/usr/local/etc/raddb/dictionary 
/usr/include/uradius.h 

%changelog
* Sat Apr 09 2009 Dilva Garmendia <support@unixservice.com> 
- Initial RPM release
