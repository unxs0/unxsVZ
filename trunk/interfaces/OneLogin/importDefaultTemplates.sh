#!/bin/bash
#
#FILE
# $Id: importDefaultTemplates.sh 1335 2010-04-09 16:05:21Z Gary $
#PURPOSE
#	Import all OneLogin.cgi templates in one fell swoop.
#AUTHOR/LEGAL
#	(C) 2010,2015 Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies. See included LICENSE file in source root dir.
#
if [ -z "$CGIDIR" ];then
	CGIDIR="/var/www/unxs/cgi-bin/";
fi

if [ -e "${CGIDIR}unxsVZ.cgi" ]; then
    for cFile in `find ./templates.default -maxdepth 1 -type f`; do
    	if [[ $cFile != *.swp ]] && [[ $cFile != *.default ]];then
        	${CGIDIR}unxsVZ.cgi ImportTemplateFile  `echo $cFile | cut -f 3 -d /`  ./$cFile plain "OneLogin Interface";
	fi
    done
else
    echo "unxsVZ.cgi isn't present in the CGIDIR you've defined!";
fi
