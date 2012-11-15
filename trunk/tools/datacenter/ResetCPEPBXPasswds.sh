#!/bin/bash

#PURPOSE
#	Non database change of passwords

if [ "$1" != "run" ];then
	echo "usage: $0 run <VEID>";
	exit 0;
fi

uContainer=$2;
#echo $uContainer;

cAdminPasswd=`head -c 64 /dev/urandom | tr -cd [:alnum:] | /bin/cut -c1-8`;
if [ $? != 0 ] || [ "$cAdminPasswd" == "" ];then
	echo "Admin random passwd creation failed";
	exit 1;
fi

cOpPasswd=`head -c 64 /dev/urandom | tr -cd [:alnum:] | /bin/cut -c1-8`;
if [ $? != 0 ] || [ "$cOpPasswd" == "" ];then
	echo "Op random passwd creation failed";
	exit 1;
fi

cEngPasswd=`head -c 64 /dev/urandom | tr -cd [:alnum:] | /bin/cut -c1-8`;
if [ $? != 0 ] || [ "$cEngPasswd" == "" ];then
	echo "Eng random passwd creation failed";
	exit 1;
fi

cSSHPasswd=`head -c 64 /dev/urandom | tr -cd [:alnum:] | /bin/cut -c1-8`;
if [ $? != 0 ] || [ "$cSSHPasswd" == "" ];then
	echo "SSH random passwd creation failed";
	exit 1;
fi

cAMPDBUSER=`/usr/sbin/vzctl exec2 $uContainer '/bin/grep "^AMPDBUSER=" /etc/amportal.conf | tail -n 1 | /bin/cut -f 2 -d ='`;
if [ $? != 0 ];then
	echo "vzctl exec2 1 failed";
	exit;
fi
if [ "$cAMPDBUSER" == "" ];then
	echo "cAMPDBUSER empty";
	exit;
fi

cAMPDBPASS=`/usr/sbin/vzctl exec2 $uContainer '/bin/grep "^AMPDBPASS=" /etc/amportal.conf | tail -n 1 | /bin/cut -f 2 -d ='`;
if [ $? != 0 ];then
	echo "vzctl exec2 1 failed";
	exit;
fi
if [ "$cAMPDBPASS" == "" ];then
	echo "cAMPDBPASS empty";
	exit;
fi


#Admin
/usr/sbin/vzctl exec2 $uContainer "echo 'UPDATE ampusers SET password_sha1=SHA1('\''$cAdminPasswd'\'') WHERE (username='\''root'\'' OR username='\''admin'\'') ' | /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk";
if [ $? != 0 ];then
	echo "vzctl exec2 admin failed ($cAdminPasswd) ('$cAMPDBPASS')";
	exit;
fi
#echo "New admin passwd set to $cAdminPasswd";

#Eng
/usr/sbin/vzctl exec2 $uContainer "echo 'UPDATE ampusers SET password_sha1=SHA1('\''$cEngPasswd'\'') WHERE username='\''rcmadm'\'' ' | /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk";
if [ $? != 0 ];then
	echo "vzctl exec2 eng failed ($cEngPasswd) ('$cAMPDBPASS')";
	exit;
fi
#echo "New Eng passwd set to $cEngPasswd";

#Operator
/usr/sbin/vzctl exec2 $uContainer "echo 'UPDATE ampusers SET password_sha1=SHA1('\''$cOpPasswd'\'') WHERE username='\''operator'\'' ' | /usr/bin/mysql -u $cAMPDBUSER --password=$cAMPDBPASS asterisk";
if [ $? != 0 ];then
	echo "vzctl exec2 operator failed ($cOpPasswd) ('$cAMPDBPASS')";
	exit;
fi
#echo "New Op passwd set to $cOpPasswd";

#ssh root passwd
/usr/sbin/vzctl set $uContainer --userpasswd root:$cSSHPasswd > /dev/null 2>&1;
if [ $? != 0 ];then
	echo "vzctl set $uContainer --userpasswd root:$cSSHPasswd failed";
	exit;
fi
#echo "New root ssh passwd set to $cSSHPasswd";


echo "$cAdminPasswd $cEngPasswd $cOpPasswd $cSSHPasswd";
