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

$cunxsMailDb='mysqlmail2';
$cunxsMailLogin='mysqlmail2';
$cunxsMailPwd='wsxedc';
$cunxsMailIP='196.25.27.117';

$uServerGroup=2;

use DBI;

my $unxsMailDb=DBI->connect ("DBI:mysql:$cunxsMailDb:$cunxsMailIP",$cunxsMailLogin,$cunxsMailPwd) or die DBI->errstr;


$cQuery="SELECT cLogin FROM tUser WHERE uServerGroup=$uServerGroup";

my $res=$unxsMailDb->prepare($cQuery);
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


