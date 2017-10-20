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
void html{{cTableName}}Report(void);
void html{{cTableName}}Filter(void);
void html{{cTableName}}Page(char *cTitle, char *cTemplateName);
void json{{cTableName}}Rows(void);
void json{{cTableName}}Cols(void);
void json{{cTableName}}LDRows(void);
void json{{cTableName}}LDCols(void);

//funcModuleVars
{{funcModuleVars}}

//Special template
static char gcContext[256]={""};
static char gcFilterRows[100]={""};
static char gcFilterCols[100]={""};

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


void html{{cTableName}}Filter(void)
{
        htmlHeader("{{cTableName}}","Default.Header");
        html{{cTableName}}Page("{{cTableName}}","{{cTableNameBS}}Filter.Body");
        htmlFooter("Default.Footer");

}//void html{{cTableName}}Filter()


void {{cTableName}}GetHook(entry gentries[],int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"{{cTableKey}}"))
			sscanf(gentries[i].val,"%u",&{{cTableKey}});
		{{funcBSGetHookAdditionalGentries}}
	}


	//API Get
	if(!strcmp(gcFunction,"{{cTableNameBS}}Rows"))
		json{{cTableName}}Rows();
	else if(!strcmp(gcFunction,"{{cTableNameBS}}Cols"))
		json{{cTableName}}Cols();
	else if(!strcmp(gcFunction,"{{cTableNameBS}}LDRows"))
		json{{cTableName}}LDRows();
	else if(!strcmp(gcFunction,"{{cTableNameBS}}LDCols"))
		json{{cTableName}}LDCols();

	//Add special page links
	
	{{funcBSGetHookPrePages}}
	{{funcBSGetHookPages}}
	html{{cTableName}}();

}//void {{cTableName}}GetHook(entry gentries[],int x)


void Insert_{{cTableName}}(void)
{
	{{funcModuleInsertQuery}}
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(0);
	}

}//void Insert_{{cTableName}}(void)


void Update_{{cTableName}}(char *cRowid)
{
	{{funcModuleUpdateQuery}}
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s\n",mysql_error(&gMysql));
		exit(0);
	}

}//void Update_{{cTableName}}(void)


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

		//Update or Insert
		if(!{{cTableKey}})
		{
			uOwner=guOrg;
			uCreatedBy=guLoginClient;
			Insert_{{cTableName}}();
			printf("%llu\n",mysql_insert_id(&gMysql));
		}
		else
		{
			uModBy=guLoginClient;
			char cRowid[32]={""};
			sprintf(cRowid,"%u",{{cTableKey}});
			Update_{{cTableName}}(cRowid);
			printf("%u\n",{{cTableKey}});
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


void html{{cTableName}}Report(void)
{
	htmlHeader("{{cTableName}}","Default.Header");
	html{{cTableName}}Page("{{cTableName}}","{{cTableNameBS}}Report.Body");
	htmlFooter("Default.Footer");

}//void html{{cTableName}}Report(void)


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
			template.cpValue[1]="/unxsID";
			
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

			template.cpName[10]="{{cTableKey}}";
			char c{{cTableKey}}[16];
			sprintf(c{{cTableKey}},"%u",{{cTableKey}});
			template.cpValue[10]=c{{cTableKey}};

			template.cpName[11]="cFilterRows";
			template.cpValue[11]=gcFilterRows;

			template.cpName[12]="cFilterCols";
			template.cpValue[12]=gcFilterCols;

			template.cpName[13]="cContext";
			template.cpValue[13]=gcContext;

			template.cpName[14]="cNavBarClass{{cTableNameBS}}";
			template.cpValue[14]="class=\"active\"";

			template.cpName[15]="";

			{{funcBSTemplateFKNVPairs}}	


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

	if({{cTableKey}})
		sprintf(cQuery,"SELECT {{funcBootstrapRowReportVars}}"
			" FROM {{cTableName}}"
			" WHERE uOwner=%1$u AND {{cTableKey}}=%2$u",guOrg,{{cTableKey}});
	{{funcBSFKJsonRowsSelects}}
	else
		sprintf(cQuery,"SELECT {{funcBootstrapRowVars}}"
			" FROM {{cTableName}}"
			" WHERE uOwner=%u",guOrg);
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
			if({{cTableKey}})
				printf("{{funcBootstrapRowReportFormats}}",
					{{funcBootstrapRowReportFields}});
			else
				printf("{{funcBootstrapRowFormats}}",
					{{funcBootstrapRowFields}});
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
	if({{cTableKey}})
	{
		{{funcBootstrapColsReport}}
	}
	else
	{
		{{funcBootstrapCols}}
	}
	printf("]\n");
	exit(0);
}//void json{{cTableName}}Cols(void)


void json{{cTableName}}LDRows(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	char cQuery[1028]={""};

	char cTables[32][64]={"{{funcBSLDTables}}"};

	//for errors
	printf("Content-type: text/json\n\n");
	printf("[\n");

	sprintf(cQuery,"CREATE TEMPORARY TABLE tLatestData"
			" ("
			" uLatestData INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			" cuTableKey VARCHAR(16) NOT NULL DEFAULT '',"
			" cTable VARCHAR(32) NOT NULL DEFAULT '',"
			" cTableLabel VARCHAR(32) NOT NULL DEFAULT '',"
			" cLastDate VARCHAR(32) NOT NULL DEFAULT '',"
			" cuLastUser VARCHAR(16) NOT NULL DEFAULT '',"
			" cCreatedDate VARCHAR(32) NOT NULL DEFAULT '',"
			" cModDate VARCHAR(32) NOT NULL DEFAULT '',"
			" cuCreatedBy VARCHAR(16) NOT NULL DEFAULT '',"
			" cuModBy VARCHAR(16) NOT NULL DEFAULT '',"
			" c{{funcBSLDContextVar2}} VARCHAR(16) NOT NULL DEFAULT ''"
			" )");
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		printf("\t{\n");
		printf("\"status\" : \"error\",");
		printf("\"message\" : \"error1: %s\"",mysql_error(&gMysql));
		printf("}\n");
		printf("]\n");
		exit(0);
	}

	register int i;
	for(i=0;i<32 && cTables[i][0];i++)
	{
		sprintf(cQuery,"INSERT INTO tLatestData"
			" ("
			" cuTableKey,cTable,cTableLabel,"
			" cCreatedDate,cModDate,"
			" cuCreatedBy,cuModBy,c{{funcBSLDContextVar2}},"
			" cLastDate,cuLastUser"
			" )"
			" SELECT u%s,'%s',cLabel,"
			" FROM_UNIXTIME(uCreatedDate),IF(uModDate,FROM_UNIXTIME(uModDate),'Not Modified'),"
			" uCreatedBy,uModBy,{{funcBSLDContextVar2}},"
			" IF(uModDate,FROM_UNIXTIME(uModDate),FROM_UNIXTIME(uCreatedDate)),"
			" IF(uModDate,uModBy,uCreatedBy)"
			" FROM t%s"
			" WHERE {{funcBSLDContextVar1}}=%u"
			" ORDER BY uModDate DESC,uCreatedDate DESC",
				cTables[i],cTables[i],cTables[i],{{funcBSLDContextVar1}});
		mysql_query(&gMysql,cQuery);
		if(mysql_errno(&gMysql))
		{
			printf("\t{\n");
			printf("\t\t\"status\" : \"error\",\n");
			printf("\t\t\"message\" : \"error2: %s\"\n",mysql_error(&gMysql));
			printf("\t}\n");
			printf("]\n");
			exit(0);
		}
	}

	sprintf(cQuery,"SELECT 'cuTableKey',cuTableKey,"
			"'cTable',cTable,"
			"'cTableLabel',cTableLabel,"
			"'cLastDate',cLastDate,"
			"'cLastUser',cuLastUser,"
			"'cCreatedDate',cCreatedDate,"
			"'cModDate',cModDate,"
			"'cCreatedBy',cuCreatedBy,"
			"'cModBy',cuModBy,"
			"'c{{funcBSLDContextVar2}}',c{{funcBSLDContextVar2}}"
			" FROM tLatestData");
	mysql_query(&gMysql,cQuery);
	if(mysql_errno(&gMysql))
	{
		printf("\t{\n");
		printf("\t\t\"status\" : \"error\",\n");
		printf("\t\t\"message\" : \"error3: %s\"\n",mysql_error(&gMysql));
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
			unsigned uLastUser=0;
			sscanf(field[9],"%u",&uLastUser);
			printf("\t{");
				printf("\"%s\": \"%s\","
					"\"%s\": \"<a href=?gcPage=%s&u%s=%s&{{funcBSLDContextVar1}}=%u&{{funcBSLDContextVar2}}=%s>%s</a>\","
					"\"%s\": \"%s\","
					"\"%s\": \"%s\",\"%s\": \"%s\",\"%s\": \"%s\","
					"\"%s\": \"%s\",\"%s\": \"%s\",\"%s\": \"%s\"",
					field[2],field[3],
					field[4],field[3],field[3],field[1],{{funcBSLDContextVar1}},field[19],field[5],
					field[6],field[7],
					field[8],cForeignKey("tClient","cLabel",uLastUser),field[10],field[11],field[12],field[13],
					field[14],field[15],field[16],field[17],field[18],field[19]);
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

}//void json{{cTableName}}LDRows(void)


void json{{cTableName}}LDCols(void)
{
	printf("Content-type: text/json\n\n");
	printf("[\n");

	printf("\t{\"name\": \"cTable\", \"title\": \"Table\", \"filterable\": true, \"data-type\": \"html\", \"breakpoints\": \"\"},\n");
	printf("\t{\"name\": \"cTableLabel\", \"title\": \"TableLabel\", \"filterable\": true, \"data-type\": \"html\", \"breakpoints\": \"\"},\n");
	printf("\t{\"name\": \"cLastDate\", \"title\": \"LastDate\", \"filterable\": true, \"data-type\": \"text\", \"breakpoints\": \"\"},\n");
	printf("\t{\"name\": \"cLastUser\", \"title\": \"LastUser\", \"filterable\": true, \"data-type\": \"text\", \"breakpoints\": \"\"},\n");
	printf("\t{\"name\": \"cCreatedDate\", \"title\": \"CreatedDate\", \"filterable\": true, \"data-type\": \"text\", \"breakpoints\": \"all\"},\n");
	printf("\t{\"name\": \"cModDate\", \"title\": \"ModDate\", \"filterable\": true, \"data-type\": \"text\", \"breakpoints\": \"all\"},\n");
	printf("\t{\"name\": \"cCreatedBy\", \"title\": \"CreatedBy\", \"filterable\": true, \"data-type\": \"text\", \"breakpoints\": \"all\"},\n");
	printf("\t{\"name\": \"cModBy\", \"title\": \"ModBy\", \"filterable\": true, \"data-type\": \"text\", \"breakpoints\": \"all\"},\n");
	//last line no ,		
	printf("\t{\"name\": \"c{{funcBSLDContextVar2}}\", \"title\": \"c{{funcBSLDContextVar2}}\", \"filterable\": true, \"data-type\": \"html\", \"breakpoints\": \"all\"}\n");

	printf("]\n");
	exit(0);
}//void json{{cTableName}}LDCols(void)

