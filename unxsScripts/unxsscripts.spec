%define name unxsscripts
%define version 1.0
%define release 4

Summary: unxsScripts provides initial unxsVZ system scripts.
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
Provides: /etc/unxsvz, /usr/sbin/changerootpasswd, /usr/sbin/changestaffpasswd and /usr/sbin/*.sh scripts

%description
unxvScripts provides initial unxsVZ system scripts and basic BASH environment.

%prep
#just moving stuff, this is not standard practice, fix asap.
mkdir -p ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/etc/unxsvz/
mkdir -p ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/sbin/
cp ~/unxsVZ/unxsScripts/unxsvz/sbin/*.sh ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/sbin/
rm ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/sbin/unxsvzGetPBXTrunkInfo12.sh
cp ~/unxsVZ/unxsScripts/unxsvz/*.sh.default ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/etc/unxsvz/
for cScript in ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/etc/unxsvz/*.sh.default ; do
	cp $cScript ${cScript%.*};
done
for cScript in ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/etc/unxsvz/*.sh.default ; do
	rm $cScript;
done
gcc -Wall -o changerootpasswd ~/unxsVZ/unxsScripts/unxsvz/sbin/changerootpasswd.c -lcrypt
install -s changerootpasswd ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/sbin/changerootpasswd
chmod 500 ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/sbin/changerootpasswd
rm -f changerootpasswd
gcc -Wall -o changestaffpasswd ~/unxsVZ/unxsScripts/unxsvz/sbin/changestaffpasswd.c -lcrypt
install -s changestaffpasswd ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/sbin/changestaffpasswd
chmod 500 ~/rpm/BUILDROOT/%{name}-%{version}-%{release}.x86_64/usr/sbin/changestaffpasswd
rm -f changestaffpasswd
exit 0

%build
exit 0

%install
exit 0

%post
chmod 500 /etc/unxsvz
exit 0

%clean
exit 0

%files
%defattr(-,root,root)
/etc/unxsvz
/usr/sbin

%changelog
* Wed Apr 15 2015 Gary Wallis <support@unixservice.com> 
- Added EveryNode scripts. Added new graphics subsystem scripts.
* Wed Mar 4 2015 Gary Wallis <support@unixservice.com> 
- Initial RPM release of new CentOS6 version
