#!/bin/bash
#FILE
#	testjobqueue.sh
#PURPOSE
#	This script will create a new tResource record at the 
#	zone which is deployed by the installer.
#	Once that done it will create a new tJob entry 
#	for testing iDNS job queue processing.
#	Lastly will use dig to verify NS data.
#AUTHOR
#	(C) 2008-2009 Hugo Urquiza for Unixservice.
#

DBIP=`grep DBIP0 local.h | cut -d " " -f 3 | cut -d \" -f 2`;
DBNAME=`grep DBNAME local.h | cut -d " " -f 3 | cut -d \" -f 2`;
DBLOGIN=`grep DBLOGIN local.h | cut -d " " -f 3 | cut -d \" -f 2`;
DBPASSWD=`grep DBPASSWD local.h | cut -d " " -f 3 | cut -d \" -f 2`;

#Insert tResource record
cQuery="INSERT INTO tResource SET uZone=1,cName='@',uRRType=3,cParam1='10',cParam2='mail.yourdomain.com.',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW());";

echo "Inserting tResource record...";

if [ "$DBIP" != "NULL" ]; then
        echo $cQuery | mysql -h$DBIP -u $DBLOGIN -p$DBPASSWD $DBNAME > /dev/null 2>&1;
else
        echo $cQuery | mysql -u $DBLOGIN -p$DBPASSWD $DBNAME > /dev/null 2>&1;
fi

if [ "$?" != "0" ]; then
	echo "Error: could not insert tResource record";
	exit 1;
fi

echo "tResource record insert: OK";

#Insert tJob record
cQuery="INSERT INTO tJob SET uTime=UNIX_TIMESTAMP(NOW()),cZone='ns1.yourdomain.com',uNSSet=1,cJob='Modify',uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW()),cTargetServer='ns1.yourdomain.com MASTER',uMasterJob=1;"

echo "Inserting tJob record...";

if [ "$DBIP" != "NULL" ]; then
	echo $cQuery | mysql -h$DBIP -u $DBLOGIN -p$DBPASSWD $DBNAME > /dev/null 2>&1;
else
	echo $cQuery | mysql -u $DBLOGIN -p$DBPASSWD $DBNAME > /dev/null 2>&1;
fi

if [ "$?" != "0" ]; then
	echo "Error: could not insert tJob record";
	exit 1;
fi

echo "tJob record insert: OK";

#
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
        echo "Unable to locate your cgi-bin directory. Please specify it:";
	read CGIDIR;
    fi
fi

echo "Processing iDNS job queue...";
#Process job queue
$CGIDIR"iDNS.cgi" ProcessJobQueue ns1.yourdomain.com > /dev/null 2>&1;

if [ "$?" = "0" ]; then
	echo "Job queue processing: OK";
else
	echo "Job queue processing failed. Contact support";
fi

echo "Testing with dig...";

cDig=`which dig`;
cDigOutPut=`$cDig @localhost mx ns1.yourdomain.com +short`;

if [ "$?" != "0" ]; then
	echo "Error: dig comand failed. Test aborted.";
	exit 1;
fi

if [ "$cDigOutPut" != "10 mail.yourdomain.com." ]; then
	echo "Error: job queue processing didn't add the test RR";
else
	echo "The test RR was added OK. System is working correctly";
fi

