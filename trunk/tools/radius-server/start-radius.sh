#!/bin/bash

#Customize these for your setup!
CGIDIR='/usr/local/bin/';
RADIUS_SERVER='radius0';

$CGIDIR/mysqlRadius2.cgi Start $RADIUS_SERVER;

