#/bin/bash
#
#FILE
#	$Id: importTemplates.sh 85 2009-05-26 15:30:04Z Hugo $
#PURPOSE
#	Import ispAdmin.cgi templates in one fell swoop.
#	Very useful when moving application to new server.
#	Or during development work with vi editing of templates.
#AUTHOR
#	Hugo Urquiza for Unixservice (C) 2007-2009
#
#
for i in `find ./templates/* -type f`; do 
	/var/www/unxs/cgi-bin/unxsISP.cgi ImportTemplateFile  `echo $i | cut -f 3 -d /`  ./$i plain;
done
