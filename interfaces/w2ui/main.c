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

/*
{
    "status"  : "success",
    "total"   : 36,
    "records" : [
        { "recid": 1, "field-1": "value-1", ... "field-N": "value-N" },
        ...
        { "recid": N, "field-1": "value-1", ... "field-N": "value-N" }
    ]
}

else 

{
    "status"  : "error",
    "message" : "Error Message"
}
*/

	printf("Content-type: text/json\n\n");
	sprintf(gcQuery,"SELECT NOW();");
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	unsigned uNumRows=0;
	printf("{\n");
	if((uNumRows=mysql_num_rows(res))>0)
	{
		printf("    'status' : 'success',\n");
		printf("    'total' : %u,\n",uNumRows);
		printf("    'records' : [\n");

		while((field=mysql_fetch_row(res)))
		{
			printf("        { 'recid': 1, 'fname': '%s' }\n",field[0]);
		}
		printf("    ]\n");

	}
	else
	{
		printf("    'status' : 'error',\n");
		printf("    'message' : 'error: mysql_num_rows<=0',\n");
	}
	printf("}\n");

	mysql_free_result(res);
	return(0);

}//end of main()

