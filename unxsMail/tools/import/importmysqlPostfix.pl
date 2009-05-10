#!/usr/bin/perl
#FILE
#	importmysqlPostfix.pl
#PURPOSE
#	Import mysqlPostfix data into mysqlMail2 database
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

$cmysqlMail2Db='mysqlmail2';
$cmysqlMail2Login='mysqlmail2';
$cmysqlMail2Pwd='wsxedc';
$cmysqlMail2IP='localhost';

$uDefaultServerGroup=1;

#Set the variable below to 0 after setting up the other vars above ;)
#$uDefaultConf=1;
#
#Config section end, do not touch below this line unless you know what you are doing

use DBI;

if($uDefaultConf eq 1) { die('I think you should not run me before configuring me'); }


my $mysqlSendMailDb=DBI->connect ("DBI:mysql:$cmysqlPostfixDb:$cmysqlPostfixIP",$cmysqlPostfixLogin,$cmysqlPostfixPwd) or die DBI->errstr;
my $mysqlMail2Db=DBI->connect ("DBI:mysql:$cmysqlMail2Db:$cmysqlMail2IP",$cmysqlMail2Login,$cmysqlMail2Pwd) or die DBI->errstr;

print("Truncating mysqlMail2 data tables...");

#
#tDomain
$cQuery="TRUNCATE tRelay";
$run=$mysqlMail2Db->prepare($cQuery);
$run->execute() or die DBI->errstr;

print("OK\n");

print("Importing tRelay data...");

$cQuery="SELECT cDomain,cTransport FROM tRelay ORDER BY uRelay";

my $res=$mysqlSendMailDb->prepare($cQuery);

$res->execute();

while((@field=$res->fetchrow_array()))
{
	$cQuery="INSERT INTO tRelay SET cDomain='$field[0]',cTransport='$field[1]',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW()),uServerGroup=$uDefaultServerGroup";
	$run=$mysqlMail2Db->prepare($cQuery);
	$run->execute() or die DBI->errstr;
}

print("OK\n");

