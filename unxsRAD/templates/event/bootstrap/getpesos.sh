#!/bin/bash
# Get dolar peso exchange rate from ambito.com
#CRON example:
#	#get pesos value
#	*/15 9,10,11,12,13,14 * * * /usr/local/sbin/getpesos.sh > /tmp/getpesos.sh.log 2>&1;


cVarPesos="Empty";

cVarPesos=`/usr/bin/wget -q -O - https://mercados.ambito.com/dolar/informal/variacion | cut -f 8 -d "\"" | sed 's/,/\./g'`;

#echo $cVarPesos;

if [ "$cVarPesos" != "Empty" ] && [ "$cVarPesos" != "" ] && [ "$cVarPesos" != " " ];then
	/usr/bin/mysql -pwsxedc -uunxsak unxsak -e "UPDATE tConfiguration SET cValue='$cVarPesos',uModDate=UNIX_TIMESTAMP(NOW()) WHERE cLabel='cVarPesos' AND $cVarPesos>0";
else
	echo "cVarPesos not set";
fi
