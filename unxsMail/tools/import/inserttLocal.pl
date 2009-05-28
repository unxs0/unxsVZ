#!/usr/bin/perl
#FILE
#	inserttLocal.pl	
#PURPOSE
#	Insert tLocal records into unxsMail database
#	from tDomain data.
#AUTHOR
#	(C) 2009 Hugo Urquiza for Unixservice.
#USAGE
#	./inserttLocal.pl
#	

#
#Config section start, you must edit this as required

$cunxsMailDb='mysqlmail2';
$cunxsMailLogin='mysqlmail2';
$cunxsMailPwd='wsxedc';
$cunxsMailIP='localhost';

#Please set this vars carefully
$uDefaultServerGroup=2;

#Set the variable below to 0 after setting up the other vars above ;)
#$uDefaultConf=1;
#
#Config section end, do not touch below this line unless you know what you are doing

use DBI;

if($uDefaultConf eq 1) { die('I think you should not run me before configuring me'); }


my $unxsMailDb=DBI->connect ("DBI:mysql:$cunxsMailDb:$cunxsMailIP",$cunxsMailLogin,$cunxsMailPwd) or die DBI->errstr;


#
#Import tDomain data
#

print("Importing tDomain data...");

$cQuery="SELECT cDomain,uOwner,uCreatedBy FROM tDomain ORDER BY cDomain";

my $res=$unxsMailDb->prepare($cQuery);
$res->execute();

while((@field=$res->fetchrow_array()))
{
	$cQuery="INSERT INTO tLocal SET cDomain='$field[0]',uOwner=$field[1],uCreatedBy=$field[2],uCreatedDate=UNIX_TIMESTAMP(NOW()),uServerGroup=$uDefaultServerGroup;";
#	$run=$unxsMailDb->prepare($cQuery);
#debug only
	print("$cQuery\n");
#	$run->execute() or die $cQuery;
}

print("OK\n");

