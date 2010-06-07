#!/bin/bash

#FILE
#	$Id$
#PURPOSE
#	Provide a simple example for a persistent background ssh tunnel for remote
#	mysql connections. To avoid mysql C API ssl overhead.
#AUTHOR/LEGAL
#	(C) 2010 Unixservice, LLC.
#	GPLv2 license applies. See LICENSE file included.
#NOTES
#	You must make sure remote.net server has correct firewall settings and mysql grants.
#
/usr/bin/ssh -f root@n1.remote.net  -L 3306:n1.remote.net:3306 -N
