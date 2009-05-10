#!/usr/bin/perl
#FILE
#       userList.pl
#PURPOSE
#       List users of a given user group
#AUTHOR
#       (C) 2008 Hugo Urquiza for Unixservice.
#USAGE
#       ./userList.pl
#       and pray.
#

$cmysqlMail2Db='mysqlmail2';
$cmysqlMail2Login='mysqlmail2';
$cmysqlMail2Pwd='wsxedc';
$cmysqlMail2IP='localhost';
$uServerGroup=11;

use DBI;


my $mysqlMail2Db=DBI->connect ("DBI:mysql:$cmysqlMail2Db:$cmysqlMail2IP",$cmysqlMail2Login,$cmysqlMail2Pwd) or die DBI->errstr;

$cQuery="SELECT cLogin FROM tUser WHERE uServerGroup='$uServerGroup'";
$res=$mysqlMail2Db->prepare($cQuery);
$res->execute() or die DBI->errstr;

while((@field=$res->fetchrow_array()))
{
	print("$field[0]\n");
}

