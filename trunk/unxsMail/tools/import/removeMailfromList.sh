#!/bin/bash
#FILE
#	removeMailfromList.sh
#PURPOSE
#	Given a list of email addresses this script
#	will remove them from the mail server.
#AUTHOR
#	(C) Hugo Urquiza 2009 for Unixservice.
#

while read line
do
        #As these are full email addresses, we must look thet mailbox name via VUT
        #Please note that VUT rebuild should be made semi-automatically
        #after running this script, by modifying any of the tVUTEntries records
        #at unxsMail db.
        cMbox=`grep $line /etc/mail/virtusertable | cut -d " " -f 2`;
        uResults=`grep $line /etc/mail/virtusertable | wc -l`;
        if [ "$uResults" = "1" ]; then
                /usr/sbin/userdel -r $cMbox;
        else
                echo "$line must be done by hand";
        fi

done < "maillist.txt"
	
