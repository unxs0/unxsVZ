#!/bin/bash

function mysql_query
{
        cDbIP='192.168.0.4';
        cDbName='mysqlradius2';
        cDbLogin='mysqlradius2';
        cDBPwd='wsxedc';

        cQuery=$1;
        if [ "$cQuery" = "" ]; then return; fi

        echo "$cQuery" | mysql -h $cDbIP -u $cDbLogin -p$cDBPwd $cDbName;
        if [ "$?" != "0" ]; then
        #       echo "Oops, query $cQuery failed!";
                echo "_error_";
        fi
}

PATH="/sbin:/usr/local/bin:/bin:/usr/bin:/usr/local/sbin:/usr/sbin"
export PATH

echo "HTTP/1.0 200 Transaction ok"
echo "Content-Type: text/plain"
echo ""
echo ""

if [ $2 .eq. ""]; then
	echo "usage: nph-userTest.cgi?login+passwd";
	exit;
fi

if [ $1 .eq. ""]; then
	echo "usage: nph-userTest.cgi?login+passwd"
	exit
fi

echo "Login: $1"
echo "Password: $2"

echo
echo
echo "____________________________________________________________________________"
echo
echo

echo "Testing database existance..."

cQuery="SELECT uUser FROM tUser WHERE cLogin='$1'";
#echo $cQuery;
uUser=`mysql_query "$cQuery" | grep -v uUser`;

if [ "$uUser" = "_error_" ]; then
        echo "Query failed";
        exit 1;
fi

echo "uUser=$uUser";

echo
echo
echo "____________________________________________________________________________"
echo
echo

echo "Testing RADIUS auth..."

cRADIUSServer0='192.168.0.10';
cRADIUSPort0=1812;

cRADIUSServer1='192.168.0.11';
cRADIUSPort1=1812;

echo "radtest against $cRADIUSServer0:$cRADIUSPort0";
echo radtest $1 $2 $cRADIUSServer0:$cRADIUSPort0 0 testing123
radtest $1 $2 $cRADIUSServer0:$cRADIUSPort0 0 testing123 2>&1
echo
echo
echo "____________________________________________________________________________"
echo
echo
echo "radtest against $cRADIUSServer1:$cRADIUSPort1";
echo radtest $1 $2 $cRADIUSServer1:$cRADIUSPort1 0 testing123
radtest $1 $2 $cRADIUSServer1:$cRADIUSPort1 0 testing123 2>&1


