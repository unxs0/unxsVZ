#!/usr/bin/perl
#FILE
# 	fixSpamHook.pl	
#PURPOSE
#	Fix Spam mailbox hook for Squirrelmail
#AUTHOR
#	(C) 2009 Hugo Urquiza for Unixservice.
#USAGE
#	./fixSpamHook.pl
#	

#
#Config section start, you must edit this as required

$cmysqlMail2Db='mysqlmail2';
$cmysqlMail2Login='mysqlmail2';
$cmysqlMail2Pwd='wsxedc';
$cmysqlMail2IP='196.25.27.117';

$uServerGroup=2;

use DBI;

my $mysqlMail2Db=DBI->connect ("DBI:mysql:$cmysqlMail2Db:$cmysqlMail2IP",$cmysqlMail2Login,$cmysqlMail2Pwd) or die DBI->errstr;


$cQuery="SELECT cLogin FROM tUser WHERE uServerGroup=$uServerGroup";

my $res=$mysqlMail2Db->prepare($cQuery);
$res->execute();

while((@field=$res->fetchrow_array()))
{
	#Add .spam mailbox default data
	printf("mv /var/mail/$field[0].spam /var/mail/$field[0].spam.0\n");
	printf("cat /tmp/default.dat /var/mail/$field[0].spam.0 > /var/mail/$field[0].spam\n");
	printf("rm -f /var/mail/$field[0].spam.0\n");

	#Fix .spam mailbox perms
	printf("chown $field[0]:mailusers /var/mail/$field[0].spam\n");

	#Remove /home/{{cLogin}}/Spam and re link
	printf("rm -f /home/$field[0]/Spam\n");
	printf("ln -s /var/mail/$field[0].spam /home/$field[0]/Spam\n");
}


