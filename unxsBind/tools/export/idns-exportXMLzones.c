/*
FILE 
	tools/export/idns-exportXMLzones.c
	$Id$
PURPOSE
	Standalone XML zone export tool
EXAMPLE OUTPUT

	<zone name="abc.com">
	<record type="A" owner="abc.com." class="IN" ttl="86400" address="1.2.3.4"/>
	<record type="A" owner="mail.abc.com." class="IN" ttl="86400" address="1.2.3.5"/>
	<record type="MX" owner="abc.com." class="IN" ttl="86400" preference="10" exchange="mail.abc.com."/>
	</zone>

LEGAL
	(C) Gary Wallis 2001-2020. All Rights Reserved.
	LICENSE file should be included in distribution.
COMPILE
	
	gcc idns-exportXMLzones.c -o idns-exportXMLzones -L/usr/lib/mysql -L/usr/lib64/mysql -L/usr/lib/openisp -lz -lcrypt -lm -lssl -lucidr -lmysqlclient
*/
#include "../../mysqlrad.h"
#include "../../local.h"
#include <dirent.h>
#include <ctype.h>

MYSQL gMysql;

//TOC
int main(int iArgc, char *cArg[]);
void ConnectDb(void);

int main(int iArgc, char *cArg[])
{

	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;
	char gcQuery[1028];

	ConnectDb();

	sprintf(gcQuery,"SELECT tZone.cZone,tZone.uZone,tView.cLabel"
			" FROM tZone,tView"
			" WHERE tZone.uView=tView.uView"
			" AND tZone.uView=2 AND tZone.cZone LIKE '%s'",cArg[1]);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(20);
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("<zone=\"%.99s\">\n",field[0]);

		sprintf(gcQuery,"SELECT tRRType.cLabel,tResource.cName,tResource.uTTL,tResource.cParam1"
			" FROM tResource,tRRType"
			" WHERE tResource.uRRType=1 AND tResource.uRRType=tRRType.uRRType AND tResource.uZone=%s",field[1]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql)) 
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(20);
		}
		res2=mysql_store_result(&gMysql);
		while((field2=mysql_fetch_row(res2)))
		{
			//<record type="A" owner="mail.abc.com." class="IN" ttl="86400" address="1.2.3.5"/>
			printf("\t<record type=\"%s\" owner=\"%s\" class=\"IN\" ttl=\"%s\" address=\"%s\">\n",
					field2[0],field2[1],field2[2],field2[3]);
		}

		printf("</zone>\n");
	}
	return(0);
}//main()


void ConnectDb(void)
{
        mysql_init(&gMysql);
        if(!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,DBPORT,DBSOCKET,0))
	{
		printf("Could not connect to MySQL server\n");
		exit(0);
	}

}//end of ConnectDb()
