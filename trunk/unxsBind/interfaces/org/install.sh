#!/bin/bash
#
#FILE
# $Id: install.sh 663 2008-12-18 19:00:37Z hus $
#PURPOSE
#       Make a full install of idnsOrg easy.
#AUTHOR
#      Jonathan Petersson and Hugo Urquiza for Unixservice (C) 2008-2009
#

# CGIDIR is identified automatically but can be set manually here or with ARGV
#export CGIDIR=

# Collect ARGV
declare -r ARGS="${0} ${@}"
declare -i ARGC=${#}+1
declare -a ARGV=($ARGS)

# Usage print function
function usage {
    echo "
    Syntax: $0 -w <webdir> [-c <cgidir>]

    Use this script to install idnsAdmin.

    Options:
    -h|--help     : Show this help text
    
    -w|--webdir   : Set the root web directory (E.g. /var/www/htdocs)
    -c|--cgidir   : Set the cgi-bin location
    -n|--nobuildcheck   : Disabled build environment check (Not recommended)
    "
    exit 0;
};

# Use ARGV to set variables
for (( i=1 ; i < $ARGC ; i++ )); do
    case ${ARGV[$i]} in
        '-w'|'--webdir')
            cWebdir=${ARGV[$i+1]};
            ;;
        '-c'|'--cgidir')
            export CGIDIR=${ARGV[$i+1]};
            ;;
        '-h'|'--help')
            usage;
            ;;
	'-n'|'--nobuildcheck')
	    NOBUILDCHECK=1
	    ;;
    esac
done

# Check that all variables is set
if [ "${cWebdir}" = "" ]; then
    usage;
fi

# Try to locate default cgi-bin folder
if [ "$CGIDIR" = "" ] || [ ! -e "$CGIDIR" ]; then
    if [ -r "cgi-bin" ]; then
        CGIDIR=cgi-bin/
    elif [ -r "/var/www/cgi-bin" ]; then
        export CGIDIR=/var/www/cgi-bin/
        DISTRO=Fedora
    elif [ -r "/usr/lib/cgi-bin" ]; then
        export CGIDIR=/usr/lib/cgi-bin/
        DISTRO=Ubuntu
    elif [ -r "/usr/local/www/cgi-bin" ]; then
        export CGIDIR=/usr/local/www/cgi-bin
        DISTRO=FreeBSD
    else
        echo "Unable to locate your cgi-bin directory. Please define it with the -c flag."
        usage;
    fi
fi

if [ "$NOBUILDCHECK" != 1 ]; then
        source ../../buildcheck.sh;
        if [ "$BUILDOK" != 1 ]; then
                echo "Your building environment seems incomplete. Can't continue";
                exit 1;
        fi
fi

# Install functions
make clean
make install

if [ ! -x "./importTemplates.sh" ]; then
	echo "Your idnsAdmin copy seems incomplete. Could not execute the importTemplates.sh script.";
	exit 1;
fi

# Install templates
./importTemplates.sh

# Install images

if [ ! -x "${cWebdir}/images/" ]; then
	mkdir ${cWebdir}/images/;
	if [ -x "${cWebdir}/images/" ]; then
		echo "Could not create ${cWebdir}/images directory. Check your permissions";
		exit 1;
	fi
fi

# Install required stylesheet and interface images
cp ../admin/templates/images/* ${cWebdir}/images/
cp ../admin/templates/popups.js ${cWebdir}/
cp ../admin/templates/styles.css ${cWebdir}/

exit 0;

exit 0;
