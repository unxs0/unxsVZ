/*
FILE 
	{{cTableNameLC}}.c
	Template unxsRAD/interfaces/bootstrap/table.c
AUTHOR/LEGAL
	(C) 2010-2017 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See included LICENSE file.
PURPOSE
	For developing bootstrap/footable templates
	for unxsRAD RAD4 generated web apps.
*/

#include "interface.h"

//TOC
void Process{{cTableName}}Vars(pentry entries[], int x);
void {{cTableName}}GetHook(entry gentries[],int x);
void {{cTableName}}Commands(pentry entries[], int x);
void html{{cTableName}}(void);
void html{{cTableName}}Page(char *cTitle, char *cTemplateName);
void json{{cTableName}}Rows(void);
void json{{cTableName}}Cols(void);

//funcModuleVars
{{funcModuleVars}}

void Process{{cTableName}}Vars(pentry entries[], int x)
{
	register int i;
	
	char *cp;
	for(i=0;i<x;i++)
	{
		//no sql injection
		if((cp=strchr(entries[i].val,'\''))) *cp=0;

		//funcModuleProcVars
		{{funcModuleProcVars}}

	}

}//void Process{{cTableName}}Vars(pentry entries[], int x)


void {{cTableName}}GetHook(entry gentries[],int x)
{
	//register int i;
	//for(i=0;i<x;i++)
	//{
	//}

	//API Get
	if(!strcmp(gcFunction,"{{cTableNameBS}}Rows"))
		json{{cTableName}}Rows();
	else if(!strcmp(gcFunction,"{{cTableNameBS}}Cols"))
		json{{cTableName}}Cols();
	html{{cTableName}}();

}//void {{cTableName}}GetHook(entry gentries[],int x)


void {{cTableName}}Commands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"{{cTableNameBS}}"))
	{
		Process{{cTableName}}Vars(entries,x);
		html{{cTableName}}();
	}

	//API Post
        MYSQL_RES *res;
	MYSQL_ROW field;
	if(!strcmp(gcFunction,"{{cTableNameBS}}Rows"))
		json{{cTableName}}Rows();
	else if(!strcmp(gcFunction,"{{cTableNameBS}}Cols"))
		json{{cTableName}}Cols();
	else if(!strcmp(gcFunction,"Add{{cTableNameBS}}") || !strcmp(gcFunction,"Mod{{cTableNameBS}}"))
	{
		Process{{cTableName}}Vars(entries,x);
		printf("Content-type: text/plain\n\n");
		if(guPermLevel<7)
		{
			printf("insufficient permissions for add/mod\n");
			exit(0);
		}

		//Check data
		if(!cLabel[0])
		{
			printf("No cLabel for %u\n",{{cTableKey}});
			exit(0);
		}
		else
		{
			//Update or Insert
			if(!{{cTableKey}})
			{
				sprintf(gcQuery,"INSERT INTO {{cTableName}}"
					" SET cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,cLabel,guOrg,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				printf("%llu\n",mysql_insert_id(&gMysql));
			}
			else
			{
				sprintf(gcQuery,"UPDATE {{cTableName}} SET cLabel='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
					" WHERE {{cTableKey}}=%u"
						,cLabel,guLoginClient,{{cTableKey}});
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				printf("%u\n",{{cTableKey}});
			}
		}
		exit(0);
	}
	else if(!strcmp(gcFunction,"Del{{cTableNameBS}}"))
	{
		Process{{cTableName}}Vars(entries,x);

		printf("Content-type: text/plain\n\n");
		if(guPermLevel<10)
		{
			printf("insufficient permissions for delete\n");
			exit(0);
		}
		//check data
		if({{cTableKey}})
		{
			sprintf(gcQuery,"SELECT {{cTableKey}} FROM {{cTableName}}"
					" WHERE {{cTableKey}}=%u"
					" AND (uCreatedBy=%u OR (uOwner=%u AND %u>=10))",
						{{cTableKey}},guLoginClient,guOrg,guPermLevel);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(gcQuery,"DELETE FROM {{cTableName}}"
					" WHERE {{cTableKey}}=%u"
					" AND (uCreatedBy=%u OR (uOwner=%u AND %u>=10))",
						{{cTableKey}},guLoginClient,guOrg,guPermLevel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				if(mysql_affected_rows(&gMysql)>0)
					printf("%u\n",{{cTableKey}});
				else
					printf("{{cTableNameBS}} not deleted. Unexpected mysql_affected_rows() error\n");
			}
			else
			{
				printf("{{cTableNameBS}} not deleted. Insuficient permissions. guOrg=%u guPermLevel=%u\n",guOrg,guPermLevel);
			}
		}
		else
		{
			printf("{{cTableNameBS}} not found %u\n",{{cTableKey}});
		}
		exit(0);
	}

}//void {{cTableName}}Commands(pentry entries[], int x)


void html{{cTableName}}(void)
{
	htmlHeader("{{cTableName}}","Default.Header");
	html{{cTableName}}Page("{{cTableName}}","{{cTableNameBS}}.Body");
	htmlFooter("Default.Footer");

}//void html{{cTableName}}(void)


void html{{cTableName}}Page(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uDEFAULT,uBOOTSTRAP);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;

			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="mobile.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcUser;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);//Safe?

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcMessage";
			template.cpValue[7]=gcMessage;

			template.cpName[8]="gcBrand";
			template.cpValue[8]=INTERFACE_HEADER_TITLE;

			template.cpName[9]="gcCopyright";
			template.cpValue[9]=INTERFACE_COPYRIGHT;

			template.cpName[10]="";

			printf("\n<!-- Start html{{cTableName}}Page(%s) -->\n",cTemplateName); 
			Template(field[0],&template,stdout);
			printf("\n<!-- End html{{cTableName}}Page(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void html{{cTableName}}Page()


void json{{cTableName}}Rows(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cQuery[1028]={""};

	printf("Content-type: text/json\n\n");
	printf("[\n");

	sprintf(cQuery,"SELECT {{cTableName}}.{{cTableKey}},{{cTableName}}.cLabel,tClient.cLabel"
			" FROM {{cTableName}},tClient"
			" WHERE {{cTableName}}.uOwner=%u AND tClient.uClient={{cTableName}}.uOwner",guOrg);
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		printf("\t{\n");
		printf("\t\t'status' : 'error',\n");
		printf("\t\t'message' : 'error: %s',\n",mysql_error(&gMysql));
		printf("\t}\n");
		printf("]\n");
		exit(0);
	}
	res=mysql_store_result(&gMysql);
	unsigned uNumRows=0;
	if((uNumRows=mysql_num_rows(res))>0)
	{
		unsigned uLast=0;
		while((field=mysql_fetch_row(res)))
		{
			printf("\t{");
			printf("\"{{cTableKey}}\": \"%s\","
				" \"cLabel\": \"%s\","
				" \"cOwner\": \"%s\""
					,field[0],field[1],field[2]);
			printf("}");
			if((++uLast)<uNumRows)
				printf(",\n");
			else
				printf("\n");
		}
	}
	mysql_free_result(res);
	printf("]\n");
	exit(0);

}//void json{{cTableName}}Rows(void)


void json{{cTableName}}Cols(void)
{
	printf("Content-type: text/json\n\n");
	printf("[\n");
	//printf("\t{\"name\": \"{{cTableKey}}\", \"title\": \"Unique {{cTableNameBS}} ID\", \"filterable\": false },\n");
	printf("\t{\"name\": \"{{cTableKey}}\", \"title\": \"{{cTableNameBS}} ID\" },\n");
	printf("\t{\"name\": \"cLabel\", \"title\": \"Label\"},\n");
	printf("\t{\"name\": \"cOwner\", \"title\": \"Owner\", \"breakpoints\": \"xs sm\"}\n");
	printf("]\n");
	exit(0);
}//void json{{cTableName}}Cols(void)
