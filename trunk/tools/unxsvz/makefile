#
#FILE
#	tools/unxsvz/makefile
#	$Id$
#AUTHOR/LEGAL
#	(C) 2001-2014 Gary Wallis for Unixservice, LLC. GPLv2 license applies.
#
#NOTES
#

CFLAGS=-Wall
LIBS=-L/usr/lib/mysql -L/usr/lib/openisp -L/usr/lib/oath -lmysqlclient -lz -lcrypt -lm -lssl -lucidr -ltemplate -loath

all: install-local.sh install-scripts install-dir

mysql.local.sh: mysql.local.sh.default
	@ if [ ! -f mysql.local.sh ];then cp -i mysql.local.sh.default mysql.local.sh; fi

iptables.local.sh: iptables.local.sh.default
	@ if [ ! -f iptables.local.sh ];then cp -i iptables.local.sh.default iptables.local.sh; fi

install-local.sh: mysql.local.sh iptables.local.sh
	@ if [ ! -f /etc/unxsvz/mysql.local.sh ];then install mysql.local.sh /etc/unxsvz/mysql.local.sh;else echo "/etc/unxsvz/mysql.local.sh already installed";fi
	@ if [ ! -f /etc/unxsvz/iptables.local.sh ];then install iptables.local.sh /etc/unxsvz/iptables.local.sh;else echo "/etc/unxsvz/iptables.local.sh already installed";fi

install-scripts: sbin/unxsvzUpdateZabbixPBX.sh
	@ if [ ! -f /usr/sbin/unxsvzUpdateZabbixPBX.sh ];then install sbin/unxsvzUpdateZabbixPBX.sh /usr/sbin/unxsvzUpdateZabbixPBX.sh;else echo "/usr/sbin/unxsvzUpdateZabbixPBX.sh already installed";fi

install-dir: zabbix_agentd_upgrade/README
	@ if [ ! -d /etc/unxsvz/zabbix_agentd_upgrade ];then mkdir -p /etc/unxsvz/zabbix_agentd_upgrade; install zabbix_agentd_upgrade/README /etc/unxsvz/zabbix_agentd_upgrade/README;else echo "/etc/unxsvz/zabbix_agentd_upgrade already exists";fi
