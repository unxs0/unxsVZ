#!/usr/bin/perl
#FILE
#	updateContainer.pl
#PURPOSE
#	Massively rename cloned VEs to 'production' name
#	and submit required unxsVZ job.
#AUTHOR
#	(C) 2010 Hugo Urquiza for Unixservice, LLC.
#

use DBI;
my $Dbh=DBI->connect("DBI:mysql:unxsvz:localhost","unxsvz","wsxedc") or die DBI->errstr;

my $cQuery="SELECT uContainer,cHostname,uNode,uDatacenter FROM tContainer WHERE cHostname LIKE '%clone%'";
	print("$cQuery\n");
my $res=$Dbh->prepare($cQuery);
$res->execute();

while(($uContainer,$cHostname,$uNode,$uDatacenter)=$res->fetchrow_array)
{
	$cHostname=~s/.clone2//;

	$cQuery="UPDATE tContainer SET cHostname='$cHostname' WHERE uContainer=$uContainer";
	#print("$cQuery\n");
	$Dbh->do($cQuery);

	$cQuery="INSERT INTO tJob SET cLabel='ChangeHostnameContainer($uContainer)',cJobName='ChangeHostnameContainer',uDatacenter=$uDatacenter,uNode=$uNode,uContainer=$uContainer,uJobDate=UNIX_TIMESTAMP(NOW())+60,uJobStatus=1,uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())";
	#print("$cQuery\n");
	$Dbh->do($cQuery);
}
