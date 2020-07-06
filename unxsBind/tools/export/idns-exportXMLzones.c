/*
FILE 
	tools/export/idns-exportXMLzones.c
PURPOSE
	Standalone XML zone export tool
STANDARDS
	https://tools.ietf.org/id/draft-daley-dnsxml-00.html
ISSUES
WORK IN PROGRESS
	SOA record at new tZone is required. Also RRset requires, e.g. type="SOA".
	NS records must come from tNSSet mess.
	Also check for main address and special MX sets. These come from tZone.
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

	sprintf(gcQuery,"SELECT tZone.cZone,tZone.uZone,tView.cLabel,"
			"tNS.cFQDN,tZone.cHostmaster,tZone.uSerial,"
			"tZone.uRefresh,tZone.uRetry,tZone.uExpire,tZone.uTTL,"
			"tZone.cMainAddress,tZone.uMailServers"
			" FROM tZone,tView,tNS"
			" WHERE tZone.uView=tView.uView"
			" AND tZone.uNSSet=tNS.uNSSet"
			" AND tNS.uNSType=1"
			" AND tZone.uView=2 AND tZone.cZone LIKE '%s' ORDER BY tNS.uNS",cArg[1]);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) 
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(20);
	}
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		//RRSet allows for app handling for non standard usage
		//like here. Use default A type
		printf("\t<RRSet owner=\"%.99s\" class=\"IN\" ttl=\"0\" type=\"A\">\n",field[0]);


		//tZone specified records
		printf("\t\t<SOA mname=\"%s\" rname=\"%s\" serial=\"%s\" refresh=\"%s\" retry=\"%s\" expire=\"%s\" minimum=\"%s\"/>\n",
				field[3],field[4],field[5],field[6],field[7],field[8],field[9]);
		//TODO one or more like so...
		printf("\t\t<NS owner=\"%s\" class=\"IN\" ttl=\"0\" nsdname=\"dns1.lon2.telecity.net\"/>\n",field[0]);
		//if cMainAddress add A record
		//if uMailServer add associated MX records

		sprintf(gcQuery,"SELECT tRRType.cLabel,tResource.cName,tResource.uTTL,tResource.cParam1,tResource.cParam2"
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

			if(!strcmp(field2[1],"@")) field2[1][0]=0;
			if(!strcmp(field2[0],"A"))
			{
				//<record type="A" owner="mail.abc.com." class="IN" ttl="86400" address="1.2.3.5"/>
				printf("\t\t<A owner=\"%s\" class=\"IN\" ttl=\"%s\" address=\"%s\"/>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"AAAA"))
			{
				//<record type="AAAA" owner="mail.abc.com." class="IN" ttl="86400" ip6address="::1::1234::"/>
				printf("\t\t<AAAA owner=\"%s\" class=\"IN\" ttl=\"%s\" ip6address=\"%s\"/>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"CNAME"))
			{
				//<record type="CNAME" owner="mail.abc.com." class="IN" ttl="86400" host="blabla.com"/>
				printf("\t\t<CNAME owner=\"%s\" class=\"IN\" ttl=\"%s\" host=\"%s\"/>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"HINFO"))
			{
				printf("\t\t<HINFO owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED/>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"MX"))
			{
				printf("\t\t<MX owner=\"%s\" class=\"IN\" ttl=\"%s\" preference=\"%s\""
						" exchange=\"%s\"/>\n",
					field2[1],field2[2],field2[3],field2[4]);
			}
			else if(!strcmp(field2[0],"NAPTR"))
			{
				printf("\t\t<NAPTR owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED/>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"NS"))
			{
				printf("\t\t<NS owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED/>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"PTR"))
			{
				printf("\t\t<PTR owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED/>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"SRV"))
			{
				printf("\t\t<SRV owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED/>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(!strcmp(field2[0],"TXT"))
			{
				printf("\t\t<TXT owner=\"%s\" class=\"IN\" ttl=\"%s\">%s</TXT>\n",
					field2[1],field2[2],field2[3]);
			}
			else if(1)
			{
				printf("\t\t<%s owner=\"%s\" class=\"IN\" ttl=\"%s\" NOT-IMPLEMENTED/>\n",
					field2[0],field2[1],field2[2]);
			}
		}

		printf("\t</RRSet>\n");
	}
	printf("</dnsxml>\n");

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
