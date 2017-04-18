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
void jsonTableRows(char const *cTable);

static char cName[64]={""};
static char cStatus[64]={""};
static char cTemplate[64]={""};
static unsigned {{cTableKey}}=0;

void Process{{cTableName}}Vars(pentry entries[], int x)
{
	register int i;
	
	char *cp;
	for(i=0;i<x;i++)
	{
		//no sql injection
		if((cp=strchr(entries[i].val,'\''))) *cp=0;

		if(!strcmp(entries[i].name,"cName"))
			sprintf(cName,"%.63s",entries[i].val);
		else if(!strcmp(entries[i].name,"cStatus"))
			sprintf(cStatus,"%.63s",entries[i].val);
		else if(!strcmp(entries[i].name,"cTemplate"))
			sprintf(cTemplate,"%.63s",entries[i].val);
		else if(!strcmp(entries[i].name,"{{cTableKey}}"))
			sscanf(entries[i].val,"%u",&{{cTableKey}});
	}

}//void Process{{cTableName}}Vars(pentry entries[], int x)


void {{cTableName}}GetHook(entry gentries[],int x)
{
	//register int i;
	//for(i=0;i<x;i++)
	//{
	//}

	//API Get
	if(!strcmp(gcFunction,"{{cTableName}}Rows"))
		json{{cTableName}}Rows();
	else if(!strcmp(gcFunction,"{{cTableName}}Cols"))
		json{{cTableName}}Cols();
	html{{cTableName}}();

}//void {{cTableName}}GetHook(entry gentries[],int x)


void {{cTableName}}Commands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"{{cTableName}}"))
	{
		Process{{cTableName}}Vars(entries,x);
		html{{cTableName}}();
	}

	//API Post
        MYSQL_RES *res;
	MYSQL_ROW field;
	if(!strcmp(gcFunction,"{{cTableName}}Rows"))
		json{{cTableName}}Rows();
	else if(!strcmp(gcFunction,"{{cTableName}}Cols"))
		json{{cTableName}}Cols();
	else if(!strcmp(gcFunction,"Add{{cTableName}}") || !strcmp(gcFunction,"Mod{{cTableName}}"))
	{
		Process{{cTableName}}Vars(entries,x);
		printf("Content-type: text/plain\n\n");
		if(guPermLevel<7)
		{
			printf("insufficient permissions for add/mod\n");
			exit(0);
		}
		if(cName[0] && cStatus[0] && cTemplate[0])
		{
			unsigned {{cTableKey}}Status=0;
			unsigned uTemplateSet=0;
			sprintf(gcQuery,"SELECT uStatus FROM {{cTableName}}Status WHERE cLabel='%.31s'",cStatus);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&{{cTableKey}}Status);
			if(!{{cTableKey}}Status)
			{
				printf("No such project status: %s\n",cStatus);
				exit(0);
			}
			sprintf(gcQuery,"SELECT uTemplateSet FROM tTemplateSet WHERE cLabel='%.31s'",cTemplate);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sscanf(field[0],"%u",&uTemplateSet);
			if(!uTemplateSet)
			{
				printf("No such template set: %s\n",cTemplate);
				exit(0);
			}
			if({{cTableKey}})
			{
				sprintf(gcQuery,"SELECT {{cTableKey}} FROM {{cTableName}} WHERE {{cTableKey}}=%u"
					" AND (uCreatedBy=%u OR (uOwner=%u AND %u>=10))",
						{{cTableKey}},guLoginClient,guOrg,guPermLevel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				res=mysql_store_result(&gMysql);
				{{cTableKey}}=0;
				if((field=mysql_fetch_row(res)))
				{
					sscanf(field[0],"%u",&{{cTableKey}});
				}
				else
				{
					printf("Insufficient permission to modify project %u\n",{{cTableKey}});
					exit(0);
				}
			}
			if(!{{cTableKey}})
			{
				sprintf(gcQuery,"INSERT INTO {{cTableName}} SET cLabel='%.31s',"
					"{{cTableKey}}Status=%u,"
					"uTemplateSet=%u,"
					"uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())"
						,cName,{{cTableKey}}Status,uTemplateSet,guOrg,guLoginClient);
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
				sprintf(gcQuery,"UPDATE {{cTableName}} SET cLabel='%.31s',"
					"{{cTableKey}}Status=%u,"
					"uTemplateSet=%u,"
					"uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
					" WHERE {{cTableKey}}=%u"
						,cName,{{cTableKey}}Status,uTemplateSet,guLoginClient,{{cTableKey}});
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				printf("%u\n",{{cTableKey}});
			}
		}
		else
		{
			printf("No cName etc.\n");
		}
		exit(0);
	}
	else if(!strcmp(gcFunction,"Del{{cTableName}}"))
	{
		Process{{cTableName}}Vars(entries,x);

		printf("Content-type: text/plain\n\n");
		if(guPermLevel<10)
		{
			printf("insufficient permissions for delete\n");
			exit(0);
		}
		if({{cTableKey}} && cName[0])
		{
			sprintf(gcQuery,"SELECT {{cTableKey}} FROM tTable WHERE {{cTableKey}}=%u",{{cTableKey}});
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				printf("%s\n",mysql_error(&gMysql));
				exit(0);
			}
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				printf("{{cTableName}} has tables not deleted\n");
				exit(0);
			}
			sprintf(gcQuery,"SELECT {{cTableKey}} FROM {{cTableName}}"
					" WHERE {{cTableKey}}=%u AND cLabel='%s'"
					" AND (uCreatedBy=%u OR (uOwner=%u AND %u>=10))",
						{{cTableKey}},cName,guLoginClient,guOrg,guPermLevel);
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
					" WHERE {{cTableKey}}=%u AND cLabel='%s'"
					" AND (uCreatedBy=%u OR (uOwner=%u AND %u>=10))",
						{{cTableKey}},cName,guLoginClient,guOrg,guPermLevel);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
				{
					printf("%s\n",mysql_error(&gMysql));
					exit(0);
				}
				if(mysql_affected_rows(&gMysql)>0)
					printf("%u\n",{{cTableKey}});
				else
					printf("{{cTableName}} not deleted. Unexpected mysql_affected_rows() error\n");
			}
			else
			{
				printf("{{cTableName}} not deleted. Insuficient permissions. guOrg=%u guPermLevel=%u\n",guOrg,guPermLevel);
			}
		}
		else
		{
			printf("{{cTableName}} not found %u/%s\n",{{cTableKey}},cName);
		}
		exit(0);
	}

}//void {{cTableName}}Commands(pentry entries[], int x)


void html{{cTableName}}(void)
{
	htmlHeader("{{cTableName}}","{{cTableName}}.Header");
	html{{cTableName}}Page("{{cTableName}}","{{cTableName}}.Body");
	htmlFooter("{{cTableName}}.Footer");

}//void html{{cTableName}}(void)


void html{{cTableName}}Page(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelectInterface(cTemplateName,uPLAINSET,uOneLogin);
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

	sprintf(cQuery,"SELECT {{cTableName}}.{{cTableKey}},{{cTableName}}.cLabel,{{cTableName}}Status.cLabel,tTemplateSet.cLabel"
			" FROM {{cTableName}},{{cTableName}}Status,tTemplateSet"
			" WHERE {{cTableName}}.{{cTableKey}}Status={{cTableName}}Status.uStatus"
			" AND {{cTableName}}.uTemplateSet=tTemplateSet.uTemplateSet"
			" AND {{cTableName}}.uOwner=%u",guOrg);
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
				" \"cName\": \"%s\","
				" \"cStatus\": \"%s\","
				" \"cTemplate\": \"%s\""
					,field[0],field[1],field[2],field[3]);
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
	//printf("\t{\"name\": \"{{cTableKey}}\", \"title\": \"Unique {{cTableName}} ID\", \"filterable\": false },\n");
	printf("\t{\"name\": \"{{cTableKey}}\", \"title\": \"{{cTableName}} ID\" },\n");
	printf("\t{\"name\": \"cName\", \"title\": \"Name\"},\n");
	printf("\t{\"name\": \"cStatus\", \"title\": \"Status\"},\n");
	printf("\t{\"name\": \"cTemplate\", \"title\": \"Template\", \"breakpoints\": \"xs sm\"}\n");
	printf("]\n");
	exit(0);
}//void json{{cTableName}}Cols(void)


void jsonTableRows(char const *cTable)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cQuery[1028]={""};
	char cColumnName[64][64];
	unsigned uColumn=0;

	printf("Content-type: text/json\n\n");
	printf("[\n");

	sprintf(cQuery,"SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS"
					" WHERE TABLE_SCHEMA='unxsrad' AND TABLE_NAME='%s';",cTable);
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
		while((field=mysql_fetch_row(res)) && uColumn<64)
		{
			sprintf(cColumnName[uColumn],"%.63s",field[0]);
			uColumn++;
		}
	}
	mysql_free_result(res);

	sprintf(cQuery,"SELECT * FROM %.99s",cTable);
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
	uNumRows=0;
	if((uNumRows=mysql_num_rows(res))>0)
	{
		unsigned uLast=0;
		unsigned register i;
		unsigned uFirst;
		while((field=mysql_fetch_row(res)))
		{
			printf("\t{");
			uFirst=1;
			for(i=0;i<uColumn;i++)
			{
				if(!uFirst)
					printf(", ");
				printf("\"%s\": \"%s\"",cColumnName[i],field[i]);
				uFirst=0;
			}
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

}//void jsonTableRows(char const *cTable)

