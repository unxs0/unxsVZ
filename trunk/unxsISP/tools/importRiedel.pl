#!/usr/bin/perl
#FILE
#       importRiedel.pl
#PURPOSE
#       Import mysqlXXX tClient data into the mysqlISP db
#AUTHOR
#       (C) 2009 Hugo Urquiza for Unixservice.
#USAGE
#       ./importRiedel.pl
#       and pray.
#

#
#Config section start, you must edit this as required

$cSourceDb='db00027099';
$cSourceLogin='root';
$cSourcePwd='ZorBexin';
$cSourceIP='192.168.0.4';

$cmysqlISP2Db='mysqlisp2';
$cmysqlISP2Login='mysqlisp2';
$cmysqlISP2Pwd='wsxedc';
$cmysqlISP2IP='192.168.0.4';


#Set the variable below to 0 after setting up the other vars above ;)
#$uDefaultConf=1;
#
#Config section end, do not touch below this line unless you know what you are doing

use DBI;

if($uDefaultConf eq 1) { die('I think you should not run me before configuring me'); }


my $SourceDb=DBI->connect ("DBI:mysql:$cSourceDb:$cSourceIP",$cSourceLogin,$cSourcePwd) or die DBI->errstr;
my $mysqlISP2Db=DBI->connect ("DBI:mysql:$cmysqlISP2Db:$cmysqlISP2IP",$cmysqlISP2Login,$cmysqlISP2Pwd) or die DBI->errstr;


$cQuery="SELECT  asset_name,asset_info FROM asset WHERE asset_info LIKE 'mac%'";

my $res=$SourceDb->prepare($cQuery);
$res->execute();

while((@field=$res->fetchrow_array()))
{
	$cWirelessMAC="";
	$cWiredMAC="";
	$cLabel=$field[0];
	($cFirstName,$cLastName,$cExtra)=split(/ /,$cLabel);
	if($cExtra)
	{
		$cFirstName=$cFirstName . " " . $cLastName;
		$cLastName=$cExtra;
	}
	($a,$b,$c,$d,$e,$f,$g,$h,$i)=split(/\n/,$field[1]);
#	printf("cLabel=$cLabel cFirstName=$cFirstName cLastName=$cLastName\n");
#	printf("a=$a\n b=$b\n c=$c\n d=$d\n");
	$a=trim(lc($a));
	$b=trim(lc($b));
	$c=trim(lc($c));
	$d=trim(lc($d));
	$e=trim(lc($e));
	$f=trim(lc($f));
	$g=trim(lc($g));
	$h=trim(lc($h));
	$i=trim(lc($i));
	
	if(substr($a,0,3)=="mac")
	{
		if(index($a,":")!=-1)
		{
			if((index($a,"wlan")!=-1) || (index($a,"wireless")!=-1))
			{
				($z,$x,$cWirelessMAC)=split(/ /,$a);
			}
			else
			{
				($z,$x,$cWiredMAC)=split(/ /,$a);
			}
		}
		else
		{
			#$a it's only the header, $b has the mac
			if((index($a,"wlan")!=-1) || (index($a,"wireless")!=-1))
			{
				$cWirelessMAC=$b;
				$uMACAtB=1;
			}
			else
			{
				$cWiredMAC=$b;
				$uMACAtB=1;
			}
		}
	}

	if(substr($b,0,3)=="mac")
	{
		if(index($b,":")!=-1)
		{
			if((index($b,"wlan")!=-1) || (index($b,"wireless")!=-1))
			{
				($z,$x,$cWirelessMAC)=split(/ /,$b);
				$uMACAtB=1;
			}
			else
			{
				($z,$x,$cWiredMAC)=split(/ /,$b);
				$uMACAtB=1;
			}
		}
		else
		{
			#$b it's only the header, $c has the mac
			if((index($b,"wlan")!=-1) || (index($b,"wireless")!=-1))
			{
				$cWirelessMAC=$c;
				$uMACAtC=1;
			}
			else
			{
				$cWiredMAC=$c;
				$uMACAtC=1;
			}
		}
	}


#	printf("cWirelessMAC=$cWirelessMAC\n");
#	printf("cWiredMAC=$cWiredMAC\n");
	#printf ("user=$cLabel group=$cGroup team=$cTeam\n");
#	printf("a=$a\n b=$b\n c=$c\n d=$d\n e=$e\n f=$f\n g=$g\n h=$h\n i=$i\n");
	if($f!="function")
	{
		$cFunction=$f;
	}
	else
	{
		if($g!="function")
		{
			$cFunction=$g;
		}
		else
		{
			$cFunction=$h;
		}
	}
	if($uMACAtB && !length($d))
	{
		if($b!="function")
		{
			$cFunction=$b;
		}
		else
		{
			$cFunction=$c;
		}
	}
	
	if(length($cWirelessMAC) || length($cWiredMAC)) {
	printf("********* $uTotal ****************\n");
	printf ("first=$cFirstName\n");
	printf ("last=$cLastName\n");
	printf ("wlan mac:$cWirelessMAC\n");
	printf("a=$a\n b=$b\n c=$c\n d=$d\n e=$e\n f=$f\n g=$g\n h=$h\n i=$i\n");  
	}
#	printf("f=$f\n");

#	print("$cQuery\n");
#	$run->execute() or die DBI->errstr;
	if(!length($cWirelessMAC) && !length($cWiredMAC))
	{
#		printf("Couldn't get MAC information for $cFirstName $cLastName\n");
		$uFail++;
	}
	$uTotal++;
}

$fRatio=($uFail*100)/$uTotal;
printf("$uFail records were not imported.\n");
printf("Error ratio: $fRatio%\n");
printf("uTotal=$uTotal\n");

sub trim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}

