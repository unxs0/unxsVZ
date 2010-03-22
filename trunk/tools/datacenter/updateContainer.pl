#!/usr/bin/perl
#FILE
#	updateContainer.pl
#PURPOSE
#	Massively rename cloned VEs to 'production' name
#	and submit required unxsVZ job.
#AUTHOR/LEGAL
#	(C) 2010 Hugo Urquiza and Gary Wallis for Unixservice, LLC.
#	GPLv2 license applies. See LICENSE file included in source dir.
#NOTES
#	This perl program is a simple example of using the unxsVZ jobqueue.c as
#	an engine API.
#

use DBI;
my $Dbh=DBI->connect("DBI:mysql:unxsvz:localhost","unxsvz","wsxedc") or die DBI->errstr;

my $cQuery="SELECT uContainer,cHostname,uNode,uDatacenter FROM tContainer WHERE cHostname LIKE '%clone%'";
	#debug only
	#print("$cQuery\n");
my $res=$Dbh->prepare($cQuery);
$res->execute();

while(($uContainer,$cHostname,$uNode,$uDatacenter)=$res->fetchrow_array)
{
	#We have two cases they can be sequential.
	$cHostname=~s/.clone2//;
	$cHostname=~s/.clone1//;

	$cQuery="UPDATE tContainer SET cHostname='$cHostname' WHERE uContainer=$uContainer";
	#debug only
	#print("$cQuery\n");
	$Dbh->do($cQuery);

	$cQuery="INSERT INTO tJob SET cLabel='ChangeHostnameContainer($uContainer)',cJobName='ChangeHostnameContainer',uDatacenter=$uDatacenter,uNode=$uNode,uContainer=$uContainer,uJobDate=UNIX_TIMESTAMP(NOW())+60,uJobStatus=1,uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())";
	#debug only
	#print("$cQuery\n");
	$Dbh->do($cQuery);
}

exit(0);
