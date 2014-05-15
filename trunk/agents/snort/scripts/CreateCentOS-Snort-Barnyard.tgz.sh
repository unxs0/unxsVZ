#!/bin/bash

/bin/tar czvf /home/unxs/centos6.snort-barnyard.tar.gz \
		--exclude=/var/log/snort/eth0/archive/snort.log.*\
		--exclude=/var/log/snort/snort.log.*\
		--exclude=/var/log/snort/alert-*\
		/var/log/barnyard2\
		/etc/sysconfig/barnyard2\
		/etc/rc.d/init.d/barnyard2\
		/etc/snort/barnyard.conf\
		/usr/bin/barnyard2\
		/usr/local/bin/barnyard2\
		/var/log/snort/eth0/archive\
		/var/log/snort/eth0/barnyard2.waldo\
		/etc/sysconfig/snort\
		/etc/rc.d/init.d/snortd\
		/etc/logrotate.d/snort\
		/etc/cron.d/unxsvz_snort.cron\
		/etc/snort\
		/usr/share/man/man8/snort.8.gz\
		/usr/bin/snort_control\
		/usr/lib64/snort-2.9.6.1_dynamicengine\
		/usr/lib64/snort-2.9.6.1_dynamicpreprocessor\
		/usr/lib64/libsfbpf*\
		/usr/sbin/snort\
		/usr/sbin/snort-plain\
		/usr/sbin/unxsSnortPost*.sh\
		/usr/local/lib/snort_dynamicpreprocessor\
		/usr/lib64/libdnet*

#after installing these files:
#	coment off /etc/cron.d/unxsvz_snort.cron temporarily 4 test cli
#	comment off /usr/sbin/unxsSnortPost*.sh so you wont send email while testing.
#	setup mysql see /etc/snort/install
#	svn co unxsSnort and compile and test
#	make sure that /etc/snort/ignore.bpf has the correct ranges in it.
#	groupadd snort
#	useradd snort
#	chown -R snort:snort /var/log/snort
#	service snortd start
#	service barnyard2 start
#	look at log files
#	make sure data is being written to mySQL snort db
#	run unxsSnort --process from the CLI
