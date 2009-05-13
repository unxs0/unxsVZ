#!/bin/bash


#Place next line in root crontab for iDNS tHit subsystem
#*/5 * * * * /usr/sbin/bind9-genstats.sh >> /var/log/idns-cron.log 2>&1

#Configure
STAT_FILE=/usr/local/idns/named.d/named.stats;
RNDC=/usr/sbin/rndc;
THIT=/usr/sbin/tHitCollector;

#Gen named.stats
rm $STAT_FILE;
$RNDC stats;
RNDC_RET=$?;
if [ $RNDC_RET -ne 0 ]; then
	echo "Error running $RNDC:$RNDC_RET";
	exit $RNDC_RET;
fi

#Load into tHit
$THIT;
THIT_RET=$?;
if [ $THIT_RET -ne 0 ]; then
	echo "Error running $THIT:$THIT_RET";
	exit $THIT_RET;
fi

#Everything cool
exit 0;
