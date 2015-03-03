#!/bin/bash

#PURPOSE
#
#DETAILS
#	Objective is to change the admin passwd daily and only set the operator and rcmadm 
#	passwd when the container's properties list does not have it listed.

if [ "$1" != "run" ];then
	echo "usage: $0 run [<VEID>] [setup]";
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



cNode=`hostname -s`;
if [ $? != 0 ];then
	echo "cNode hostname error";
	exit;
fi
uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cNode'"|$cMySQLConnect|grep -v uNode`;
if [ $? != 0 ];then
	echo "uNode SELECT error";
	exit;
fi
if [ "$uNode" == "" ];then
	echo "uNode could not be determined for $cNode";
	exit;
fi

fLog "Start for $cNode/$uNode";

#Do not change certain passwords unless new
cEngPasswdSet="n";
cOpPasswdSet="n";

for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uNode=$uNode" | $cMySQLConnect | grep -v uContainer`;do

	#Optional VEID and setup
	if [ "$2" != "" ] && [ "$2" != "setup" ];then

		if [ "$uContainer" != "$2" ];then
			continue;
		fi
	fi

        echo $uContainer;

	cAdminPasswd=`head -c 32 /dev/urandom | /usr/bin/md5sum | /bin/cut -c1-8`;
	if [ $? != 0 ] || [ "$cAdminPasswd" == "" ];then
		echo "Admin random passwd creation failed";
		exit 1;
	fi

	cOpPasswd=`head -c 32 /dev/urandom | /usr/bin/md5sum | /bin/cut -c1-8`;
	if [ $? != 0 ] || [ "$cOpPasswd" == "" ];then
		echo "Op random passwd creation failed";
		exit 1;
	fi

	cEngPasswd=`head -c 32 /dev/urandom | /usr/bin/md5sum | /bin/cut -c1-8`;
	if [ $? != 0 ] || [ "$cEngPasswd" == "" ];then
		echo "Eng random passwd creation failed";
		exit 1;
	fi

	cAMPDBUSER=`/usr/sbin/vzctl exec2 $uContainer '/bin/grep "^AMPDBUSER=" /etc/amportal.conf | tail -n 1 | /bin/cut -f 2 -d ='`;
	if [ $? != 0 ];then
		echo "vzctl exec2 1 failed";
		continue;
	fi
	#echo $cAMPDBUSER;
	#exit;
	#cAMPDBUSER="asterisk";
	if [ "$cAMPDBUSER" == "" ];then
		echo "cAMPDBUSER empty";
		continue;
	fi

	cAMPDBPASS=`/usr/sbin/vzctl exec2 $uContainer '/bin/grep "^AMPDBPASS=" /etc/amportal.conf | tail -n 1 | /bin/cut -f 2 -d ='`;
	if [ $? != 0 ];then
		echo "vzctl exec2 1 failed";
		continue;
	fi
	if [ "$cAMPDBPASS" == "" ];then
		echo "cAMPDBPASS empty";
		continue;
	fi

	#debug only
	#echo "$cAMPDBPASS $cAMPDBUSER";
	#exit;

	if [ "$2" == "setup" ] || [ "$3" == "setup" ];then
		#forces cleanup
		echo "setup";

		echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXAdminPasswd'" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 1 failed";
		fi
		echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXEngPasswd'" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 1 failed";
		fi
		echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXOperatorPasswd'" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 1 failed";
		fi
		echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_MySQLAsteriskPasswd'" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2 failed";
		fi

		echo "INSERT INTO tProperty SET cName='cOrg_FreePBXAdminPasswd',cValue='$cAdminPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 1 failed";
		fi

		echo "INSERT INTO tProperty SET cName='cOrg_FreePBXEngPasswd',cValue='$cEngPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 1 failed";
		fi
		cEngPasswdSet="y";

		echo "INSERT INTO tProperty SET cName='cOrg_FreePBXOperatorPasswd',cValue='$cOpPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 1 failed";
		fi
		cOpPasswdSet="y";

		echo "INSERT INTO tProperty SET cName='cOrg_MySQLAsteriskPasswd',cValue='$cAMPDBPASS',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2 failed";
		fi
	else
		#else NOT setup section

		cCurrentAdminPwd=`echo "SELECT cValue FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXAdminPasswd'" | $cMySQLConnect | grep -v cValue`;
		if [ "$cCurrentAdminPwd" == "" ];then
			echo "INSERT INTO tProperty SET cName='cOrg_FreePBXAdminPasswd',cValue='$cAdminPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 2 failed";
			fi
		else
			echo "UPDATE tProperty SET cValue='$cAdminPasswd',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXAdminPasswd'" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 2 failed";
			fi
		fi

		cCurrentEngPwd=`echo "SELECT cValue FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXEngPasswd'" | $cMySQLConnect | grep -v cValue`;
		if [ "$cCurrentEngPwd" == "" ];then
			echo "INSERT INTO tProperty SET cName='cOrg_FreePBXEngPasswd',cValue='$cEngPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 2 failed";
			fi
			cEngPasswdSet="y";
		fi

		cCurrentOpPwd=`echo "SELECT cValue FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXOperatorPasswd'" | $cMySQLConnect | grep -v cValue`;
		if [ "$cCurrentOpPwd" == "" ];then
			echo "INSERT INTO tProperty SET cName='cOrg_FreePBXOperatorPasswd',cValue='$cOpPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 2 failed";
			fi
			cOpPasswdSet="y";
		fi

		cCurrentMySQLPwd=`echo "SELECT cValue FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_MySQLAsteriskPasswd'" | $cMySQLConnect | grep -v cValue`;
		if [ "$cCurrentMySQLPwd" == "" ];then
			echo "INSERT INTO tProperty SET cName='cOrg_MySQLAsteriskPasswd',cValue='$cAMPDBPASS',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 2 failed";
			fi
		else
			echo "UPDATE tProperty SET cValue='$cAMPDBPASS',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_MySQLAsteriskPasswd'" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 2 failed";
			fi
		fi
	fi

	#After no errors we update the container passwd

	#Admin
	/usr/sbin/vzctl exec2 $uContainer "echo 'UPDATE ampusers SET password_sha1=SHA1('\''$cAdminPasswd'\'') WHERE (username='\''root'\'' OR username='\''admin'\'') ' | /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk";
	if [ $? != 0 ];then
		#Old m5 pbx support
		/usr/sbin/vzctl exec2 $uContainer "echo 'UPDATE ampusers SET password_sha1=SHA1('\''$cAdminPasswd'\'') WHERE (username='\''root'\'' OR username='\''admin'\'') ' | /usr/bin/mysql -u asteriskuser --password=$cAMPDBPASS asterisk";
		if [ $? != 0 ];then
			#very old no SHA1 support containers
			/usr/sbin/vzctl exec2 $uContainer "echo 'UPDATE ampusers SET password='\''$cAdminPasswd'\'' WHERE (username='\''root'\'' OR username='\''admin'\'') ' | /usr/bin/mysql -u asteriskuser --password=$cAMPDBPASS asterisk";
			if [ $? != 0 ];then
				echo "vzctl exec2 2 admin failed three times ($cAdminPasswd) ('$cAMPDBPASS')";
			fi
		fi
	fi
	echo "New admin passwd set";

	#Eng
	if [ $cEngPasswdSet == "y" ];then
		/usr/sbin/vzctl exec2 $uContainer "echo 'UPDATE ampusers SET password_sha1=SHA1('\''$cEngPasswd'\'') WHERE username='\''rcmadm'\'' ' | /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk";
		if [ $? != 0 ];then
			echo "vzctl exec2 2 eng failed ($cEngPasswd) ('$cAMPDBPASS')";
		fi
		echo "New Eng passwd set";
	elif [ "$cCurrentEngPwd" != "" ];then
		/usr/sbin/vzctl exec2 $uContainer "echo 'UPDATE ampusers SET password_sha1=SHA1('\''$cCurrentEngPwd'\'') WHERE username='\''rcmadm'\'' ' | /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk";
		if [ $? != 0 ];then
			echo "vzctl exec2 2 eng failed ($cCurrentEngPwd) ('$cAMPDBPASS')";
		fi
		echo "Eng passwd set from unxsVZ";
	fi

	#Operator
	if [ $cOpPasswdSet == "y" ];then
		/usr/sbin/vzctl exec2 $uContainer "echo 'UPDATE ampusers SET password_sha1=SHA1('\''$cOpPasswd'\'') WHERE username='\''operator'\'' ' | /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk";
		if [ $? != 0 ];then
			echo "vzctl exec2 2 op failed ($cOpPasswd) ('$cAMPDBPASS')";
		fi
		echo "New Op passwd set";
	elif [ "$cCurrentOpPwd" != "" ];then
		/usr/sbin/vzctl exec2 $uContainer "echo 'UPDATE ampusers SET password_sha1=SHA1('\''$cCurrentOpPwd'\'') WHERE username='\''operator'\'' ' | /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk";
		if [ $? != 0 ];then
			echo "vzctl exec2 2 op failed ($cCurrentOpPwd) ('$cAMPDBPASS')";
		fi
		echo "Op passwd set from unxsVZ";
	fi

done

