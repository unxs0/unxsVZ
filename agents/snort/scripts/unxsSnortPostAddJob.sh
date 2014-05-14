#!/bin/bash
TO_ADDRESS="security@yourisp.com";
BCC_ADDRESS="support@unixservice.com";
FROM_ADDRESS="security@yourisp.com";
SUBJECT="IDS proposes that $1 be blocked";

echo "$1 $2" >> /tmp/unxsSnortPostAddJob.sh.log;

if [ -f "$2" ] && [ -s "$2" ];then
	cat $2 | /bin/mail -s "$SUBJECT" -b "$BCC_ADDRESS" -r "$FROM_ADDRESS" $TO_ADDRESS;
else
	BODY="IDS proposes that $1 be blocked";
	echo "$BODY" | /bin/mail -s "$SUBJECT" -b "$BCC_ADDRESS" -r "$FROM_ADDRESS" $TO_ADDRESS;
fi

exit 0;
