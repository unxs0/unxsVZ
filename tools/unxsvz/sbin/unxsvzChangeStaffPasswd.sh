#!/bin/bash

#FILE
#	unxsvzChangeStaffPasswd.sh
#PURPOSE
#	Very simple script to change /etc/shadow non root passwd.
#	To a random secure passwd and log it to tNode tProperty table.
#	Uses changestaffpasswd.c that acquires lock.
#AUTHOR
#	(C) 2011-2014 Gary Wallis for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

if [ "$2" == "" ] || [ "$1" != "changepasswd" ];then
	echo "usage: $0 <changepasswd> <user>";
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
if [ ! -f "/usr/sbin/changestaffpasswd" ];then
	fLog "no /usr/sbin/changestaffpasswd";
	exit 1;
fi

/bin/grep "^$2:" /etc/passwd > /dev/null;
if [ "$?" != "0" ];then
	fLog "no such user";
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

cNewStaffPasswd=`head -c 96 /dev/urandom | tr -cd [:alnum:] | /bin/cut -c1-16`;
if [ $? != 0 ] || [ "$cNewStaffPasswd" == "" ];then
	fLog "random passwd creation failed";
	exit 1;
fi

#fLog $cNewStaffPasswd;
#exit;

#if passwd change succeeds then we update our db
/usr/sbin/changestaffpasswd $2 $cNewStaffPasswd;
if [ $? != 0 ];then
	fLog "/usr/sbin/changestaffpasswd failed abort";
	exit;
fi
#fLog "root passwd changed";

echo "DELETE FROM tProperty WHERE uKey=$uNode AND uType=2 AND cName='cStaffPasswd-$2'" | $cMySQLConnect;
if [ $? != 0 ];then
	fLog "mysql command 1 failed";
fi

echo "INSERT INTO tProperty SET cName='cStaffPasswd-$2',cValue='$cNewStaffPasswd',uKey=$uNode,uType=2,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
if [ $? != 0 ];then
	fLog "mysql command 2 failed";
	exit;
fi

#fLog "saved in db";
fLog "Ok new passwd for $2 $cNode/$uNode";
exit;
