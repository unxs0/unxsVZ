#/bin/bash
#
#FILE
# $Id: importTemplates.sh 101 2009-05-28 17:10:23Z Hugo $
#PURPOSE
#	Import iRadius.cgi templates in one fell swoop.
#	Very useful when moving application to new server.
#	Or during development work with vi editing of templates.
#AUTHOR
#	Hugo Urquiza for Unixservice, LLC. (C) 2007-2010
#
#TODO
#	Setup a define/var for the path to iDNS.cgi or
#	command line arg $1 for example (what?)
#
for i in `find ./templates/* -type f`; do 
	/cgi-bin/unxsMail.cgi ImportTemplateFile  `echo $i | cut -f 3 -d /`  ./$i plain;
done
