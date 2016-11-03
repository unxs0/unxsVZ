/*
FILE
	dhcpd-conf.c
	svn ID removed
PURPOSE
	Create dhcpd.conf file based on MACs
AUTHOR
	Gary Wallis for Unxiservice (C) 2008-2009. GPL2 License applies.
NOTES
*/

#include "../../mysqlrad.h"

#define mysqlrad_Query_TextErr_Exit	mysql_query(&gMysql,gcQuery);\
					if(mysql_errno(&gMysql))\
					{\
						printf("%s\n",mysql_error(&gMysql));\
						exit(2);\
					}

MYSQL gMysql;
char gcQuery[8192]={""};
unsigned guLoginClient=1;//Root user
char cHostname[100]={""};
FILE *fp;

//local protos
void TextConnectDb(void);
void CreateDHCPDConf(void);

int main()
{
	CreateDHCPDConf();
	return(0);
}//main()


void TextConnectDb(void)
{
        mysql_init(&gMysql);
        if (!mysql_real_connect(&gMysql,DBIP0,DBLOGIN,DBPASSWD,DBNAME,DBPORT0,DBSOCKET,0))
        {
        	if (!mysql_real_connect(&gMysql,DBIP1,DBLOGIN,DBPASSWD,DBNAME,DBPORT1,DBSOCKET,0))
		{
			fprintf(stderr,"Database server unavailable.\n");
			exit(1);
		}
        }
}//void TextConnectDb(void)


void CreateDHCPDConf(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	fp=stdout;
	TextConnectDb();

	//Top section of dhcpd.conf
	sprintf(gcQuery,"SELECT cTemplate FROM tTemplate WHERE cLabel='dhcpd.conf top'");
	mysqlrad_Query_TextErr_Exit;
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		fprintf(fp,"%s",field[0]);
	mysql_free_result(res);

	//Middle section A of dhcpd.conf
	sprintf(gcQuery,"SELECT tProfileName.cLabel FROM tProfileName ORDER BY tProfileName.cLabel");
	mysqlrad_Query_TextErr_Exit;
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"class \"%s\" {\n\tmatch hardware;\n}\n",field[0]);
	mysql_free_result(res);

/*	
class "allocation class 1" {
        match hardware;
}

subclass "allocation class 1" 00:2b:4c:39:ad;
*/

	//Middle section B of dhcpd.conf
	sprintf(gcQuery,"SELECT tProfileName.cLabel,tUser.cIP FROM tProfileName,tUser WHERE"
			" tUser.uProfileName=tProfileName.uProfileName AND"
			//00:16:ea:c2:7f:fa
			" tUser.cIP RLIKE '..:..:..:..:..:..' ORDER BY tProfileName.cLabel");
	mysqlrad_Query_TextErr_Exit;
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		fprintf(fp,"subclass \"%s\" %s;\n",field[0],field[1]);
	mysql_free_result(res);

	//Bottom section of dhcpd.conf
	sprintf(gcQuery,"SELECT cTemplate FROM tTemplate WHERE cLabel='dhcpd.conf bottom'");
	mysqlrad_Query_TextErr_Exit;
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		fprintf(fp,"%s",field[0]);
	mysql_free_result(res);

}//void CreateDHCPDConf(void)
