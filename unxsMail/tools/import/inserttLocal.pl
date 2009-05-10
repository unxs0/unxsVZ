#!/usr/bin/perl
#FILE
#	inserttLocal.pl	
#PURPOSE
#	Insert tLocal records into mysqlMail2 database
#	from tDomain data.
#AUTHOR
#	(C) 2009 Hugo Urquiza for Unixservice.
#USAGE
#	./inserttLocal.pl
#	

#
#Config section start, you must edit this as required

$cmysqlMail2Db='mysqlmail2';
$cmysqlMail2Login='mysqlmail2';
$cmysqlMail2Pwd='wsxedc';
$cmysqlMail2IP='localhost';

#Please set this vars carefully
$uDefaultServerGroup=2;

#Set the variable below to 0 after setting up the other vars above ;)
#$uDefaultConf=1;
#
#Config section end, do not touch below this line unless you know what you are doing

use DBI;

if($uDefaultConf eq 1) { die('I think you should not run me before configuring me'); }


my $mysqlMail2Db=DBI->connect ("DBI:mysql:$cmysqlMail2Db:$cmysqlMail2IP",$cmysqlMail2Login,$cmysqlMail2Pwd) or die DBI->errstr;


#
#Import tDomain data
#

print("Importing tDomain data...");

$cQuery="SELECT cDomain,uOwner,uCreatedBy FROM tDomain ORDER BY cDomain";

my $res=$mysqlMail2Db->prepare($cQuery);
$res->execute();

while((@field=$res->fetchrow_array()))
{
	$cQuery="INSERT INTO tLocal SET cDomain='$field[0]',uOwner=$field[1],uCreatedBy=$field[2],uCreatedDate=UNIX_TIMESTAMP(NOW()),uServerGroup=$uDefaultServerGroup;";
#	$run=$mysqlMail2Db->prepare($cQuery);
#debug only
	print("$cQuery\n");
#	$run->execute() or die $cQuery;
}

print("OK\n");

