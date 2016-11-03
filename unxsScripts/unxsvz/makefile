#
#FILE
#	unxsScripts/unxsvz/makefile
#	svn ID removed
#AUTHOR/LEGAL
#	(C) 2001-2015 Gary Wallis for Unixservice, LLC. GPLv2 license applies.
#
#NOTES
#

CFLAGS=-Wall
LIBS=-L/usr/lib/mysql -L/usr/lib/openisp -L/usr/lib/oath -lmysqlclient -lz -lcrypt -lm -lssl -lucidr -ltemplate -loath
unxsvzSCRIPTS := $(wildcard sbin/unxsvz*.sh)
unxsvzLOCALDEF:= $(wildcard *.default)

help:
	@echo "makefile options available"
	@echo "	install-scripts (installs unxsvzXXXX scripts in /usr/sbin)"
	@echo "	install-local (installs /etc/unxsvz shared functions and settings)"
	@echo "	install-changerootpasswd (installs binary for changing root passwd)"
	@echo "	install-changestaffpasswd (installs binary for changing non root user passwd)"

install-scripts: $(unxsvzSCRIPTS)
	@ for cScript in $(unxsvzSCRIPTS) ; do \
		if [ ! -f /usr/$$cScript ];then /usr/bin/install $$cScript /usr/$$cScript;else echo "/usr/$$cScript already installed";fi \
	done

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

install-local: $(unxsvzLOCALDEF)
	@ mkdir -p /etc/unxsvz
	@ chmod 500 /etc/unxsvz;
	@ chown root:root /etc/unxsvz;
	@ for cScript in $(unxsvzLOCALDEF) ; do \
		cp -i $$cScript "$${cScript%.*}";\
	done
	@ for cScript in *.sh ; do \
		if [ ! -f /etc/unxsvz/$$cScript ];then /usr/bin/install $$cScript /etc/unxsvz/$$cScript;else echo "/etc/unxsvz/$$cScript already installed";fi \
	done
	@ for cScript in *.sh ; do \
		chmod 400 /etc/unxsvz/$$cScript; \
	done
	@ for cScript in *.sh ; do \
		chown root:root /etc/unxsvz/$$cScript; \
	done
	@ rm -f *.sh
