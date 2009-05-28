#!/usr/bin/perl
#FILE
#	importmysqlSendmail.pl
#PURPOSE
#	Import mysqlSendmail data into unxsMail database
#AUTHOR
#	(C) 2008 Hugo Urquiza for Unixservice.
#USAGE
#	./importmysqlSendmail.pl
#	and pray.
#	

#
#Config section start, you must edit this as required

$cmysqlSendmailDb='mysqlsendmail';
$cmysqlSendmailLogin='mysqlsendmail';
$cmysqlSendmailPwd='wsxedc';
$cmysqlSendmailIP='localhost';

use DBI;


my $mysqlSendMailDb=DBI->connect ("DBI:mysql:$cmysqlSendmailDb:$cmysqlSendmailIP",$cmysqlSendmailLogin,$cmysqlSendmailPwd) or die DBI->errstr;

$cQuery="SELECT cLogin,cEnterPasswd FROM tUser WHERE uServer=1";

my $res=$mysqlSendMailDb->prepare($cQuery);
$res->execute();

while((@field=$res->fetchrow_array()))
{
	print("$field[0],$field[1]\n");
}


