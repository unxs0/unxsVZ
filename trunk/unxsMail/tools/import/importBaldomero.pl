#!/usr/bin/perl
#FILE
#	importBaldomero.pl
#PURPOSE
#	Import baldomero mysqlSendmail data into unxsMail database
#AUTHOR
#	(C) 2008 Hugo Urquiza for Unixservice.
#USAGE
#	./importBaldomero.pl
#	and pray.
#	

#
#Config section start, you must edit this as required


$cunxsMailDb='mysqlmail2';
$cunxsMailLogin='mysqlmail2';
$cunxsMailPwd='wsxedc';
$cunxsMailIP='localhost';

#Please set this vars carefully
$uDefaultServerGroup=3;
$uDefaultUserType=1;
$cDefaultDomain='servicoopsa.com.ar';

#Set the variable below to 0 after setting up the other vars above ;)
$uDefaultConf=0;
#
#Config section end, do not touch below this line unless you know what you are doing

use DBI;

if($uDefaultConf eq 1) { die('I think you should not run me before configuring me'); }


my $unxsMailDb=DBI->connect ("DBI:mysql:$cunxsMailDb:$cunxsMailIP",$cunxsMailLogin,$cunxsMailPwd) or die DBI->errstr;


print("Importing data...\n");

open DATASRC, "allUsersandDomain.txt" or die $!;

#uUser,0
#cLogin,1
#cEnterPasswd,2
#uStatus,3
#uOwner,4
#uCreatedBy,5
#uCreatedDate,6
#uModBy,7
#uModDate,8
#

while(my $cLine = <DATASRC>)
{
	my($cLogin,$cPasswd,$cDomain)=split(',',$cLine);
	#print "Line reads: @cLine\n";
	#print "I got: $cLogin,$cPasswd,$cDomain\n";
	
	#Remove end \n ;)
	$cDomain=~ s/\s+$//;

	$uDomain=uGetDomain($cDomain);
	
	if($uDomain eq 0)
	{ 
		print("Could not determine uDomain value for user $cLogin. cDomain=$cDomain\n");
		next; 
	}

	$cQuery="INSERT INTO tUser SET cLogin='$cLogin',cPasswd='$cPasswd',uDomain=$uDomain,uUserType=$uDefaultUserType,uServerGroup=$uDefaultServerGroup,uStatus=4,uOwner=11,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())";
	
	$res3=$unxsMailDb->prepare($cQuery);
	$res3->execute() or die DBI->errstr;
#	print("$cQuery\n");
	
}

print("OK\n");


sub uGetDomain
{
	local($cDomain)=@_;

	$cQuery="SELECT uDomain FROM tDomain WHERE cDomain='$cDomain'";
	#print("cQuery=$cQuery\n");
	$res4=$unxsMailDb->prepare($cQuery);
	$res4->execute() or die DBI->errstr;
	#print("$cQuery\n");

	my($uDomain)=$res4->fetchrow_array();
	
#	print("uDomain=$uDomain\n");	
	if($uDomain)
	{
		return($uDomain);	
	}
	else
	{
		return(0);
	}
}
