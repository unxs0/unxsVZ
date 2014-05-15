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
		/var/log/snort/eth0/archive\
		/var/log/snort/eth0/barnyard2.waldo\
		/etc/sysconfig/snort\
		/etc/rc.d/init.d/snortd\
		/etc/logrotate.d/snort\
		/etc/cron.d/unxsvz_snort.cron\
		/etc/snort\
		/usr/share/man/man8/snort_selinux.8.gz\
		/usr/share/man/man8/snort.8.gz\
		/usr/bin/snort_control\
		/usr/lib64/snort-2.9.6.1_dynamicengine\
		/usr/lib64/snort-2.9.6.1_dynamicpreprocessor\
		/usr/sbin/snort\
		/usr/sbin/snort-plain\
		/usr/local/lib/snort_dynamicpreprocessor\
		/usr/lib64/libdnet*
