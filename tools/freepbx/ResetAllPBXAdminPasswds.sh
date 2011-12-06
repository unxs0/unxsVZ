#!/bin/bash

#FILE
#	
#PURPOSE
#	Change FreePBX admin password everyday and enter into unxsVZ system

#EXTENSIONS for 1.2 PBXs
#	Manage not only admin but new rcmadm and operator logins.
#	rcmadm requires that unxsvzOrg interface only show that to users with perm level >= 10
#	(engineering team.)
#
#MODS
#	Added support for old M5 vPBXs that are being moved into new system.
#	Added ssh root passwd change also
#
#DETAILS
#	Objective is to change the admin passwd daily and only set the operator and rcmadm 
#	passwd when the container's properties list does not have it listed.
#LEGAL
#	(C) 2011, 2012 Unixservice, LLC.
#	GPLv2 license applies

cHost="localhost";
cPasswd="wsxedc";
cGroupStatement="(tGroup.cLabel='Production PBXs' OR tGroup.cLabel='UK PBXs')";
cMySQLConnect="/usr/bin/mysql -h $cHost -u unxsvz -p$cPasswd unxsvz";

if [ "$1" != "run" ];then
	echo "usage: $0 run [<VEID>] [setup]";
	exit 0;
fi

cShortHostname=`hostname -s`;
if [ $? != 0 ];then
	echo "hostname -s failed";
	exit;
fi
uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cShortHostname'" | $cMySQLConnect | grep -v uNode`;
if [ $? != 0 ];then
	echo "Select node failed";
	exit;
fi
if [ "$uNode" == "" ];then
	echo "uNode failed";
	exit;
fi

#Do not change certain passwords unless new
cEngPasswdSet="n";
cSSHPasswdSet="n";
cOpPasswdSet="n";

for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uNode=$uNode AND tContainer.uStatus=1" | $cMySQLConnect | grep -v uContainer`;do

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

	cSSHPasswd=`head -c 32 /dev/urandom | /usr/bin/md5sum | /bin/cut -c1-8`;
	if [ $? != 0 ] || [ "$cSSHPasswd" == "" ];then
		echo "SSH random passwd creation failed";
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
			echo "mysql command 2 failed";
		fi
		echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXOperatorPasswd'" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 3 failed";
		fi
		echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_MySQLAsteriskPasswd'" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 4 failed";
		fi
		echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cPasswd'" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 4b failed";
		fi

		echo "INSERT INTO tProperty SET cName='cOrg_FreePBXAdminPasswd',cValue='$cAdminPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 5 failed";
		fi

		echo "INSERT INTO tProperty SET cName='cOrg_FreePBXEngPasswd',cValue='$cEngPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 6 failed";
		fi
		cEngPasswdSet="y";

		echo "INSERT INTO tProperty SET cName='cPasswd',cValue='$cSSHPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 6b failed";
		fi
		cSSHPasswdSet="y";

		echo "INSERT INTO tProperty SET cName='cOrg_FreePBXOperatorPasswd',cValue='$cOpPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 7 failed";
		fi
		cOpPasswdSet="y";

		echo "INSERT INTO tProperty SET cName='cOrg_MySQLAsteriskPasswd',cValue='$cAMPDBPASS',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 8 failed";
		fi
	else
		#else NOT setup section

		cCurrentAdminPwd=`echo "SELECT cValue FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXAdminPasswd'" | $cMySQLConnect | grep -v cValue`;
		if [ "$cCurrentAdminPwd" == "" ];then
			echo "INSERT INTO tProperty SET cName='cOrg_FreePBXAdminPasswd',cValue='$cAdminPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 9 failed";
			fi
		else
			echo "UPDATE tProperty SET cValue='$cAdminPasswd',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXAdminPasswd'" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 10 failed";
			fi
		fi

		cCurrentSSHPwd=`echo "SELECT cValue FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cPasswd'" | $cMySQLConnect | grep -v cValue`;
		if [ "$cCurrentSSHPwd" == "" ];then
			echo "INSERT INTO tProperty SET cName='cPasswd',cValue='$cSSHPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 11 failed";
			fi
			cSSHPasswdSet="y";
		else
			echo "UPDATE tProperty SET cValue='$cSSHPasswd',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uContainer AND uType=3 AND cName='cPasswd'" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 11b failed";
			fi
		fi

		cCurrentEngPwd=`echo "SELECT cValue FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXEngPasswd'" | $cMySQLConnect | grep -v cValue`;
		if [ "$cCurrentEngPwd" == "" ];then
			echo "INSERT INTO tProperty SET cName='cOrg_FreePBXEngPasswd',cValue='$cEngPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 11 failed";
			fi
			cEngPasswdSet="y";
		fi

		cCurrentOpPwd=`echo "SELECT cValue FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_FreePBXOperatorPasswd'" | $cMySQLConnect | grep -v cValue`;
		if [ "$cCurrentOpPwd" == "" ];then
			echo "INSERT INTO tProperty SET cName='cOrg_FreePBXOperatorPasswd',cValue='$cOpPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 12 failed";
			fi
			cOpPasswdSet="y";
		fi

		cCurrentMySQLPwd=`echo "SELECT cValue FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_MySQLAsteriskPasswd'" | $cMySQLConnect | grep -v cValue`;
		if [ "$cCurrentMySQLPwd" == "" ];then
			echo "INSERT INTO tProperty SET cName='cOrg_MySQLAsteriskPasswd',cValue='$cAMPDBPASS',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 13 failed";
			fi
		else
			echo "UPDATE tProperty SET cValue='$cAMPDBPASS',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_MySQLAsteriskPasswd'" | $cMySQLConnect;
			if [ $? != 0 ];then
				echo "mysql command 14 failed";
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

	#root ssh 
	if [ $cSSHPasswdSet == "y" ];then
		/usr/sbin/vzctl set $uContainer --userpasswd root:$cSSHPasswd;
		if [ $? != 0 ];then
			echo "vzctl set $uContainer --userpasswd root:$cSSHPasswd failed";
		fi
		echo "New root ssh passwd set $cSSHPasswd";
	elif [ "$cCurrentSSHPwd" != "" ];then
		/usr/sbin/vzctl set $uContainer --userpasswd root:$cCurrentSSHPwd;
		if [ $? != 0 ];then
			echo "vzctl set $uContainer --userpasswd root:$cCurrentSSHPwd failed";
		fi
		echo "New root ssh passwd set from unxsVZ $cCurrentSSHPwd";
	fi

done

