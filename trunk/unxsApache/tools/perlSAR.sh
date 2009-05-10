#!/bin/sh
#
#FILE
#	perlSAR.sh
#	$Id: perlSAR.sh 1294 2007-07-09 23:40:10Z Gary $
#PURPOSE
#	Fast/flexible/safe mass auto editing.
#AUTHOR
# (C) 2006-2007 Gary Wallis
#

#patch 1
for file in $( ls -1 mainfunc.h ); do

	#file_check=`grep -l "perlSAR patch1" $file`;

	#if [ "$file_check" = "$fileXXX"  ]; then
	#	echo NOT editing already edited file: $file ...
	#else
		echo editing $file ...

		#mysqlApache2(mysql_error(&gMysql))
		#replace with
		#htmlPlainTextError(mysql_error(&gMysql))
		#/usr/bin/perl -pi -e "s/\{\{cProject\}\}\(mysql_error\(\&gMysql\)\)/htmlPlainTextError\(mysql_error\(\&gMysql\)\)/g" $file
		/usr/bin/perl -pi -e "s/mysqlApache2\(mysql_error\(\&gMysql\)\)/htmlPlainTextError\(mysql_error\(\&gMysql\)\)/g" $file
		#echo "//perlSAR patch1" >> $file
	#fi
done
exit;

#not used example only for serial controlled changes
#patch 2
for file in $( ls -1 bind.c ); do

	file_check=`grep -l "perlSAR patch2" $file`

	if [ "$file_check" = "$file"  ]; then

		echo NOT editing already edited file: $file ...

	else

		echo editing $file ...

		#/usr/bin/perl -pi -e "s/query,/gcQuery,/g" $file
		#echo "//perlSAR patch2" >> $file

	fi
done
