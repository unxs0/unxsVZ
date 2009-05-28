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
$cmysqlSendmailIP='baldomero.servicoopsa.com.ar';

$cunxsMailDb='mysqlmail2';
$cunxsMailLogin='mysqlmail2';
$cunxsMailPwd='wsxedc';
$cunxsMailIP='localhost';

#Please set this vars carefully
$uDefaultServerGroup=3;
$uDefaultUserType=1;
$cDefaultDomain='servicoopsa.com.ar';

#Set the variable below to 0 after setting up the other vars above ;)
$uDefaultConf=1;
#
#Config section end, do not touch below this line unless you know what you are doing

use DBI;

if($uDefaultConf eq 1) { die('I think you should not run me before configuring me'); }


my $mysqlSendMailDb=DBI->connect ("DBI:mysql:$cmysqlSendmailDb:$cmysqlSendmailIP",$cmysqlSendmailLogin,$cmysqlSendmailPwd) or die DBI->errstr;
my $unxsMailDb=DBI->connect ("DBI:mysql:$cunxsMailDb:$cunxsMailIP",$cunxsMailLogin,$cunxsMailPwd) or die DBI->errstr;

#
#Whipe out _EVERY_ table we are going to import at unxsMail db.
#Careful!

#print("Truncating unxsMail data tables...");

#
#tDomain
#$cQuery="TRUNCATE tDomain";
#$run=$unxsMailDb->prepare($cQuery);
#$run->execute() or die DBI->errstr;

#
#tUser
#$cQuery="TRUNCATE tUser";
#$run=$unxsMailDb->prepare($cQuery);
#$run->execute() or die DBI->errstr;

#
#tVUT
#$cQuery="TRUNCATE tVUT";
#$run=$unxsMailDb->prepare($cQuery);
#$run->execute() or die DBI->errstr;

#tVUTEntries
#$cQuery="TRUNCATE tVUTEntries";
#$run=$unxsMailDb->prepare($cQuery);
#$run->execute() or die DBI->errstr;

#tAccess
#$cQuery="TRUNCATE tAccess";
#$run=$unxsMailDb->prepare($cQuery);
#$run->execute() or die DBI->errstr;

#tLocal
#$cQuery="TRUNCATE tLocal";
#$run=$unxsMailDb->prepare($cQuery);
#$run->execute() or die DBI->errstr;

#print("OK\n");


#
#Import tDomain data
#

print("Importing tDomain data...");

$cQuery="SELECT uDomain,cDomain,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate FROM tDomain";

my $res=$mysqlSendMailDb->prepare($cQuery);
$res->execute();

while((@field=$res->fetchrow_array()))
{
	$cQuery="INSERT INTO tDomain SET cDomain='$field[1]',uOwner=$field[2],uCreatedBy=$field[3],uCreatedDate=$field[4],uModBy=$field[5],uModDate=$field[6]";
	$run=$unxsMailDb->prepare($cQuery);
#debug only
#	print("$cQuery\n");
	$run->execute() or die DBI->errstr;
}

print("OK\n");

#
#Import tUser data
#

print("Importing tUser data...");

$cQuery="SELECT uUser,cLogin,cEnterPasswd,uStatus,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate FROM tUser WHERE uStatus!=3 ORDER BY uUser"; 

my $res=$mysqlSendMailDb->prepare($cQuery);
$res->execute() or die DBI->errstr;

#uUser,0
#cLogin,1
#cEnterPasswd,2
#uStatus,3
#uOwner,4
#uCreatedBy,5
#uCreatedDate,6
#uModBy,7
#uModDate,8
#

while((@field=$res->fetchrow_array()))
{
	#Encode user password using MD5, we use an external software ;)
	open(CRYPTED,"./md5encrypt $field[2]|");
	$cPasswd=<CRYPTED>;
	close(CRYPTED);

	$cQuery="INSERT INTO tUser SET cLogin='$field[1]',uDomain=0,cPasswd='$cPasswd',uUserType=$uDefaultUserType,uServerGroup=$uDefaultServerGroup,uStatus=4,uOwner=$field[4],uCreatedBy=$field[5],uCreatedDate=$field[6],uModBy=$field[7],uModDate=$field[8]";
	
	$res3=$unxsMailDb->prepare($cQuery);
	$res3->execute() or die DBI->errstr;
	#print("$cQuery\n");
	
	#
	#Will figure out uDomain using tVUTEntries record
	#
	$cQuery="SELECT tVUT.cDomain FROM tVUT,tDomain,tVUTEntries,tUser WHERE tVUT.cDomain=tDomain.cDomain AND tVUT.uVUT=tVUTEntries.uVUT AND tVUTEntries.cTargetEmail=CONCAT(tUser.cLogin,'\@servicoopsa.com.ar') and tUser.cLogin='$field[1]'";
	#print("$cQuery\n");

	$res2=$mysqlSendMailDb->prepare($cQuery);
	$res2->execute() or die DBI->errstr;
	my($cDomain)=$res2->fetchrow_array();

	if($cDomain eq '') { $cDomain=$cDefaultDomain; }
	
	$cQuery="SELECT uDomain FROM tDomain WHERE cDomain='$cDomain'";
	$res4=$unxsMailDb->prepare($cQuery);
	$res4->execute() or die DBI->errstr;
	#print("$cQuery\n");

	my($uDomain)=$res4->fetchrow_array();
	
	$rv=$dbh->{mysql_insertid};

	$cQuery="UPDATE tUser SET uDomain=$uDomain WHERE uUser=$rv";
	$res5=$unxsMailDb->prepare($cQuery);
	$res5->execute() or die DBI->errstr;
	#print("$cQuery\n");
}

print("OK\n");


#
#Import tVUT and tVUTEntries data
#

print("Importing tVUT/tVUTEntries data...");

$cQuery="SELECT uVUT,cDomain,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate FROM tVUT ORDER BY uVUT";
$res=$mysqlSendMailDb->prepare($cQuery);
$res->execute() or die DBI->errstr;

while((@field=$res->fetchrow_array()))
{
	$cQuery="INSERT INTO tVUT SET uVUT=$field[0],uServerGroup=$uDefaultServerGroup,cDomain='$field[1]',uOwner=$field[2],uCreatedBy=$field[3],uCreatedDate=$field[4],uModBy=$field[5],uModDate=$field[6]";
	$run=$unxsMailDb->prepare($cQuery);
	$run->execute() or die DBI->errstr;
}

#
#tVUTEntries can be imported using mysqldump ;)
system("mysqldump -h $cmysqlSendmailIP -u $cmysqlSendmailLogin -p$cmysqlSendmailPwd $cmysqlSendmailDb tVUTEntries > /tmp/tVUTEntries.dump");
system("mysql -h $cunxsMailIP -u $cunxsMailLogin -p$cunxsMailPwd $cunxsMailDb < /tmp/tVUTEntries.dump");

print("OK\n");

#
#Import static tAccess records
#By static, we mean those records not created by qpopper pop after smtp mechaism.
#

print("Importing tAccess static data...");

$cQuery="SELECT cDomainIP,cRelayAttr FROM tAccess WHERE uExpireDate=0";
$res=$mysqlSendMailDb->prepare($cQuery);
$res->execute() or die DBI->errstr;

while((@field=$res->fetchrow_array()))
{
	$cQuery="INSERT INTO tAccess SET uServerGroup=$uDefaultServerGroup,cDomainIP='$field[0]',cRelayAttr='$field[1]',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())";
	$run=$unxsMailDb->prepare($cQuery);
	$run->execute() or die DBI->errstr;
}

print("OK\n");

#
#Import tLocal
#

print("Importing tLocal data...");

$cQuery="SELECT cDomain FROM tLocal ORDER BY uLocal";
$res=$mysqlSendMailDb->prepare($cQuery);
$res->execute() or die DBI->errstr;

while((@field=$res->fetchrow_array()))
{
	$cQuery="INSERT INTO tLocal SET uServerGroup=$uDefaultServerGroup,cDomain='$field[0]',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())";
	$run=$unxsMailDb->prepare($cQuery);
	$run->execute() or die DBI->errstr;
}

print("OK\n");

