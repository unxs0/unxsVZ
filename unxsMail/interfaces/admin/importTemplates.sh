#!/bin/bash
#
#FILE
# $Id: importTemplates.sh 421 2007-02-22 22:42:48Z root $
#PURPOSE
#	Import mail2Admin.cgi templates in one fell swoop.
#	Very useful when moving application to new server.
#	Or during development work with vi editing of templates.
#AUTHOR
#	Hugo Urquiza for Unixservice (C) 2007-2008
#CONTRIBUTORS
#	Jonathan Petersson
#
if [ ! -z "$CGIDIR" ]; then
    if [ -e "${CGIDIR}mysqlMail2.cgi" ]; then
    for i in `find ./templates/* -type f`; do
        ${CGIDIR}mysqlMail2.cgi ImportTemplateFile  `echo $i | cut -f 3 -d /`  ./$i plain;
    done
    else
    echo "mysqlMail2.cgi isn't present in the CGIDIR you've defined!";
    fi
else
    echo "You haven't defined a CGIDIR, export CGIDIR=/cgi-bin/";
fi
