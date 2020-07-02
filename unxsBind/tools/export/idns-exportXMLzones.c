/*
FILE 
	tools/export/idns-exportXMLzones.c
PURPOSE
	Standalone XML zone export tool
STANDARDS
	https://tools.ietf.org/id/draft-daley-dnsxml-00.html
EXAMPLE OUTPUT

	<zone name="abc.com">
	<record type="A" owner="abc.com." class="IN" ttl="86400" address="1.2.3.4"/>
	<record type="A" owner="mail.abc.com." class="IN" ttl="86400" address="1.2.3.5"/>
	<record type="MX" owner="abc.com." class="IN" ttl="86400" preference="10" exchange="mail.abc.com."/>
	</zone>
ISSUES
	1-. Customer provides example with non standard zone, and record dnsxml rr types.
	Do we follow as extension, as per standard above? Or ignore using
	3.2.1. Base RR element and base attributes. E.g. owner is the zone with subzones?
	2-. The current code, proof of concept based on email from D. Rogal, does not adhere to the
	dnsxml standard as set forth in ietf draft.
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


	printf("<dnsxml xmlns=\"urn:ietf:params:xml:ns:dns\"\n"
			"\txmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
			"\txsi:schemaLocation=\"urn:ietf:params:xml:ns:dns dnsxml-1.0.xsd\">\n");

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
		printf("\t<zone=\"%.99s\">\n",field[0]);

		sprintf(gcQuery,"SELECT tRRType.cLabel,tResource.cName,tResource.uTTL,tResource.cParam1"
			" FROM tResource,tRRType"
			" WHERE tResource.uRRType=tRRType.uRRType AND tResource.uZone=%s",field[1]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql)) 
		{
			printf("%s\n",mysql_error(&gMysql));
			exit(20);
		}
		res2=mysql_store_result(&gMysql);
		while((field2=mysql_fetch_row(res2)))
		{
/*
A
AAAA
CNAME
HINFO
MX
NAPTR
NS
PTR
SRV
TXT
*/

			if(!strcmp(field2[0],"A"))
			{
				//<record type="A" owner="mail.abc.com." class="IN" ttl="86400" address="1.2.3.5"/>
				printf("\t\t<record type=\"A\" owner=\"%s\" class=\"IN\" ttl=\"%s\" address=\"%s\">\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"AAAA"))
			{
				//<record type="AAAA" owner="mail.abc.com." class="IN" ttl="86400" ip6address="::1::1234::"/>
				printf("\t\t<record type=\"AAAA\" owner=\"%s\" class=\"IN\" ttl=\"%s\" ip6address=\"%s\">\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"CNAME"))
			{
				//<record type="CNAME" owner="mail.abc.com." class="IN" ttl="86400" host="blabla.com"/>
				printf("\t\t<record type=\"CNAME\" owner=\"%s\" class=\"IN\" ttl=\"%s\" host=\"%s\">\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"HINFO"))
			{
				printf("\t\t<record type=\"HINFO\" owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"MX"))
			{
				printf("\t\t<record type=\"MX\" owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"NAPTR"))
			{
				printf("\t\t<record type=\"NAPTR\" owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"NS"))
			{
				printf("\t\t<record type=\"NS\" owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"PTR"))
			{
				printf("\t\t<record type=\"PTR\" owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"SRV"))
			{
				printf("\t\t<record type=\"SRV\" owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"TXT"))
			{
				printf("\t\t<record type=\"TXT\" owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED>\n",
					field2[1],field2[2],field2[3]);
			}
		}

		printf("\t</zone>\n");
	}
	printf("</dnsxmls>\n");

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
