#!/bin/bash
TO_ADDRESS="security@yourisp.com";
BCC_ADDRESS="support@unixservice.com";
FROM_ADDRESS="security@yourisp.com";
SUBJECT="IDS has unblocked $1";

echo "$1 $2 $3 $4 $5 $6 $7" >> /tmp/unxsSnortPostUndoJob.sh.log;

BODY="IDS has unblocked $1";
echo "$BODY" | /bin/mail -s "$SUBJECT" -b "$BCC_ADDRESS" -r "$FROM_ADDRESS" $TO_ADDRESS;

exit 0;
