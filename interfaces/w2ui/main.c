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

	if(!strcmp(getenv("REQUEST_METHOD"),"POST"))
	{
	}

        MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT NOW();");
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		printf("%s\n",field[0]);
	}
	mysql_free_result(res);
	return(0);

}//end of main()

