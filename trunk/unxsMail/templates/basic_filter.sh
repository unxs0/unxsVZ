#!/bin/bash

#
#FILE
#	$Id: basic_filter.sh 1395 2007-08-08 21:12:04Z Gary $
#	basic_filter.sh
#PURPOSE
#	To help design basic postfix non amavis spamassassin and clamav
#	as fast as bash allows as well as rugged and featureful content_filter script
#
#AUTHORS
#	(C) 2006-2007 Gary Wallis, Hugo Urquiza for Unixservice
#NOTES
#	The dependency on pids has a wrap-around effect especially after reboot
#	This means the quarantine area must be checked daily and cleaned out.
#
#USAGE
#	Since we use clamd it must be running and postfix content_filter user
#	must be clamav so it can access the tmp files used here in the case
#	where the recipient has no local account for spamd configuration purposes.
#
#	For the local account we use joe..domain.dom.do for internal OS
#	compatible joe@domain.dom.so email address we get from postfix
#
#POSTFIX USAGE
#	master.cf example lines
#
#	Our simple bash filter script
#
#	smtp      inet  n       -       n       -       -       smtpd
#	  -o content_filter=filter:dummy
#	... normal master.cf stuff ...
#	filter    unix  -       n       n       -       10      pipe
#	  flags=Rq user=clamav argv=/var/spool/filter/basic_filter.sh -f ${sender} -- ${recipient}
#
#	Note that $4 is ${recipient} and is used below with mysqlMail2 ActiveUser
#
#INSTALL NOTES
#
#

# Localize these. The -G option does nothing before Postfix 2.3.
#dirs
HOME_DIR=/home;
INSPECT_DIR=/var/spool/filter;
QUARANTINE_DIR=/var/spool/filter/quarantine;
TRASH_DIR=/var/spool/filter/trash;
#this is for all users. But could be easily extended as above
VIRUS_DIR=/var/spool/filter/trash;

#paths to programs and program paramters
MYSQL_MAIL2="/cgi-bin/mysqlMail2.cgi";
MYSQL_MAIL2_ACTIVEUSER="/cgi-bin/mysqlMail2.cgi ActiveUser";
SENDMAIL="/usr/sbin/sendmail -G -i"; # NEVER NEVER NEVER use "-t" here.
#This is the default. It may change if you run the ActiveUser command
SPAMC_USER="clamav";
SPAMC="/usr/bin/spamc -U /var/run/spamd.sock"; #Notice sock use requires spamd setup
CLAMSCAN="/usr/bin/clamdscan";

#spamassassin point classification
#
#debug only
#QUARANTINE_SPAMLEVEL="X-Spam-Level: \*\*"; #2
#TRASH_SPAMLEVEL="X-Spam-Level: \*\*\*\*"; #4
#QUARANTINE_SPAMLEVEL="X-Spam-Level: \*\*\*\*\*\*\*\*\*\*"; #10
QUARANTINE_SPAMLEVEL="X-Spam-Level: \*\*\*\*\*\*\*"; #7
TRASH_SPAMLEVEL="X-Spam-Level: \*\*\*\*\*\*\*\*\*\*\*\*\*\*\*"; #15

# Exit codes from <sysexits.h>
EX_TEMPFAIL=75;
EX_UNAVAILABLE=69;

# Clean up when done or when aborting.
trap "rm -f in.$$ vr.$$" 0 1 2 3 15;


#Using mysqlPostfix determine if we have a local configuration for this user
#if we do run as that user
if [ -x $MYSQL_MAIL2 ]; then

        POSTFIXUSER=`$MYSQL_MAIL2_ACTIVEUSER $4`;
	ACTIVEUSER_RESULT=$?;

	case "$ACTIVEUSER_RESULT" in
	1)
		#we just continue no local configuration
		#exists
		;;
        0)
		#This is what allows us to use per email prefs
                SPAMC_USER=$POSTFIXUSER;
		;;

	*)
		exit $EX_TEMPFAIL;
		;;
        esac
fi



# Start processing.
cd $INSPECT_DIR || { echo $INSPECT_DIR does not exist; exit $EX_TEMPFAIL; };

# First run spamassassin
cat | $SPAMC -u $SPAMC_USER >in.$$ || { echo Cannot save mail; exit $EX_TEMPFAIL; };

# If spam points above this hardcoded threshold do not re-inject move to folder for
# automated deletion and possible daily review by eyeballs ;)
# 10 * or more 90%+ sure it is spam
grep "$QUARANTINE_SPAMLEVEL" in.$$  > /dev/null 2>&1;
QSL_RESULT=$?;

case "$QSL_RESULT" in
0)
	grep "$TRASH_SPAMLEVEL" in.$$  > /dev/null 2>&1;
	TSL_RESULT=$?;
	case "$TSL_RESULT" in
	0)
		#mail is removed...gone...history...bye-bye ;)

		#debug only
		#for testing we move here for now
		mv in.$$ $TRASH_DIR || { echo Cannot move spam mail; exit $EX_TEMPFAIL; };

		exit 0;
		;;
	*)
		mv in.$$ $QUARANTINE_DIR || { echo Cannot move possible-spam mail; exit $EX_TEMPFAIL; };
		exit 0;
	esac
	;;
*)

	#Message has less than QUARANTINE_SPAMLEVEL so we continue with...
	#clamav anti-virus scan
	#clamscan file
	#depending on results
	#quarantine or reinject
	#These extra args are not used in clamdscan
	#$CLAMSCAN -v -r --mbox --no-summary --stdout in.$$ > vr.$$ 2>&1
	$CLAMSCAN -v --no-summary --stdout in.$$ > vr.$$ 2>&1
	AV_RESULT=$?
	case "$AV_RESULT" in
	0)

		#re-inject into postfix
		$SENDMAIL "$@" <in.$$;
		exit $?
		;;
	1)
		#mv to quarantine area for review two parts the actual email and the report
		mv in.$$ $VIRUS_DIR/vm.$$ || { echo Cannot move possible-virus mail; exit $EX_TEMPFAIL; };
		mv vr.$$ $VIRUS_DIR;
		exit 0;
		;;
	*)
		#other clamscan errors: yikes!
		exit $EX_TEMPFAIL;
		;;
	esac
	;;
esac
