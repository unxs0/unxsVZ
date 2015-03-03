#
#FILE
#	unxsScripts/unxsvz/makefile
#	$Id$
#AUTHOR/LEGAL
#	(C) 2001-2015 Gary Wallis for Unixservice, LLC. GPLv2 license applies.
#
#NOTES
#

CFLAGS=-Wall
LIBS=-L/usr/lib/mysql -L/usr/lib/openisp -L/usr/lib/oath -lmysqlclient -lz -lcrypt -lm -lssl -lucidr -ltemplate -loath
unxsvzSCRIPTS := $(wildcard sbin/unxsvz*.sh)

help:
	@echo "makefile options available"
	@echo "	install-scripts (installs unxsvzXXXX scripts in /usr/sbin)"
	@echo "	install-local.sh (installs /etc/unxsvz shared functions and settings)"
	@echo "	install-changerootpasswd (installs binary for changing root passwd)"
	@echo "	install-changestaffpasswd (installs binary for changing non root user passwd)"

install-scripts: $(unxsvzSCRIPTS)
	@ for cScript in $(unxsvzSCRIPTS) ; do \
		if [ ! -f /usr/$$cScript ];then /usr/bin/install $$cScript /usr/$$cScript;else echo "/usr/$$cScript already installed";fi \
	done

unxsvz.functions.sh: unxsvz.functions.sh.default
	@ if [ ! -f unxsvz.functions.sh ];then cp -i unxsvz.functions.sh.default unxsvz.functions.sh; fi

mysql.local.sh: mysql.local.sh.default
	@ if [ ! -f mysql.local.sh ];then cp -i mysql.local.sh.default mysql.local.sh; fi

iptables.local.sh: iptables.local.sh.default
	@ if [ ! -f iptables.local.sh ];then cp -i iptables.local.sh.default iptables.local.sh; fi

zabbix.local.sh: zabbix.local.sh.default
	@ if [ ! -f zabbix.local.sh ];then cp -i zabbix.local.sh.default zabbix.local.sh; fi

install-local.sh: mysql.local.sh iptables.local.sh zabbix.local.sh unxsvz.functions.sh
	@ if [ ! -f /etc/unxsvz/unxsvz.functions.sh ];then install unxsvz.functions.sh /etc/unxsvz/unxsvz.functions.sh;else echo "/etc/unxsvz/unxsvz.functions.sh already installed";fi
	@ if [ ! -f /etc/unxsvz/mysql.local.sh ];then install mysql.local.sh /etc/unxsvz/mysql.local.sh;else echo "/etc/unxsvz/mysql.local.sh already installed";fi
	@ if [ ! -f /etc/unxsvz/iptables.local.sh ];then install iptables.local.sh /etc/unxsvz/iptables.local.sh;else echo "/etc/unxsvz/iptables.local.sh already installed";fi
	@ if [ ! -f /etc/unxsvz/zabbix.local.sh ];then install zabbix.local.sh /etc/unxsvz/zabbix.local.sh;else echo "/etc/unxsvz/zabbix.local.sh already installed";fi
	@ rm -f *.sh

install-changerootpasswd: sbin/changerootpasswd.c
	gcc -Wall -o changerootpasswd sbin/changerootpasswd.c -lcrypt
	install -s changerootpasswd /usr/sbin/changerootpasswd
	chmod 500 /usr/sbin/changerootpasswd
	rm -f changerootpasswd

install-changestaffpasswd: sbin/changestaffpasswd.c
	gcc -Wall -o changestaffpasswd sbin/changestaffpasswd.c -lcrypt
	install -s changestaffpasswd /usr/sbin/changestaffpasswd
	chmod 500 /usr/sbin/changestaffpasswd
	rm -f changestaffpasswd
