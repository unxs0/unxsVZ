#!/usr/bin/perl
#FILE
#       importiDNS.pl
#PURPOSE
#       Import mysqlXXX tClient data into the mysqlISP db
#AUTHOR
#       (C) 2008 Hugo Urquiza for Unixservice.
#USAGE
#       ./importClient.pl
#       and pray.
#

#
#Config section start, you must edit this as required

$ciDNSDb='idns';
$ciDNSLogin='idns';
$ciDNSPwd='wsxedc';
$ciDNSIP='localhost';

$cmysqlISP2Db='mysqlisp2';
$cmysqlISP2Login='mysqlisp2';
$cmysqlISP2Pwd='wsxedc';
$cmysqlISP2IP='localhost';


#Set the variable below to 0 after setting up the other vars above ;)
$uDefaultConf=1;
#
#Config section end, do not touch below this line unless you know what you are doing

use DBI;

if($uDefaultConf eq 1) { die('I think you should not run me before configuring me'); }


my $iDNSDb=DBI->connect ("DBI:mysql:$ciDNSDb:$ciDNSIP",$ciDNSLogin,$ciDNSPwd) or die DBI->errstr;
my $mysqlISP2Db=DBI->connect ("DBI:mysql:$cmysqlISP2Db:$cmysqlISP2IP",$cmysqlISP2Login,$cmysqlISP2Pwd) or die DBI->errstr;


$cQuery="SELECT uClient,cLabel,cEmail,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate FROM tClient";

my $res=$iDNSDb->prepare($cQuery);
$res->execute();

while((@field=$res->fetchrow_array()))
{
	$cQuery="INSERT INTO tClient SET uClient=$field[0],cLabel='$field[1]',cEmail='$field[2]',uOwner=$field[3]',uCreatedBy=$field[4],uCreatedDate=$field[5],uModBy=$field[6],uModDate=$field[7]";
#	$run=$mysqlMail2Db->prepare($cQuery);
#debug only
	print("$cQuery\n");
	$run->execute() or die DBI->errstr;
}

print("OK\n");

