#!/bin/bash

#FILE
#	/usr/sbin/unxsvzGetPBXTrunkInfo.sh
#PURPOSE
#	FreePBX family script
#	Gather trunk info including trunk sequence for outbound routes.
#	Update or insert appropiate tProperty entries accordingly.
#NOTES
#	Requires pre 2.9 FreePBX 
#AUTHOR
#	(C) 2011-2014 Ricardo Armas for Unixservice, LLC.
#	GPLv3 license applies see root dir LICENSE

if [ "$1" != "run" ];then
	echo "usage: $0 run <uVEID>";
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
	fLog "cNode hostname error";
	exit;
fi
uNode=`echo "SELECT uNode FROM tNode WHERE cLabel='$cNode'"|$cMySQLConnect -B -N`;
if [ $? != 0 ];then
	fLog "uNode SELECT error";
	exit;
fi
if [ "$uNode" == "" ];then
	fLog "uNode could not be determined for $cNode";
	exit;
fi
fLog "Start for $cNode/$uNode";


#echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue,tOSTemplate WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tOSTemplate.uOSTemplate=tContainer.uOSTemplate AND (tOSTemplate.uOSTemplate<751 OR tOSTemplate.cLabel LIKE 'vcpbx1%') AND tContainer.uNode=$uNode AND tContainer.uStatus=1";

for uContainer in `echo "SELECT tContainer.uContainer FROM tContainer,tGroup,tGroupGlue,tOSTemplate WHERE $cGroupStatement AND tGroup.uGroup=tGroupGlue.uGroup AND tContainer.uContainer=tGroupGlue.uContainer AND tOSTemplate.uOSTemplate=tContainer.uOSTemplate AND tOSTemplate.cLabel LIKE 'vcpbx1%' AND tContainer.uNode=$uNode AND tContainer.uStatus=1"|$cMySQLConnect -B -N`;do

	#single veid option
	if [ "$2" != "" ];then
		if [ "$2" != "$uContainer" ];then
			continue;
		fi
	fi

	echo $uContainer;

	#all used trunks where they are used and in what order
	#instead of select and update or insert we just clean out and add again.
	echo "DELETE FROM tProperty WHERE uKey=$uContainer AND uType=3 AND cName='cPBXTrunkSeqAndRoute'" | $cMySQLConnect;
	if [ $? != 0 ];then
		echo "mysql command 1 failed";
	fi
        for cResult in `/usr/sbin/vzctl exec2 $uContainer "/usr/bin/mysql -u vcxrfv -ppK69rLuvC0 asterisk -N -e 'select distinct concat(t.name,'\'':'\'',e.priority,'\'':'\'',e.context) from trunks t left join extensions e on e.args=concat('\''dialout-trunk,'\'',t.trunkid,'\'',\\${EXTEN},'\'') WHERE t.name='\''KAM1-SIP'\'' or t.trunkid=1;'"`;do
		echo $cResult;
		echo "INSERT INTO tProperty SET cName='cPBXTrunkSeqAndRoute',cValue='$cResult',uKey=$uContainer,uType=3,uOwner=2,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())" | $cMySQLConnect;
		if [ $? != 0 ];then
			echo "mysql command 2 failed";
		fi
	done
done
fLog "End";
