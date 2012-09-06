/*
FILE
	$Id$
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis for Unixservice.
 
*/

static unsigned guTable=0;
static char cuTablePullDown[256]={""};
static char gcTableName[32]={""};
static char gcTableNameLC[32]={""};
static char gcTableKey[33]={""};

//ModuleFunctionProtos()
unsigned CreateFileFromTemplate(unsigned uTemplate,unsigned uTable);


void tTemplateNavList(void);

void ExtProcesstTemplateVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uTable"))
			sscanf(entries[i].val,"%u",&guTable);
		else if(!strcmp(entries[i].name,"cuTablePullDown"))
		{
			sprintf(cuTablePullDown,"%.255s",entries[i].val);
			guTable=ReadPullDown("tTable","cLabel",cuTablePullDown);
		}
	}
}//void ExtProcesstTemplateVars(pentry entries[], int x)


void ExttTemplateCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tTemplateTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstTemplateVars(entries,x);
                        	guMode=2000;
	                        tTemplate(LANG_NB_CONFIRMNEW);
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Create File"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstTemplateVars(entries,x);
                        	guMode=0;
				if(!guTable)
	                        	tTemplate("Must select a table");
				if(!uTemplate)
	                        	tTemplate("Must load a template");

				if(CreateFileFromTemplate(uTemplate,guTable))
	                        	tTemplate("Create file error");
				else
	                        	tTemplate("Create file ok");
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstTemplateVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uTemplate=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtTemplate(0);
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstTemplateVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tTemplate(LANG_NB_CONFIRMDEL);
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstTemplateVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetTemplate();
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstTemplateVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tTemplate(LANG_NB_CONFIRMMOD);
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstTemplateVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtTemplate();
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttTemplateCommands(pentry entries[], int x)


void ExttTemplateButtons(void)
{
	OpenFieldSet("tTemplate Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review changes</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("<p><u>Record Context Info</u><br>");
			printf("<p><u>Operations</u><br>");
			tTablePullDownOwner("tTable;cuTablePullDown","cLabel","cLabel",guTable,1);
			printf("<br><input type=submit class=largeButton title='Create /tmp debug file from this template based on cLabel'"
				" name=gcCommand value='Create File'>");
			tTemplateNavList();
	}
	CloseFieldSet();

}//void ExttTemplateButtons(void)


void ExttTemplateAuxTable(void)
{

}//void ExttTemplateAuxTable(void)


void ExttTemplateGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uTemplate"))
		{
			sscanf(gentries[i].val,"%u",&uTemplate);
			guMode=6;
		}
	}
	tTemplate("");

}//void ExttTemplateGetHook(entry gentries[], int x)


void ExttTemplateSelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTemplate ORDER BY"
				" uTemplate",
				VAR_LIST_tTemplate);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tTemplate,tClient WHERE tTemplate.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uTemplate",
					VAR_LIST_tTemplate,uContactParentCompany,uContactParentCompany);
					

}//void ExttTemplateSelect(void)


void ExttTemplateSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tTemplate WHERE uTemplate=%u",
			VAR_LIST_tTemplate,uTemplate);
	else
                sprintf(gcQuery,"SELECT %s FROM tTemplate,tClient"
                                " WHERE tTemplate.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tTemplate.uTemplate=%u",
                        		VAR_LIST_tTemplate
					,uContactParentCompany,uContactParentCompany
					,uTemplate);

}//void ExttTemplateSelectRow(void)


void ExttTemplateListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTemplate",
				VAR_LIST_tTemplate);
	else
		sprintf(gcQuery,"SELECT %s FROM tTemplate,tClient"
				" WHERE tTemplate.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tTemplate
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttTemplateListFilter()
        if(!strcmp(gcFilter,"uTemplate"))
        {
                sscanf(gcCommand,"%u",&uTemplate);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tTemplate.uTemplate=%u"
						" ORDER BY uTemplate",
						uTemplate);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uTemplate");
        }

}//void ExttTemplateListSelect(void)


void ExttTemplateListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uTemplate"))
                printf("<option>uTemplate</option>");
        else
                printf("<option selected>uTemplate</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttTemplateListFilter(void)


void ExttTemplateNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

	if(uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);

	if(uAllowDel(uOwner,uCreatedBy)) 
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttTemplateNavBar(void)


void tTemplateNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uTemplate,cLabel FROM tTemplate ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tTemplate.uTemplate,"
				" tTemplate.cLabel"
				" FROM tTemplate,tClient"
				" WHERE tTemplate.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tTemplateNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tTemplateNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsRAD.cgi?gcFunction=tTemplate"
				"&uTemplate=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tTemplateNavList(void)

#include <openisp/template.h>

unsigned CreateModuleFile(unsigned uTemplate, unsigned uTable)
{
	FILE *fp;
	unsigned uRetVal= -1;

	if(!uTable || !uTemplate) return(uRetVal);

	char cFile[100]={""};

	sprintf(gcTableName,"%.31s",ForeignKey("tTable","cLabel",uTable));
	sprintf(gcTableNameLC,"%.31s",gcTableName);
	WordToLower(gcTableNameLC);

	sprintf(cFile,"/tmp/%.31s.c",gcTableNameLC);
	if((fp=fopen(cFile,"w"))==NULL)
		return(1);

       	MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cTemplate FROM tTemplate WHERE uTemplate=%u",uTemplate);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
		fprintf(fp,"%s\n",mysql_error(&gMysql));
		return(uRetVal);
	}
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		struct t_template template;

/*
	funcModuleCreateQuery
	funcModuleInsertQuery
	funcModuleListPrint
	funcModuleListTable
	funcModuleLoadVars
	funcModuleProcVars
	funcModuleRAD3Input
	funcModuleUpdateQuery
	funcModuleVars
	funcModuleVarList
	cProject
	cTableKey
	cTableName
	cTableNameLC
	cTableTitle
*/
			
		template.cpName[0]="cTableName";
		template.cpValue[0]=gcTableName;
			
		template.cpName[1]="cTableNameLC";
		template.cpValue[1]=gcTableNameLC;
			
		template.cpName[2]="cTableKey";
		sprintf(gcTableKey,"u%.31s",gcTableName+1);//New table name includes table type t prefix
		template.cpValue[2]=gcTableKey;
			
		template.cpName[3]="";

		Template(field[0],&template,fp);
		uRetVal=0;
	}
	else
	{
		fprintf(fp,"No such uTemplate %u\n",uTemplate);
		uRetVal=2;
	}
	mysql_free_result(res);
	fclose(fp);

	return(uRetVal);

}//unsigned CreateModuleFile()


unsigned CreateModuleFuncFile(unsigned uTemplate, unsigned uTable)
{
	unsigned uRetVal= -1;

	if(!uTable || !uTemplate) return(uRetVal);
	return(0);

}//unsigned CreateModuleFuncFile()


unsigned CreateFileFromTemplate(unsigned uTemplate,unsigned uTable)
{
	char cFile[32];
	unsigned uRetVal= -1;

	if(!uTable || !uTemplate) return(uRetVal);

	sprintf(cFile,"%.31s",ForeignKey("tTemplate","cLabel",uTemplate));

	if(!strcmp(cFile,"module.c"))
		uRetVal=CreateModuleFile(uTemplate,uTable);
	else if(!strcmp(cFile,"modulefunc.h"))
		uRetVal=CreateModuleFuncFile(uTemplate,uTable);

	return(uRetVal);

}//unsigned CreateFileFromTemplate()


/*
	funcModuleCreateQuery
	funcModuleInsertQuery
	funcModuleListPrint
	funcModuleListTable
	funcModuleLoadVars
	funcModuleProcVars
	funcModuleRAD3Input
	funcModuleUpdateQuery
	funcModuleVars
	funcModuleVarList
*/
void funcModuleListPrint(FILE *fp)
{
}//void funcModuleListPrint(FILE *fp)

void funcModuleListTable(FILE *fp)
{
}//void funcModuleListTable(FILE *fp)

void funcModuleLoadVars(FILE *fp)
{
}//void funcModuleLoadVars(FILE *fp)

void funcModuleProcVars(FILE *fp)
{
}//void funcModuleProcVars(FILE *fp)

void funcModuleRAD3Input(FILE *fp)
{
}//void funcModuleRAD3Input(FILE *fp)

void funcModuleVars(FILE *fp)
{
}//void funcModuleVars(FILE *fp)

void funcModuleVarList(FILE *fp)
{
}//void funcModuleVarList(FILE *fp)

/*
	sprintf(gcQuery,"UPDATE tTable SET uTable=%u,cLabel='%s',uProject=%u,uTableOrder=%u,uSourceLock=%u,cDescription='%s',cLegend='%s',cToolTip='%s',uNewLevel=%u,uModLevel=%u,uDelLevel=%u,uReadLevel=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uTable
			,TextAreaSave(cLabel)
			,uProject
			,uTableOrder
			,uSourceLock
			,TextAreaSave(cDescription)
			,TextAreaSave(cLegend)
			,TextAreaSave(cToolTip)
			,uNewLevel
			,uModLevel
			,uDelLevel
			,uReadLevel
			,uModBy
			,cRowid);

*/
void funcModuleUpdateQuery(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	fprintf(fp,"sprintf(gcQuery,\"UPDATE %s SET\"\n",gcTableName);

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=0;
	unsigned uRADType=0;
	while((field=mysql_fetch_row(res)))
	{
		//Special internal fields
		if(!strcmp(field[0],"uCreatedBy"))
			continue;
		if(!strcmp(field[0],"uCreatedDate"))
			continue;
		if(uFirst)
			fprintf(fp,",\"\n");
		sscanf(field[1],"%u",&uRADType);
		switch(uRADType)
		{
			default:
			case(COLTYPE_RADPRI):
				fprintf(fp,"\t\t\"%s=%%u",field[0]);
			break;
			case(COLTYPE_VARCHAR):
				fprintf(fp,"\t\t\"%s='%%s'",field[0]);
			break;
			case(COLTYPE_UNIXTIMEUPDATE):
				fprintf(fp,"\t\t\"%s=UNIX_TIMESTAMP(NOW())",field[0]);
			break;
		}
		uFirst=1;
	}

	fprintf(fp,"\",\n");
	uFirst=0;
	uRADType=0;
	mysql_data_seek(res,0);
	while((field=mysql_fetch_row(res)))
	{
		//Special internal fields
		if(!strcmp(field[0],"uCreatedBy"))
			continue;
		if(!strcmp(field[0],"uCreatedDate"))
			continue;
		if(uFirst)
			fprintf(fp,",\n");
		sscanf(field[1],"%u",&uRADType);
		switch(uRADType)
		{
			default:
			case(COLTYPE_RADPRI):
				fprintf(fp,"\t\t\t%s",field[0]);
			break;
			case(COLTYPE_VARCHAR):
				fprintf(fp,"\t\t\tTextAreasave(%s)",field[0]);
			break;
		}
		uFirst=1;
	}
	mysql_free_result(res);

	fprintf(fp,");\n");
}//void funcModuleUpdateQuery(FILE *fp)


void funcModuleInsertQuery(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	fprintf(fp,"sprintf(gcQuery,\"INSERT INTO %s SET\"\n",gcTableName);

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.uRADType"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=0;
	unsigned uRADType=0;
	while((field=mysql_fetch_row(res)))
	{
		//Special internal fields
		if(!strcmp(field[0],"uModBy"))
			continue;
		if(!strcmp(field[0],"uModDate"))
			continue;
		if(uFirst)
			fprintf(fp,",\"\n");
		sscanf(field[1],"%u",&uRADType);
		switch(uRADType)
		{
			default:
			case(COLTYPE_RADPRI):
				fprintf(fp,"\t\t\"%s=%%u",field[0]);
			break;
			case(COLTYPE_VARCHAR):
				fprintf(fp,"\t\t\"%s='%%s'",field[0]);
			break;
			case(COLTYPE_UNIXTIMECREATE):
				fprintf(fp,"\t\t\"%s=UNIX_TIMESTAMP(NOW())",field[0]);
			break;
		}
		uFirst=1;
	}

	fprintf(fp,"\",\n");
	uFirst=0;
	uRADType=0;
	mysql_data_seek(res,0);
	while((field=mysql_fetch_row(res)))
	{
		//Special internal fields
		if(!strcmp(field[0],"uModBy"))
			continue;
		if(!strcmp(field[0],"uModDate"))
			continue;
		if(uFirst)
			fprintf(fp,",\n");
		sscanf(field[1],"%u",&uRADType);
		switch(uRADType)
		{
			default:
			case(COLTYPE_RADPRI):
				fprintf(fp,"\t\t\t%s",field[0]);
			break;
			case(COLTYPE_VARCHAR):
				fprintf(fp,"\t\t\tTextAreasave(%s)",field[0]);
			break;
			case(COLTYPE_UNIXTIMECREATE):
				uFirst=0;//Skip comma
				continue;
			break;
		}
		uFirst=1;
	}
	mysql_free_result(res);

	fprintf(fp,");\n");

}//void funcModuleInsertQuery(FILE *fp)


void funcModuleCreateQuery(FILE *fp)
{
       	MYSQL_RES *res;
        MYSQL_ROW field;

	fprintf(fp,"void Create%s(void)\n{\n",gcTableName);
	fprintf(fp,"\tsprintf(gcQuery,\"CREATE TABLE IF NOT EXISTS %s (\"\n",gcTableName);

	sprintf(gcQuery,"SELECT tField.cLabel,tFieldType.cSQLCreatePart,"
			"tFieldType.uRADType,tField.uSQLSize,"
			"tField.uIndexType,tField.cExtIndex"
			" FROM tField,tTable,tFieldType"
			" WHERE tField.uTable=tTable.uTable"
			" AND tField.uFieldType=tFieldType.uFieldType"
			" AND tTable.uTable=%u"
			" ORDER BY tField.uOrder",guTable);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
	{
                fprintf(fp,"%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	unsigned uFirst=0;
	unsigned uRADType=0;
	unsigned uIndexType=0;
	while((field=mysql_fetch_row(res)))
	{
		if(uFirst)
			fprintf(fp,",\"\n");
		sscanf(field[2],"%u",&uRADType);
		sscanf(field[4],"%u",&uIndexType);
		switch(uRADType)
		{
			default:
			case(COLTYPE_RADPRI):
				fprintf(fp,"\t\t\"%s INT UNSIGNED %s",field[0],field[1]);
			break;
			case(COLTYPE_VARCHAR):
				fprintf(fp,"\t\t\"%s VARCHAR(%s) %s",field[0],field[3],field[1]);
			break;
		}
		switch(uIndexType)
		{
			case(1):
				fprintf(fp,", INDEX (%s)",field[0]);
			break;
			case(2):
				fprintf(fp,", UNIQUE (%s)",field[0]);
			break;
			default:
			break;
		}
		//cExtIndex
		if(field[5][0])
			fprintf(fp,", %s",field[5]);
		uFirst=1;
	}
	fprintf(fp," )\");\n");
	mysql_free_result(res);

	fprintf(fp,"\tmysql_query(&gMysql,gcQuery);\n");
	fprintf(fp,"\tif(mysql_errno(&gMysql))\n");
	fprintf(fp,"\t\thtmlPlainTextError(mysql_error(&gMysql));\n");

	fprintf(fp,"}//void Create%s(void)\n\n",gcTableName);

}//void funcModuleCreateQuery(FILE *fp)


//libtemplate.a required
void AppFunctions(FILE *fp,char *cFunction)
{
	if(!strcmp(cFunction,"funcModuleCreateQuery"))
		funcModuleCreateQuery(fp);
	else if(!strcmp(cFunction,"funcModuleInsertQuery"))
		funcModuleInsertQuery(fp);
	else if(!strcmp(cFunction,"funcModuleListPrint"))
		funcModuleListPrint(fp);
	else if(!strcmp(cFunction,"funcModuleListTable"))
		funcModuleListTable(fp);
	else if(!strcmp(cFunction,"funcModuleLoadVars"))
		funcModuleLoadVars(fp);
	else if(!strcmp(cFunction,"funcModuleProcVars"))
		funcModuleProcVars(fp);
	else if(!strcmp(cFunction,"funcModuleRAD3Input"))
		funcModuleRAD3Input(fp);
	else if(!strcmp(cFunction,"funcModuleUpdateQuery"))
		funcModuleUpdateQuery(fp);
	else if(!strcmp(cFunction,"funcModuleVars"))
		funcModuleVars(fp);
	else if(!strcmp(cFunction,"funcModuleVarList"))
		funcModuleVarList(fp);

}//void AppFunctions(FILE *fp,char *cFunction)


