#/bin/bash
#
#FILE
# $Id: importTemplates.sh 1395 2007-08-08 21:12:04Z Gary $
#PURPOSE
#	Import templates in one fell swoop.
#	Very useful when moving application to new server.
#	Or during development work with vi editing of templates.
#AUTHORS
#	Hugo Urquiza
#	Gary Wallis
#	for Unixservice (C) 2007
#
cRADProgram="unxsMail";

if [ $2 ]; then
	if [ $CGIDIR ]; then
		for i in `find $1/* -type f`; do 
		$CGIDIR/$cRADProgram.cgi ImportTemplateFile  `echo ${i##*/}`  $i $2;
		done
	else
		echo "To use $0 you must set the env variable $CGIDIR (Ex. export CGIDIR=/cgi-bin)";
	fi
else
	echo "Usage: $0 <path to template dir> <template set name>";
fi
