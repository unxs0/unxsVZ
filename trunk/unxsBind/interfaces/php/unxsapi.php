<?
/*
FILE
	unxsapi.php
PURPOSE
	Provide the required PHP functions
	to interact with the iDNS database
AUTHOR
	(C) 2009 Hugo Urquiza for Unixservice
*/

function SubmitJob($cCommand,$uNSSet,$cZone,$uTime,$uOwner,$uCreatedBy)
{
	//This function inserts a new tJob entry per each NS Set member
	$gcQuery="SELECT tNS.cFQDN,tNSType.cLabel,tServer.cLabel FROM "
		."tNS,tNSType,tServer WHERE tNSType.uNSType=tNS.uNSType "
		."AND tServer.uServer=tNS.uServer AND tNS.uNSSet=$uNSSet "
		."ORDER BY tNS.uNSType";
	$res=mysql_query($gcQuery) or die(mysql_error());

	while(($field=mysql_fetch_row($res)))
	{
		$gcQuery="INSERT INTO tJob SET cJob='$cCommand',cZone='$cZone',"
			."uNSSet=$uNSSet,cTargetServer='$field[0] $field[1]',"
			."uTime=$uTime,cJobData='$field[2]',uCreatedBy=$uCreatedBy,"
			."uOwner=$uOwner,uCreatedDate=UNIX_TIMESTAMP(NOW())";
		mysql_query($gcQuery) or die(mysql_error());

		$uJob=mysql_insert_id();
		if($field[1]='MASTER')
			$uMasterJob=$uJob;
		if($uMasterJob)
		{
			$gcQuery="UPDATE tJob SET uMasterJob=$uMasterJob WHERE uJob=$uJob";
			mysql_query($gcQuery) or die(mysql_error());
		}
	}

}//function SubmitJob($cCommand,$uNSSet,$cZone,$uTime)

function UpdateSerial($uZone)
{
	$gcQuery="SELECT uSerial FROM tZone WHERE uZone=$uZone";
	$res=mysql_query($gcQuery) or die(mysql_error());
	
	if(($field=mysql_fetch_row($res)))
		$uSerial=$field[0];
	
	$luYearMonDay=SerialNum();

	//Typical year month day and 99 changes per day max
	//to stay in correct date format. Will still increment even if>99 changes in one day
	//but will be stuck until 1 day goes by with no changes.
	if($uSerial<$luYearMonDay)
		$gcQuery="UPDATE tZone SET uSerial=$luYearMonDay WHERE uZone=$uZone";
	else
		$gcQuery="UPDATE tZone SET uSerial=uSerial+1 WHERE uZone=$uZone";

	mysql_query($gcQuery) or die(mysql_error());

}//function UpdateSerial($uZone)

function SerialNum()
{
	return(strftime("%Y%m%d00"));

}//function SerialNum()

?>
