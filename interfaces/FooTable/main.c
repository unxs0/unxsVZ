/*
FILE 
	main.c
AUTHOR/LEGAL
	(C) 2017 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
REQUIRES
*/

#include "interface.h"

MYSQL gMysql;
//Multipurpose buffer
char gcQuery[4096]={""};
char gcHost[128]={""};
char gcHostname[128]={""};

//
//Local only
int main(int argc, char *argv[]);



int main(int argc, char *argv[])
{
	InterfaceConnectDb();

	if(getenv("REMOTE_ADDR")!=NULL)
		sprintf(gcHost,"%.127s",getenv("REMOTE_ADDR"));

	gethostname(gcHostname,127);

	printf("Content-type: text/json\n\n");
	printf("{\n");


	char cRequestMethod[256]={""};
	sprintf(cRequestMethod,"%.255s",getenv("REQUEST_METHOD"));
	if(!strcmp(cRequestMethod,"POST"))
	{
		//printf("    'status' : 'error',\n");
		//printf("    'message' : 'error: unsupported post',\n");
		//printf("}\n");
		//return(1);
	}


	sprintf(gcQuery,"SELECT uLog,cHost,cLogin,cTableName,cLabel,FROM_UNIXTIME(uCreatedDate,'%%m/%%d/%%Y') FROM tLog");
	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		printf("    'status' : 'error',\n");
		printf("    'message' : 'error: %s',\n",mysql_error(&gMysql));
		printf("}\n");
		return(0);
	}
        MYSQL_RES *res;
	MYSQL_ROW field;
	res=mysql_store_result(&gMysql);
	unsigned uNumRows=0;
	if((uNumRows=mysql_num_rows(res))>0)
	{
		printf("    \"rows\" : [\n");

		unsigned uLast=0;
		while((field=mysql_fetch_row(res)))
		{
			printf("        { \"uLog\": %s, \"cHost\": \"%s\", \"cLogin\": \"%s\", \"cTableName\": \"%s\", \"cLabel\": \"%s\", \"uCreatedDate\": \"%s\" }",
						field[0],field[1],field[2],field[3],field[4],field[5]);
			if((++uLast)<uNumRows)
				printf(",\n");
			else
				printf("\n");
		}
		printf("]\n");

	}
	mysql_free_result(res);

	printf("}\n");
	return(0);

}//end of main()

