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


class unxsBindZone
{
	public $cZone='';
	public $uNSSet=1; //Default to first tNSSet record
	public $uOwner=1; //Default Root
	public $uCreatedBy=1; //Default Root
	public $uModBy=1; //Default Root
	public $cErrMsg='';
	public $uERrCode=0;
	
	public function Create()
	{
		if($this->cZone=='')
		{
			$this->uErrCode=1;
			$this->cErrMsg="Can't create a zone without defining the cZone property";
			return($this->uErrCode);
		}
		
		if($this->uNSSet==0)
		{
			$this->uErrCode=2;
			$this->cErrMsg="Can't create a zone without defining the uNSSet property";
			return($this->uErrCode);
		}

		//Check if zone exists

		if($this->ZoneExists())
		{
			$this->uErrCode=3;
			$this->cErrMsg="Zone with name $this->cZone already exists";
			return($this->uErrCode);
		}

		$uSerial=$this->SerialNum();

		$gcQuery="INSERT INTO tZone SET cZone='$this->cZone',uNSSet=1,cHostmaster='support.unixservice.com',"
			."uSerial='$uSerial',uExpire=604800,uRefresh=28800,uTTL=86400,"
			."uRetry=7200,uZoneTTL=86400,uMailServers=0,uView=2,uOwner=$this->uOwner,"
			."uCreatedBy=$this->uCreatedBy,uCreatedDate=UNIX_TIMESTAMP(NOW())";
		mysql_query($gcQuery);
		
		if(mysql_errno())
		{
			$this->uErrCode=5; 
			$this->cErrMsg=mysql_error();
			return($this->uErrCode);
		}
		
		if($this->SubmitJob("New"))
		{
			$this->cErrMsg="Could not submit job: ".$this->cErrMsg;
			return($this->uErrCode);
		}

		return(0);

	}//public function Create()


	public function Delete()
	{
		if($this->cZone=='')
		{
			$this->uErrCode=1;
			$this->cErrMsg="Can't delete a zone without defining the cZone property";
			return($this->uErrCode);
		}

		if($this->uNSSet==0)
		{
			$this->uErrCode=2;
			$this->cErrMsg="Can't delete a zone without defining the uNSSet property";
			return($this->uErrCode);
		}
		
		if(!$this->ZoneExists())
		{
			$this->uErrCode=4;
			$this->cErrMsg="Zone with name $this->cZone doesn't exist";
			return($this->uErrCode);
		}
		
		$gcQuery="DELETE FROM tResource WHERE uZone IN "
			."(SELECT uZone FROM tZone WHERE cZone='$this->cZone' AND uView=2)";
		mysql_query($gcQuery);
		
		if(mysql_errno())
		{
			$this->uErrCode=5; 
			$this->cErrMsg=mysql_error();
			return($this->uErrCode);
		}
		
		$gcQuery="DELETE FROM tZone WHERE cZone='$this->cZone' AND uView=2";
		mysql_query($gcQuery);
		
		if(mysql_errno())
		{
			$this->uErrCode=5; 
			$this->cErrMsg=mysql_error();
			return($this->uErrCode);
		}

		if($this->SubmitJob("Delete"))
		{
			$this->uErrCode=6;
			$this->cErrMsg="Could not submit iDNS job";
			return($this->uErrCode);
		}

	}//public function Delete()

	public function GetProperty($cPropName)
	{
		if($this->cZone=='')
		{
			$this->uErrCode=1;
			$this->cErrMsg="Can't delete a zone without defining the cZone property";
			return($this->uErrCode);
		}
		if($cPropName=='Hostmaster')
			return($this->Get_tZoneField("cHostmaster"));
		else if($cPropName=='NS Set')
			return($this->Get_tZoneField("uNSSet"));
		else if($cPropName=='Serial')
			return($this->Get_tZoneField("uSerial"));
		else if($cPropName=='Expire TTL')
			return($this->Get_tZoneField("uExpire"));
		else if($cPropName=='Refresh TTL')
			return($this->Get_tZoneField("uRefresh"));
		else if($cPropName=='Default TTL')
			return($this->Get_tZoneField("uTTL"));
		else if($cPropName=='Retry TTL')
			return($this->Get_tZoneField("uRetry"));
		else if($cPropName=='Zone TTL')
			return($this->Get_tZoneField("uZoneTTL"));
		else if($cPropName=='View RID')
			return($this->Get_tZoneField("uView"));
		else if($cPropName=='Registrar RID')
			return($this->Get_tZoneField("uRegistrar"));
		else if($cPropName=='Is Secondary Only')
			return($this->Get_tZoneField("uSecondaryOnly"));
		else if($cPropName=='Options')
			return($this->Get_tZoneField("cOptions"));
		else if($cPropName=='Owner RID')
			return($this->Get_tZoneField("uOwner"));
		else if($cPropName=='Created By RID')
			return($this->Get_tZoneField("uCreatedBy"));
		else if($cPropName=='Creation Date')
			return($this->Get_tZoneField("uCreatedDate"));
		else if($cPropName=='Modified By RID')
			return($this->Get_tZoneField("uModBy"));
		else if($cPropName=='Modification Date')
			return($this->Get_tZoneField("uModDate"));
	}


	private function Get_tZoneField($cFieldname)
	{
		$res=mysql_query("SELECT $cFieldname FROM tZone WHERE cZone='$this->cZone' AND uView=2");
		if(mysql_errno())
		{
			$this->uErrCode=5;
			return(NULL);
		}
		if($field=mysql_fetch_row($res))
			return($field[0]);
		else
			return(NULL);

	}//private function Get_tZoneField($cFieldname)


	public function SetProperty($cPropName,$cValue)
	{
		if($this->cZone=='')
		{
			$this->uErrCode=1;
			$this->cErrMsg="Can't set a zone property without defining the cZone property";
			return($this->uErrCode);
		}
		
		if($this->uNSSet==0)
		{
			$this->uErrCode=2;
			$this->cErrMsg="Can't set a zone property without defining the uNSSet property";
			return($this->uErrCode);
		}
	}
	
	private function ZoneExists()
	{
		$res=mysql_query("SELECT uZone FROM tZone WHERE cZone='$this->cZone' "
				."AND uView=2") or die(mysql_error());
		return(mysql_num_rows($res));

	}//private function ZoneExists()

	private function SubmitJob($cCommand)
	{
		//This function inserts a new tJob entry per each NS Set member
		$gcQuery="SELECT tNS.cFQDN,tNSType.cLabel,tServer.cLabel FROM "
			."tNS,tNSType,tServer WHERE tNSType.uNSType=tNS.uNSType "
			."AND tServer.uServer=tNS.uServer AND tNS.uNSSet=$this->uNSSet "
			."ORDER BY tNS.uNSType";
		$res=mysql_query($gcQuery);
	
		if(mysql_errno())
		{
			$this->uErrCode=5; 
			$this->cErrMsg=mysql_error();
			return($this->uErrCode);
		}

		$uTime=time();
		while(($field=mysql_fetch_row($res)))
		{
			$gcQuery="INSERT INTO tJob SET cJob='$cCommand',cZone='$this->cZone',"
				."uNSSet=$this->uNSSet,cTargetServer='$field[0] $field[1]',"
				."uTime=$uTime,cJobData='$field[2]',uCreatedBy=$this->uCreatedBy,"
				."uOwner=$this->uOwner,uCreatedDate=UNIX_TIMESTAMP(NOW())";
			mysql_query($gcQuery);
			if(mysql_errno())
			{
				$this->uErrCode=5; 
				$this->cErrMsg=mysql_error();
				return($this->uErrCode);
			}
	
			$uJob=mysql_insert_id();
			if($field[1]='MASTER')
				$uMasterJob=$uJob;
			if($uMasterJob)
			{
				$gcQuery="UPDATE tJob SET uMasterJob=$uMasterJob WHERE uJob=$uJob";
				mysql_query($gcQuery);
				if(mysql_errno())
				{
					$this->uErrCode=5; 
					$this->cErrMsg=mysql_error();
					return($this->uErrCode);
				}
			}
		}
		
		return(0);

	}//private function SubmitJob($cCommand)
	
	private function UpdateSerial()
	{
	
		$gcQuery="SELECT uSerial,uZone FROM tZone WHERE cZone='$this->cZone' AND uView=2";
		$res=mysql_query($gcQuery) or die(mysql_error());
		
		if(($field=mysql_fetch_row($res)))
		{
			$uSerial=$field[0];
			$uZone=$field[1];
		}
	
		$luYearMonDay=$this->SerialNum();

		//Typical year month day and 99 changes per day max
		//to stay in correct date format. Will still increment even if>99 changes in one day
		//but will be stuck until 1 day goes by with no changes.
		if($uSerial<$luYearMonDay)
			$gcQuery="UPDATE tZone SET uSerial=$luYearMonDay WHERE uZone=$uZone";
		else
			$gcQuery="UPDATE tZone SET uSerial=uSerial+1 WHERE uZone=$uZone";

		mysql_query($gcQuery) or die(mysql_error());
	
	}//private function UpdateSerial()

	private function SerialNum()
	{
		return(strftime("%Y%m%d00"));

	}//private function SerialNum()

}//class unxsBindZone
?>
