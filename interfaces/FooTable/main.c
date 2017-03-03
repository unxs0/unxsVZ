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
#include "cgi.h"


//
//Local only
int main(int argc, char *argv[]);



int main(int argc, char *argv[])
{
	MYSQL sMysql;
	MYSQL_RES *res;
	MYSQL_ROW field;
	//cgi.c
	pentry entries[512];

	char cQuery[4096]={""};

	char cHost[128]={""};
	char cHostname[128]={""};

	InterfaceConnectDb(&sMysql);

	if(getenv("REMOTE_ADDR")!=NULL)
		sprintf(cHost,"%.127s",getenv("REMOTE_ADDR"));

	gethostname(cHostname,127);



	char cRequestMethod[256]={""};
	char cFunction[100]={""};
	sprintf(cRequestMethod,"%.255s",getenv("REQUEST_METHOD"));
	if(!strcmp(cRequestMethod,"POST"))
	{
		register int x;
		int iCLength = atoi(getenv("CONTENT_LENGTH"));
		for(x=0; iCLength && (!feof(stdin)) && x<512 ;x++)
		{
			entries[x].val = fmakeword(stdin,'&',&iCLength);
			plustospace(entries[x].val);
			unescape_url(entries[x].val);
			entries[x].name = makeword(entries[x].val,'=');

			if(!strcmp(entries[x].name,"cFunction")) 
				sprintf(cFunction,"%.99s",entries[x].val);
		}
/*
		FILE *fp;
		if((fp=fopen("/tmp/unxsvzFT.cgi.log","w"))!=NULL)
		{
			fprintf(fp,"POST data\n");
			fprintf(fp,"%s\n",cFunction);
			fclose(fp);
		}
*/
	}


	if(!strcmp(cFunction,"PostRows"))
	{
		printf("Content-type: text/json\n\n");
		sprintf(cQuery,"SELECT uLog,cHost,cLogin,cTableName,cLabel,FROM_UNIXTIME(uCreatedDate,'%%m/%%d/%%Y') FROM tLog");
		mysql_query(&sMysql,cQuery);
		if(mysql_errno(&sMysql))
		{
			printf("{\n");
			printf("    'status' : 'error',\n");
			printf("    'message' : 'error: %s',\n",mysql_error(&sMysql));
			printf("}\n");
			return(0);
		}
		res=mysql_store_result(&sMysql);
		unsigned uNumRows=0;
		if((uNumRows=mysql_num_rows(res))>0)
		{
			printf("[\n");
	
			unsigned uLast=0;
			while((field=mysql_fetch_row(res)))
			{
				printf("        { \"uLog\": %s, \"cHost\": \"%s\", \"cLogin\": \"%s\","
				" \"cTableName\": \"%s\", \"cLabel\": \"%s\", \"uCreatedDate\": \"%s\" }",
							field[0],field[1],field[2],field[3],field[4],field[5]);
				if((++uLast)<uNumRows)
					printf(",\n");
				else
					printf("\n");
			}
			printf("]\n");

		}
		mysql_free_result(res);
	}//PostRows
	else if(!strcmp(cFunction,"PostCols"))
	{
		printf("Content-type: text/json\n\n");
		sprintf(cQuery,"SELECT COLUMN_NAME,DATA_TYPE,CHARACTER_MAXIMUM_LENGTH FROM INFORMATION_SCHEMA.COLUMNS"
					" WHERE TABLE_SCHEMA='"DBNAME"' AND TABLE_NAME='%s';","tLog");
		mysql_query(&sMysql,cQuery);
		if(mysql_errno(&sMysql))
			return(1);
		res=mysql_store_result(&sMysql);
		unsigned uNumRows=0;
		if((uNumRows=mysql_num_rows(res))>0)
		{
			printf("[\n");
	
			unsigned uLast=0;
			unsigned uVarcharCount=0;
			char *cBreakpoints="";
			while((field=mysql_fetch_row(res)))
			{
				if(!strcmp(field[1],"varchar"))
				{
					uVarcharCount++;
					unsigned uSize=0;
					sscanf(field[2],"%u",&uSize);
					if(uVarcharCount>4)
						cBreakpoints=", \"breakpoints\": \"xs sm md\"";
					else if(uVarcharCount>3)
						cBreakpoints=", \"breakpoints\": \"xs sm\"";
				}
				else if(1)
				{
					cBreakpoints=", \"breakpoints\": \"xs sm\"";
				}
				printf("        { \"name\": \"%s\", \"title\": \"%s\"%s }",
							field[0],field[0],cBreakpoints);
				if((++uLast)<uNumRows)
					printf(",\n");
				else
					printf("\n");
			}
			printf("]\n");

		}
		mysql_free_result(res);
	}//PostCols

	return(0);

}//end of main()

