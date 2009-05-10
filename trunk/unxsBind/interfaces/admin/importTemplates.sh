#!/bin/bash
#
#FILE
# $Id: importTemplates.sh 660 2008-12-18 18:48:48Z hus $
#PURPOSE
#	Import idnsAdmin.cgi templates in one fell swoop.
#	Very useful when moving application to new server.
#	Or during development work with vi editing of templates.
#AUTHOR
#	Hugo Urquiza for Unixservice (C) 2007
#CONTRIBUTORS
#	Jonathan Petersson
#
if [ ! -z "$CGIDIR" ]; then
    if [ -e "${CGIDIR}iDNS.cgi" ]; then
    for i in `find ./templates/* -type f`; do
        ${CGIDIR}iDNS.cgi ImportTemplateFile  `echo $i | cut -f 3 -d /`  ./$i plain;
    done
    else
    echo "iDNS.cgi isn't present in the CGIDIR you've defined!";
    fi
else
    echo "You haven't defined a CGIDIR, export CGIDIR=/cgi-bin/";
fi
