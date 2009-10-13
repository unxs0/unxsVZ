<?php
require_once('unxsapi.php');

if(!count($_POST))
{
	$cMessage="&nbsp;";
	UI();
	return;
}
$gcFunction=$_POST['gcFunction'];
$cZone=$_POST['cZone'];
$cIP=$_POST['cIP'];

ConnectDb();

$Zone=new unxsBindZone();
$Zone->uOwner=5;
$Zone->uCreatedBy=6;
$Zone->cZone=$cZone;
if($gcFunction=='Add Zone')
{
	$Zone->Create();
	$cMessage=$Zone->cErrMsg;
	if($cMesage=='')
		$cMessage="Zone added OK";
}
/*else if($gcFunction=='Modify Zone')
{
	UpdateZone($cZone,$cIP);
	SubmitJob("Modify",1,$cZone,$uTime,$uOwner,$uCreatedBy);
	$cMessage="Zone modified OK";
}
*/
else if($gcFunction=='Delete Zone')
{
	$Zone->Delete();
	$cMessage=$Zone->cErrMsg;
	if($cMessage=='')
		$cMessage="Zone deleted OK";
}

UI();
//
//Functions section
//


function NewZone($cZone,$cIP)
{
	//This function creates a new zone
	//And a couple of RRs for it
	//E.g:
	//Zone: test.com
	//@ IN A 192.168.0.123
	//www IN CNAME test.com.
	//
	//Default uNSSet=1. See tNSSet
	//Default uView=2 (external) See tView
	$uSerial=SerialNum();
	//Both uOwner and uCreatedBy refer to the tClient row id. See tClient
	$uOwner=5;
	$uCreatedBy=6;

	$gcQuery="INSERT INTO tZone SET cZone='$cZone',uNSSet=1,cHostmaster='support.unixservice.com',"
		."uSerial='$uSerial',uExpire=604800,uRefresh=28800,uTTL=86400,"
		."uRetry=7200,uZoneTTL=86400,uMailServers=0,uView=2,uOwner=$uOwner,"
		."uCreatedBy=$uCreatedBy,uCreatedDate=UNIX_TIMESTAMP(NOW())";
	mysql_query($gcQuery) or die(mysql_error());

	$uZone=mysql_insert_id();
	
	$gcQuery="INSERT INTO tResource SET uZone=$uZone,cName='@',uRRType=1,cParam1='$cIP',".
		"uOwner=$uOwner,uCreatedBy=$uCreatedBy,uCreatedDate=UNIX_TIMESTAMP(NOW())";
	mysql_query($gcQuery) or die(mysql_error());
	
	$gcQuery="INSERT INTO tResource SET uZone=$uZone,cName='www',uRRType=5,cParam1='$cZone.'".
		"uOwner=$uOwner,uCreatedBy=$uCreatedBy,uCreatedDate=UNIX_TIMESTAMP(NOW())";

}//function NewZone()


function UpdateZone($cZone,$cIP)
{
	//This function won't change zone name, but update the zone A record
	//And increase zone serial number
	$uModBy=6;

	//Update zone serial number with a very simple mechanism.
	//Please note that only 9 changes per day allowed
	$gcQuery="UPDATE tZone SET uSerial=uSerial+1,uModBy=$uModBy,"
		."uModDate=UNIX_TIMESTAMP(NOW()) WHERE cZone='$cZone' AND uView=2";
	mysql_query($gcQuery) or die(mysql_error());

	//Update zone A record; uRRType=1. See tRRType
	$gcQuery="UPDATE tResource SET cParam1='$cIP',uModBy=$uModBy,"
		."uModDate=UNIX_TIMESTAMP(NOW()) WHERE "
		."uZone IN (SELECT uZone FROM tZone WHERE cZone='$cZone' AND uView=2) "
		."AND uRRType=1";
	mysql_query($gcQuery) or die(mysql_error());

}//function UpdateZone($cZone)


function DelZone($cZone)
{
	//This function deletes the zone RRs and then the zone record
	$gcQuery="DELETE FROM tResource WHERE uZone IN "
		."(SELECT uZone FROM tZone WHERE cZone='$cZone' AND uView=2)";
	mysql_query($gcQuery) or die(mysql_error());

	$gcQuery="DELETE FROM tZone WHERE cZone='$cZone' AND uView=2";
	mysql_query($gcQuery) or die(mysql_error());

}//function DelZone($cZone)


function ConnectDb()
{
        mysql_connect('localhost','idns','wsxedc') or die ('Could not connect:'. mysql_error());
        mysql_select_db('idns') or die ('Could not select iDNS database:'. mysql_error());
}//function ConnectDb()


function UI()
{
	global $cMessage;
?>
<html>
<body>
<form method=post>
<? echo $cMessage ?>
<table>
<tr><td>Zone name</td></td><td><input type text name=cZone></td></tr>
<tr><td>IP address</td></td><td><input type text name=cIP></td></tr>
<tr><td>&nbsp;</td></tr>
<tr><td colspan=2><input type=submit name=gcFunction value='Add Zone'> <input type=submit name=gcFunction value='Modify Zone'> <input type=submit name=gcFunction value='Delete Zone'></td></tr>
</table>
</form>
</html>
<?
}//function UI()

?>
