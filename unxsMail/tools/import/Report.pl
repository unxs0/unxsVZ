#!/usr/bin/perl
#FILE
#	Report.pl 
#PURPOSE
#	Parse a -.txt file and report which mailbox correspond to which
#	email address
#AUTHOR
#	(C) 2009 Hugo Urquiza for Unixservice.
#USAGE
#	./Report.pl

#
#Config section start, you must edit this as required

$cmysqlMail2Db='mysqlmail2';
$cmysqlMail2Login='mysqlmail2';
$cmysqlMail2Pwd='wsxedc';
$cmysqlMail2IP='196.25.27.117';

#Please set this vars carefully
$uDefaultServerGroup=2;
$cDefaultTransport="196.25.27.112";
$uServerGroupMX=1;

#Set the variable below to 0 after setting up the other vars above ;)
#$uDefaultConf=1;
#
#Config section end, do not touch below this line unless you know what you are doing

use DBI;
sub uGetDomain($);

if($uDefaultConf eq 1) { die('I think you should not run me before configuring me'); }


 srand (time ^ $$ ^ unpack "%L*", `ps axww | gzip`);

my $mysqlMail2Db=DBI->connect ("DBI:mysql:$cmysqlMail2Db:$cmysqlMail2IP",$cmysqlMail2Login,$cmysqlMail2Pwd) or die DBI->errstr;


open(DATA, "mboxes") || die("Could not open file!");

while(($cUser=<DATA>))
{
	($cOldName,$cNewName)=split(/ /,$cUser);
	#Remove \n from end of $cDomain
	$cNewName=~ s/\s+$//;
	
	#printf("Login=$cLogin, Domain)$cDomain\n");
	
	$cQuery="SELECT cVirtualEmail,(SELECT cDomain FROM tVUT WHERE tVUT.uVUT=tVUTEntries.uVUT) FROM tVUTEntries WHERE cTargetEmail='$cNewName'";
	my $res=$mysqlMail2Db->prepare($cQuery);
	$res->execute();
	if((@field=$res->fetchrow_array()))
	{
		print("$field[0]\@$field[1]: $cNewName\n");
	}
		
}

