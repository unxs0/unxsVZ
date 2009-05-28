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

$cunxsMailDb='mysqlmail2';
$cunxsMailLogin='mysqlmail2';
$cunxsMailPwd='wsxedc';
$cunxsMailIP='localhost';
$uServerGroup=11;

use DBI;


my $unxsMailDb=DBI->connect ("DBI:mysql:$cunxsMailDb:$cunxsMailIP",$cunxsMailLogin,$cunxsMailPwd) or die DBI->errstr;

$cQuery="SELECT cLogin FROM tUser WHERE uServerGroup='$uServerGroup'";
$res=$unxsMailDb->prepare($cQuery);
$res->execute() or die DBI->errstr;

while((@field=$res->fetchrow_array()))
{
	print("$field[0]\n");
}

