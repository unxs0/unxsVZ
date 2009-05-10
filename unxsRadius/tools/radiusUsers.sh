#!/bin/bash
#FILE
#	$Id$
#PURPOSE
#	This script adds, modifies and deletes
#	RADIUS users via the command line.
#AUTHOR
#	(C) 2009 Hugo Urquiza for Unixservice.
#
#

#Parse command line args
cOperation=$1;
cUser=$2;
cPasswd=$3;

function usage
{
	echo;
	echo "Usage: radiusUsers.sh operation cLogin [cPasswd] ";
	echo "		--add : Adds a new RADIUS user.";
	echo "          --mod : Modifies an existing RADIUS user password."
	echo "          --del : Deletes an existing RADIUS user.";
	echo;
}

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
	#	echo "Oops, query $cQuery failed!";
		echo "_error_";
	fi
}

function Insert_tUser
{
	cLogin=$1;
	cPwd=$2;
	uOwner=1;

	#Insert tUser record
	cQuery="INSERT INTO tUser SET cLogin='$cLogin',cEnterPasswd='$cPwd',uClearText=1,uProfileName=1,uSimulUse=1,cIP='0.0.0.0',uOwner=$uOwner,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())";
	#echo $cQuery;
	uMySQLError=`mysql_query "$cQuery"`;
	if [ "$uMySQLError" = "_error_" ]; then
		echo "Oops, query $cQuery failed!";
		exit 1;
	fi

	#This might be improved.
	cQuery="SELECT uUser FROM tUser WHERE cLogin='$cLogin'";
	#echo $cQuery;
	uUser=`mysql_query "$cQuery" | grep -v uUser`;
	if [ "$uUser" = "_error_" ]; then
		echo "Could not get tUser row id of last inserted record. Can't continue";
		exit 1;
	fi
	#
	#Will insert 3 tServerGroup records, one per cluster server.
	#Please note you may have to updte this code section.
	#
	cQuery="INSERT INTO tServerGroup SET uUser=$uUser,uServer=1";
	#echo $cQuery;
	uMySQLError=`mysql_query "$cQuery"`;
	if [ "$uMySQLError" = "_error_" ]; then
		echo "Oops, query $cQuery failed!";
		exit 1;
	fi
	cQuery="INSERT INTO tServerGroup SET uUser=$uUser,uServer=11";
	uMySQLError=`mysql_query "$cQuery"`;
	if [ "$uMySQLError" = "_error_" ]; then
		echo "Oops, query $cQuery failed!";
		exit 1;
	fi
	cQuery="INSERT INTO tServerGroup SET uUser=$uUser,uServer=21";
	uMySQLError=`mysql_query "$cQuery"`;
	if [ "$uMySQLError" = "_error_" ]; then
		echo "Oops, query $cQuery failed!";
		exit 1;
	fi

}

function Update_tUser
{
	cUser=$1;
	cPasswd=$2;
	
	cQuery="UPDATE tUser SET cEnterPasswd='$cPasswd',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE cLogin='$cUser'";
	uMySQLError=`mysql_query "$cQuery"`;
	if [ "$uMySQLError" = "_error_" ]; then
		echo "Oops, query $cQuery failed!";
		exit 1;
	fi
}

function Delete_tUser
{
	cUser=$1;

	cQuery="DELETE FROM tUser WHERE cLogin='$cUser'";
	uMySQLError=`mysql_query "$cQuery"`;
	if [ "$uMySQLError" = "_error_" ]; then
		echo "Oops, query $cQuery failed!";
		exit 1;
	fi
}


function SubmitJob
{
	cJobName=$1;
	cExtra=$2;

	cQuery="INSERT INTO tJob SET cLabel='$cJobName $cExtra',cServer='radius0',cJobName='$cJobName',uJobDate=UNIX_TIMESTAMP(NOW()),uOwner=1,uCreatedBy=1";
	uMySQLError=`mysql_query "$cQuery"`;
	if [ "$uMySQLError" = "_error_" ]; then
		echo "Oops, query $cQuery failed!";
		exit 1;
	fi
	cQuery="INSERT INTO tJob SET cLabel='$cJobName $cExtra',cServer='radius1',cJobName='$cJobName',uJobDate=UNIX_TIMESTAMP(NOW()),uOwner=1,uCreatedBy=1";
	uMySQLError=`mysql_query "$cQuery"`;
	if [ "$uMySQLError" = "_error_" ]; then
		echo "Oops, query $cQuery failed!";
		exit 1;
	fi
	cQuery="INSERT INTO tJob SET cLabel='$cJobName $cExtra',cServer='radius2',cJobName='$cJobName',uJobDate=UNIX_TIMESTAMP(NOW()),uOwner=1,uCreatedBy=1";
	uMySQLError=`mysql_query "$cQuery"`;
	if [ "$uMySQLError" = "_error_" ]; then
		echo "Oops, query $cQuery failed!";
		exit 1;
	fi
}

#
#Main program start
#

if [ "$cOperation" = "--add" ]; then
	#Add User
	echo "Add User";

	if [ "$cUser" = "" ] || [ "$cPasswd"  = "" ]; then
		echo "Missing arguments.";
		usage;
		exit 1;
	fi
	
	Insert_tUser $cUser $cPasswd;
	SubmitJob "NewUser" $cUser;

elif [ "$cOperation" = "--mod" ]; then
	#Mod User
	echo "Mod User";

	if [ "$cUser" = "" ] || [ "$cPasswd"  = "" ]; then
		echo "Missing arguments.";
		usage;
		exit 1;
	fi

	Update_tUser $cUser $cPasswd;
	SubmitJob "ModUser" $cUser;

elif [ "$cOperation" = "--del" ]; then
	#Del User
	echo "Del User";

	if [ "$cUser" = "" ]; then
		echo "Missing arguments.";
		usage;
		exit 1;
	fi

	Delete_tUser $cUser;
	SubmitJob "DelUser" $cUser;

elif [ "$cOperation" != "" ]; then
	echo "$cOperation not supported.";
	usage;
	exit 1;
elif [ "1" ]; then
	usage;
fi



