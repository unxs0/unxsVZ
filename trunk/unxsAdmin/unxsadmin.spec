Summary: unxsadmin provides the http shared content and httpd conf.d file for all unxsVZ web admins
Name: unxsadmin
Version: 1.7
Release: 5
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/rpm/src/unxsadmin-1.7.tar.gz
URL: http://unixservice.com
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: httpd, mod_ssl, rrdtool
BuildArch: noarch
BuildRoot: %{_builddir}/%{name}-root
Provides: unxsAdmin

%description
unxsadmin provides the http shared content and conf for all unxsVZ web admins.
It provides the dir layout in /var/www/unxs and all the shared css, js and image
content needed by unxsVZ family of web administration interfaces like
unxsMail, unxsApache, unxsVZ, unxsBind and unxsISP.
It also provides some common binary utilities like lastmonth.
It does not provide advanced interface web admin, images, css and js.
For example the new OneLogin, mobile first, responsive, bootstrap based JQuery 
interface, requires that you install files in the /var/www/unxs directory structure.

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
/var/www/unxs
%config(noreplace) /etc/httpd/conf.d/unxs.conf
#this is a simple binary that runs on any x86 Linux. Fix this.
/usr/bin/lastmonth

%changelog
* Sun Feb 22 2015 Gary Wallis <support@unixservice.com>
- Repackaged as noarch, simplified and corrected spec file.
* Fri Apr 30 2010 Gary Wallis <support@unixservice.com>
- Added the Unxs X favicon.ico.
* Fri Sep 22 2009 Gary Wallis <support@unixservice.com>
- Clean up and modernization, added an img for unxsVZ.
* Fri May 15 2009 Hugo Urquiza <support2@unixservice.com>
- RPM rebuild removing extra files
* Wed Apr 11 2009 Gary Wallis <support@unixservice.com> 
- Fixed unxs.conf 

