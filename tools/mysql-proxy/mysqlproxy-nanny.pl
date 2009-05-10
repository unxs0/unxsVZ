#!/usr/bin/perl

# FILE
# 	$Id$
# Copyright (C) 2007-2009 Unixservice
# Copyright (C) 2004  Internet Systems Consortium, Inc. ("ISC")
# Copyright (C) 2000, 2001  Internet Software Consortium.
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
# OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

#*Modified for use with mysqlproxy by (C) Unixservice mods GPLv2 license applies to this
#derivative work. The ISC (C) above applies as well.

$pid_file_location = '/var/run/mysql-proxy.pid';

fork() && exit();

#printf("/usr/sbin/mysqlproxy-nanny.pl starting\n");

for (;;) {
	$pid = 0;

	#printf("checking $pid_file_location\n");

	open(FILE, $pid_file_location) || goto restart;
	$pid = <FILE>;
	close(FILE);
	chomp($pid);

	#printf("checking $pid\n");

	$res = kill 0, $pid;
	goto restart if ($res == 0);

	sleep 10;
	next;

 restart:
	#printf("restarting\n");

	system("/usr/sbin/start-mysqlproxy.sh");
	sleep 10;
}

