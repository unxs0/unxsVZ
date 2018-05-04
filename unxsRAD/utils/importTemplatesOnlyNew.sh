#!/bin/bash
#
#FILE
# $Id: importTemplates.sh 1954 2012-05-22 16:11:09Z Colin $
#PURPOSE
#	Import unxsRAD.cgi templates in one fell swoop.
#	Very useful when moving application to new server.
#	Or during development work with vi editing of templates.
#AUTHOR/LEGAL
#	(C) 2007-2009 Hugo Urquiza,Jonathan Petersson for Unixservice, LLC.
#	(C) 2010-2017 Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies. See included LICENSE file in source root dir.
#CONTRIBUTORS
#	Jonathan Petersson
#

cTemplateSet="default";
if [ -d "$CGIDIR" ];then
	cCGIDIR=${CGIDIR};
	echo $cCGIDIR;
fi
if [ -d "$UNXSRAD" ];then
	cunxsRAD=${UNXSRAD};
	echo $cunxsRAD;
fi

if [ "$cunxsRAD" != "" ] && [ "$cCGIDIR" != "" ];then
	if [ "$1" == "--help" ];then
		echo "usage: $0 [<template set>] | --help";
		echo cgi-dir set via ENV $cCGIDIR;
		echo unxsRAD dir set via ENV $cunxsRAD;
		echo "	ttemplate set default is $cTemplateSet."
		exit;
	fi

	if [ "$1" != "" ];then
		cTemplateSet=$1;
		echo "$cTemplateSet";
	fi

else
	if [ "$2" == "" ] || [ "$1" == "--help" ];then
		echo "usage: $0 <unxsRAD dir> <cgi-dir> [<template set>] | --help";
		echo "	or export CGIDIR=/var/www/cgi-bin && export UNXSRAD=/home/unxs/unxsVZ/unxsRAD";
			echo "	and then usage is only: $0 [<template set>]";
		echo "	unxsRAD dir e.g. /home/unxs/unxsVZ/unxsRAD"
		echo "	cgi-dir e.g. /var/www/cgi-bin"
		echo "	ttemplate set default is $cTemplateSet."
		exit;
	fi

	if [ "$1" != "" ];then
		cunxsRAD=$1;
	fi

	if [ "$2" != "" ];then
		cCGIDIR=$2;
	fi

	if [ "$3" != "" ];then
		cTemplateSet=$3;
	fi
fi

#echo "d1 $1";
#exit;

if [ ! -d "$cunxsRAD/templates/$cTemplateSet" ]; then
	echo "no dir $cunxsRAD/templates/$cTemplateSet";
	exit 1;
fi

if [ -e "$cCGIDIR/unxsRAD.cgi" ]; then
    for i in `find $cunxsRAD/templates/$cTemplateSet/*/ -maxdepth 1 -type f -mmin -15`; do
	cTemplateType=`echo $i | rev | cut -f 2 -d / | rev`;
	cFilename=`echo $i | rev | cut -f 1 -d / | rev`;
        $cCGIDIR/unxsRAD.cgi ImportTemplateFile $cFilename $i $cTemplateSet $cTemplateType;
    done
else
	echo "unxsRAD.cgi isn't present in the CGIDIR you've defined!";
fi
