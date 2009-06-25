Summary: sendmail and postfix controller as well as configuration generator for all mail services across multiple servers and multiple mail domains. 
Name: unxsmail
Version: 1.2
Release: 1
License: GPL
Group: System Environment/Applications
Source: http://unixservice.com/source/unxsmail-1.2.tar.gz
URL: http://openisp.net/openisp/unxsVZ
Distribution: unxsVZ
Vendor: Unixservice, LLC.
Packager: Unixservice Support Group <supportgrp@unixservice.com>
Requires: mysql-server >= 5.0.45, unxsadmin

%description
New RAD3 based sendmail and postfix controller as well as configuration generator for all mail services across multiple servers and multiple mail domains. For ESMTP, pop3, imap, webmail, clamAV, Spamassassin, procmail, etc. Replaces older tech mysqlSendmail and mysqlPostfix.

%prep
%setup

%build
make
cd interfaces/user
make
cd $RPM_BUILD_DIR

%install
install -s unxsMail.cgi /var/www/unxs/cgi-bin/unxsMail.cgi
install -s interfaces/user/interface.cgi /var/www/unxs/cgi-bin/unxsMaiUser.cgi
mkdir -p /usr/local/share/unxsMail/data
cp data/*.txt /usr/local/share/unxsMail/data/

%clean

%files
%doc INSTALL LICENSE
/var/www/unxs/cgi-bin/unxsMail.cgi
/var/www/unxs/cgi-bin/unxsMailUser.cgi
/usr/local/share/unxsMail/

%changelog
* Thu Jun 25 2009 - Hugo Urquiza <support2@unixservice.com>
- Minor data file update
* Wed Jun 24 2009 - Hugo Urquiza <support2@unixservice.com>
- New unxsMailUser interface, spec file fixes
* Thu May 28 2009 - Hugo Urquiza <support2@unixservice.com>
- Initial RPM release


