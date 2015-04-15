%define name unxsscripts
%define version 1.0
%define release 3

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
/usr/sbin/changerootpasswd
/usr/sbin/changestaffpasswd
/usr/sbin/unxsvzAllNodeCmd.sh
/usr/sbin/unxsvzAllNodeScp.sh
/usr/sbin/unxsvzEveryNodeCmd.sh
/usr/sbin/unxsvzEveryNodeScp.sh
/usr/sbin/unxsvzCapacityPlanning.sh
/usr/sbin/unxsvzChangePBXPasswds.sh
/usr/sbin/unxsvzChangeRootPasswd.sh
/usr/sbin/unxsvzChangeStaffPasswd.sh
/usr/sbin/unxsvzDeleteZabbixHost.sh
/usr/sbin/unxsvzExecEachPBXContainer.sh
/usr/sbin/unxsvzGatherNodeInfo.sh
/usr/sbin/unxsvzGetPBXTrunkInfo.sh
/usr/sbin/unxsvzSpecNodeCmd.sh
/usr/sbin/unxsvzUpdateZabbixPBX.sh
/usr/sbin/unxsvzZabbixNotMonitored.sh


%changelog
* Wed Apr 15 2015 Gary Wallis <support@unixservice.com> 
- Added EveryNode scripts.
* Wed Mar 4 2015 Gary Wallis <support@unixservice.com> 
- Initial RPM release of new CentOS6 version
