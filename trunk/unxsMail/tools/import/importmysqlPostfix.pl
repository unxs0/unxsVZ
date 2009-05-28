#!/usr/bin/perl
#FILE
#	importmysqlPostfix.pl
#PURPOSE
#	Import mysqlPostfix data into unxsMail database
#AUTHOR
#	(C) 2008 Hugo Urquiza for Unixservice.
#USAGE
#	./importmysqlPostfix.pl
#	and pray.
#	

#
#Config section start, you must edit this as required

$cmysqlPostfixDb='mysqlpostfix';
$cmysqlPostfixLogin='mysqlpostfix';
$cmysqlPostfixPwd='wsxedc';
$cmysqlPostfixIP='florencio.servicoopsa.com.ar';

$cunxsMailDb='mysqlmail2';
$cunxsMailLogin='mysqlmail2';
$cunxsMailPwd='wsxedc';
$cunxsMailIP='localhost';

$uDefaultServerGroup=1;

#Set the variable below to 0 after setting up the other vars above ;)
#$uDefaultConf=1;
#
#Config section end, do not touch below this line unless you know what you are doing

use DBI;

if($uDefaultConf eq 1) { die('I think you should not run me before configuring me'); }


my $mysqlSendMailDb=DBI->connect ("DBI:mysql:$cmysqlPostfixDb:$cmysqlPostfixIP",$cmysqlPostfixLogin,$cmysqlPostfixPwd) or die DBI->errstr;
my $unxsMailDb=DBI->connect ("DBI:mysql:$cunxsMailDb:$cunxsMailIP",$cunxsMailLogin,$cunxsMailPwd) or die DBI->errstr;

print("Truncating unxsMail data tables...");

#
#tDomain
$cQuery="TRUNCATE tRelay";
$run=$unxsMailDb->prepare($cQuery);
$run->execute() or die DBI->errstr;

print("OK\n");

print("Importing tRelay data...");

$cQuery="SELECT cDomain,cTransport FROM tRelay ORDER BY uRelay";

my $res=$mysqlSendMailDb->prepare($cQuery);

$res->execute();

while((@field=$res->fetchrow_array()))
{
	$cQuery="INSERT INTO tRelay SET cDomain='$field[0]',cTransport='$field[1]',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW()),uServerGroup=$uDefaultServerGroup";
	$run=$unxsMailDb->prepare($cQuery);
	$run->execute() or die DBI->errstr;
}

print("OK\n");

