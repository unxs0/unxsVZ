#/bin/bash
#
#FILE
# $Id: importTemplates.sh 421 2007-02-22 22:42:48Z root $
#PURPOSE
#	Import idnsAdmin.cgi templates in one fell swoop.
#	Very useful when moving application to new server.
#	Or during development work with vi editing of templates.
#AUTHOR
#	Hugo Urquiza for Unixservice (C) 2007-2008
#
#TODO
#	Setup a define/var for the path to iDNS.cgi or
#	command line arg $1 for example
#
for i in `find ./templates/* -type f`; do 
	/cgi-bin/mysqlISP2.cgi ImportTemplateFile  `echo $i | cut -f 3 -d /`  ./$i plain;
done
