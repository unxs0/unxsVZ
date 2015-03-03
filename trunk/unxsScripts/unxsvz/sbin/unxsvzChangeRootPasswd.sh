#!/bin/bash

#FILE
#	unxsvzChangeRootPasswd.sh
#PURPOSE
#	Very simple script to change /etc/shadow root passwd.
#	To a random secure passwd and log it to tNode tProperty table.
#	Uses changerootpasswd.c that acquires lock.
#AUTHOR
#	(C) 2011-2014 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

if [ "$1" == "" ];then
	echo "usage: $0 <changepasswd>";
	exit 0;
fi

cMySQLConnect="";
if [ -f "/etc/unxsvz/mysql.local.sh" ];then
	source /etc/unxsvz/mysql.local.sh;
else
	echo "no /etc/unxsvz/mysql.local.sh";
	exit 1;
fi
if [ -f "/etc/unxsvz/unxsvz.functions.sh" ];then
	source /etc/unxsvz/unxsvz.functions.sh;
else
	echo "no /etc/unxsvz/unxsvz.functions.sh";
	exit 1;
fi
if [ "$cMySQLConnect" == "" ];then
	fLog "no cMySQLConnect";
	exit 1;
fi
if [ ! -f "/usr/sbin/changerootpasswd" ];then
	fLog "no /usr/sbin/changerootpasswd";
	exit 1;
fi

cNode=`hostname -s`;
if [ $? != 0 ];then
	fLog "cNode hostname error";
	exit;
fi
uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cNode'"|$cMySQLConnect|grep -v uNode`;
if [ $? != 0 ];then
	fLog "uNode SELECT error";
	exit;
fi
if [ "$uNode" == "" ];then
	fLog "uNode could not be determined for $cNode";
	exit;
fi
fLog "Start for $cNode/$uNode";

#cNewRootPasswd=`head -c 32 /dev/urandom | /usr/bin/md5sum | /bin/cut -c1-16`;
cNewRootPasswd=`head -c 96 /dev/urandom | tr -cd [:alnum:] | /bin/cut -c1-16`;
if [ $? != 0 ] || [ "$cNewRootPasswd" == "" ];then
	fLog "random passwd creation failed";
	exit 1;
fi

#fLog $cNewRootPasswd;
#exit;

#if passwd change succeeds then we update our db
/usr/sbin/changerootpasswd $cNewRootPasswd;
if [ $? != 0 ];then
	fLog "/usr/sbin/changerootpasswd failed abort";
	exit;
fi
#fLog "root passwd changed";

echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cRootPasswd'" | $cMySQLConnect;
if [ $? != 0 ];then
	fLog "mysql command 1 failed";
fi

echo "INSERT INTO tProperty SET cName='cRootPasswd',cValue='$cNewRootPasswd',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	fLog "mysql command 2 failed";
	exit;
fi

#fLog "saved in db";
fLog "Ok new passwd for $cNode/$uNode";
exit;
