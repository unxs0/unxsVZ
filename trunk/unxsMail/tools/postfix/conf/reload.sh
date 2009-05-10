#!/bin/bash

#
#FILE
#	$Id$
#	/etc/postfix/reload.cf
#PURPOSE
#AUTHOR
#	(C) 2006 Gary Wallis
#NOTES
#


make -C /etc/postfix
/etc/init.d/postfix reload
