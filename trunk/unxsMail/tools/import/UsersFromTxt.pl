#!/usr/bin/perl
#FILE
# 	UsersFromTxt.pl	
#PURPOSE
#	Parse a -.txt file and insert tUser records
#	More interesting: This is a full import script.
#	The .txt file we are going to parse, contains a list
#	of email addresses:
#	user@domain
#	After parsing each line, we will insert tUser, tLocal,
#	tRelay,tVUT and tVUTEntries  records-
#AUTHOR
#	(C) 2009 Hugo Urquiza for Unixservice.
#USAGE
#	./UsersFromTxt.pl

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


#
#Import tDomain data
#

print("Importing tUser data...");

open(DATA, "list.txt") || die("Could not open file!");

while(($cUser=<DATA>))
{
	($cLogin,$cDomain)=split(/@/,$cUser);
	#Remove \n from end of $cDomain
	$cDomain=~ s/\s+$//;
	
	#printf("Login=$cLogin, Domain)$cDomain\n");
	$uDomain=uGetDomain($cDomain);

	if($uDomain!=-1)
	{
		#Check if a mailbox with that cLogin already exists.

		$cTargetEmail=$cLogin;
		$cVirtualEmail=$cLogin;
		
		$cQuery="SELECT uUser FROM tUser WHERE cLogin='$cLogin'";
		my $res=$mysqlMail2Db->prepare($cQuery);
		$res->execute();
		if((@field=$res->fetchrow_array()))
		{
			#If there's a tUser record with that same login, we have to 
			#change it a little bit. Then tVUTEntries will do the magic ;)
			$cTargetEmail=$cLogin . "." . generate_random_string(4);
			printf("Notice: $cLogin mailbox is now $cTargetEmail\n");
		}
		#$1$DO0Fo...$/KAal9bGTImNII4EmWGwg.
		#Default crypt() password (qazwsx)
		$cQuery="INSERT INTO tUser SET cLogin='$cTargetEmail',uDomain=$uDomain,cPasswd='\$1\$DO0Fo...\$/KAal9bGTImNII4EmWGwg.',uUserType=1,uServerGroup=$uDefaultServerGroup,uStatus=4,uOwner=35,uCreatedBy=36,uCreatedDate=UNIX_TIMESTAMP(NOW())";
		$run=$mysqlMail2Db->prepare($cQuery);
		$run->execute() or die mysql_error();
		#print("$cQuery\n");

		#Insert tVUT record if it applies
		$cQuery="SELECT uVUT FROM tVUT WHERE cDomain='$cDomain'";
		my $res=$mysqlMail2Db->prepare($cQuery);
		$res->execute();
		if((@field=$res->fetchrow_array()))
		{
			$uVUT=$field[0];
		}
		else
		{
			$cQuery="INSERT INTO tVUT SET cDomain='$cDomain',uServerGroup=$uDefaultServerGroup,uOwner=35,uCreatedBy=36,uCreatedDate=UNIX_TIMESTAMP(NOW())";
			$run=$mysqlMail2Db->prepare($cQuery);
			$run->execute() or die mysql_error();
			#print("$cQuery\n");
			$uVUT=$mysqlMail2Db->{ q{mysql_insertid}};
		}
		$cQuery="INSERT INTO tVUTEntries SET cVirtualEmail='$cVirtualEmail',uOwner=35,uCreatedBy=36,uCreatedDate=UNIX_TIMESTAMP(NOW()),cTargetEmail='$cTargetEmail',uVUT=$uVUT";
		 $run=$mysqlMail2Db->prepare($cQuery);
		 $run->execute() or die mysql_error();
		 #print("$cQuery\n");
	}
	else
	{
		printf("$cUser skipped, domain not found in tDomain\n");
	}
}
#	$cQuery="INSERT INTO tLocal SET cDomain='$field[0]',uOwner=$field[1],uCreatedBy=$field[2],uCreatedDate=UNIX_TIMESTAMP(NOW()),uServerGroup=$uDefaultServerGroup;";
#	$run=$mysqlMail2Db->prepare($cQuery);
#debug only
#	print("$cQuery\n");
#	$run->execute() or die $cQuery;
#}

print("OK\n");

sub uGetDomain($)
{
	my $cDomain=shift;

	$cQuery="SELECT uDomain FROM tDomain WHERE cDomain='$cDomain'";
	my $res=$mysqlMail2Db->prepare($cQuery); 
	$res->execute(); 
	if((@field=$res->fetchrow_array())) 
	{
		return($field[0]);
	}
	else
	{
		return(-1);
	}
}#end of sub uGetDomain($)

sub generate_random_string
{
	my $length_of_randomstring=shift;# the length of 
			 # the random string to generate

	my @chars=('a'..'z','0'..'9');
	my $random_string;
	foreach (1..$length_of_randomstring) 
	{
		# rand @chars will generate a random 
		# number between 0 and scalar @chars
		$random_string.=$chars[rand @chars];
	}
	return $random_string;
}

