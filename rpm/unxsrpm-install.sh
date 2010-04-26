#!/bin/bash

#FILE
#	$Id$
#	/usr/sbin/unxsrpm-install.sh
#PURPOSE
#	Automate yum and source repository management
#TODO
#	Needs to distinguish libs from app pkgs for tar.gz source file placement.
#AUTHOR/LEGAL
#	(C) 2010 Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies. See LICENSE file in distribution root directory.

if [ "$1" == "" ];then
        echo "usage $0 <rpm-prefix example: unxsvz-1.0-4> [<tar.gz-prefix example: unxsvz-1.0>]";
        exit 0;
fi

if [ ! -f $1.i386.rpm ];then
        echo "error no $1.i386.rpm";
        exit 1;
fi

if [ ! -f $1.src.rpm ];then
        echo "error no $1.src.rpm";
        exit 1;
fi

if [ -f $2.tar.gz ];then
        mv $2.tar.gz /var/www/html/source/
        if [ $? != 0 ];then
                echo "error tar.gz mv";
        fi
        /usr/bin/md5sum /var/www/html/source/$2.tar.gz > /var/www/html/source/$2.tar.gz.md5sum
        if [ $? != 0 ];then
                echo "error tar.gz md5sum";
        fi
        chown root:root /var/www/html/source/$2.tar.gz
fi

mv $1.i386.rpm /var/www/html/rpm/i386/
if [ $? != 0 ];then
        echo "error 1";
        exit 1;
fi
mv $1.src.rpm /var/www/html/rpm/src/
if [ $? != 0 ];then
        echo "error 2";
        exit 1;
fi

cd /var/www/html/rpm/i386/
chown root:root $1.i386.rpm
if [ $? != 0 ];then
        echo "error 3";
        exit 1;
fi
/usr/bin/md5sum $1.i386.rpm > $1.i386.rpm.md5sum
if [ $? != 0 ];then
        echo "error 4";
        exit 1;
fi

cd /var/www/html/rpm/src/
chown root:root $1.src.rpm
if [ $? != 0 ];then
        echo "error 5";
        exit 1;
fi
/usr/bin/md5sum $1.src.rpm > $1.src.rpm.md5sum
if [ $? != 0 ];then
        echo "error 6";
        exit 1;
fi

/usr/bin/createrepo /var/www/html/rpm
if [ $? != 0 ];then
        echo "error 7";
        exit 1;
fi
