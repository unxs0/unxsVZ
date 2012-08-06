#!/bin/bash

#PURPOSE
#	Get SSH port number and reset SSH root passwd, while storing in unxsVZ

#AUTHOR
#	(C) Gary Wallis for Unixservice, LLC 2012 GPL license applies


cHost="rc1.isp1.net";
cPasswd="wsxedc";
cGroupStatement="(tGroup.cLabel='Production PBXs' OR tGroup.cLabel='Other PBXs')";
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

if [ "$1" != "run" ];then
	echo "usage: $0 run [<VEID>]";
	exit 0;
fi

for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tContainer.uNode=$uNode AND tContainer.uStatus=1" | $cMySQLConnect | grep -v uContainer`;do

	#Optional VEID and setup
	if [ "$2" != "" ];then

		if [ "$uContainer" != "$2" ];then
			continue;
		fi
	fi

        echo $uContainer;

	cPasswd=`head -c 32 /dev/urandom | /usr/bin/md5sum | /bin/cut -c1-8`;
	if [ $? != 0 ] || [ "$cPasswd" == "" ];then
		echo "Random passwd creation failed";
		exit 1;
	fi

	cSSHPort=`/usr/sbin/vzctl exec2 $uContainer "/bin/netstat -nlp | /bin/grep ssh | /bin/cut -f 2 -d :" | awk '{print $1}'`;
	#debug only
	#echo $cSSHPort;
	#echo $cPasswd;
	#exit;
	#continue;


	#cOrg_SSHPort
	uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_SSHPort'" | $cMySQLConnect | grep -v uProperty`;
	if [ "$uProperty" == "" ];then
		echo "INSERT INTO tProperty SET cName='cOrg_SSHPort',cValue='$cSSHPort',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2 failed";
		fi
	else
		echo "UPDATE tProperty SET cValue='$cSSHPort',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uContainer AND uType=3 AND cName='cOrg_SSHPort'" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2 failed";
		fi
	fi

	#cPasswd
	uProperty=`echo "SELECT uProperty FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cPasswd'" | $cMySQLConnect | grep -v uProperty`;
	if [ "$uProperty" == "" ];then
		echo "INSERT INTO tProperty SET cName='cPasswd',cValue='$cPasswd',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2 failed";
		fi
	else
		echo "UPDATE tProperty SET cValue='$cPasswd',uModBy=1,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uKey=$uContainer AND uType=3 AND cName='cPasswd'" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2 failed";
		fi
	fi

	#debug only
	#echo $cSSHPort saved for $uContainer;
	#continue;

	/usr/sbin/vzctl set $uContainer --userpasswd root:$cPasswd > /dev/null 2>&1;
	if [ $? != 0 ];then
		echo "vzctl set --userpasswd failed";
	fi
	echo cPasswd=$cPasswd set for $uContainer;

done

