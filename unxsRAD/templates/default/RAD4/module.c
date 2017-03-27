/*
FILE
	{{cTableNameLC}}.c
	From unxsRAD RAD4 module.c template
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in {{cTableNameLC}}func.h
	while unxsRAD can still to be used to change this schema dependent file.
AUTHOR
	Template (C) 2001-2017 Gary Wallis for Unixservice, LLC.
TEMPLATE VARS AND FUNCTIONS
	funcModuleInput
	funcModuleCreateQuery
	funcModuleInsertQuery
	funcModuleListPrint
	funcModuleListTable
	funcModuleLoadVars
	funcModuleProcVars
	funcModuleUpdateQuery
	funcModuleVars
	funcModuleVarList
	cProject
	cTableKey
	cTableName
	cTableNameLC
	cTableTitle
*/


#include "mysqlrad.h"

//Table Variables
{{funcModuleVars}}

#define VAR_LIST_{{cTableName}} "{{funcModuleVarList}}"

 //Local only
void Insert_{{cTableName}}(void);
void Update_{{cTableName}}(char *cRowid);
void Process{{cTableName}}ListVars(pentry entries[], int x);

 //In {{cTableName}}func.h file included below
void ExtProcess{{cTableName}}Vars(pentry entries[], int x);
void Ext{{cTableName}}Commands(pentry entries[], int x);
void Ext{{cTableName}}Buttons(void);
void Ext{{cTableName}}NavBar(void);
void Ext{{cTableName}}GetHook(entry gentries[], int x);
void Ext{{cTableName}}Select(void);
void Ext{{cTableName}}SelectRow(void);
void Ext{{cTableName}}ListSelect(void);
void Ext{{cTableName}}ListFilter(void);
void Ext{{cTableName}}AuxTable(void);

#include "{{cTableNameLC}}func.h"

 //Table Variables Assignment Function
void Process{{cTableName}}Vars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		{{funcModuleProcVars}}
	}

	//After so we can overwrite form data if needed.
	ExtProcess{{cTableName}}Vars(entries,x);

}//Process{{cTableName}}Vars()


void Process{{cTableName}}ListVars(pentry entries[], int x)
{
        //register int i;
        //for(i=0;i<x;i++)
        //{
        //	if(!strcmp(entries[i].name,"example"))
        //	{
        //	}
        //}
}//void Process{{cTableName}}ListVars(pentry entries[], int x)


int {{cTableName}}Commands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	Ext{{cTableName}}Commands(entries,x);

	if(!strcmp(gcFunction,"{{cTableName}}Tools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			{{cTableName}}List();
		}

		//Default
		Process{{cTableName}}Vars(entries,x);
		{{cTableName}}("");
	}
	else if(!strcmp(gcFunction,"{{cTableName}}List"))
	{
		ProcessControlVars(entries,x);
		Process{{cTableName}}ListVars(entries,x);
		{{cTableName}}List();
	}

	return(0);

}//{{cTableName}}Commands()


void {{cTableName}}Input(unsigned uMode)
{
	{{funcModuleInput}}

}//void {{cTableName}}Input(unsigned uMode)


void {{cTableName}}(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			Ext{{cTableName}}SelectRow();
		else
			Ext{{cTableName}}Select();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				Create{{cTableName}}();
				{{cProject}}("New {{cTableName}} table created");
                	}
			else
			{
				htmlPlainTextError(mysql_error(&gMysql));
			}
        	}

		res=mysql_store_result(&gMysql);
		if((guI=mysql_num_rows(res)))
		{
			if(guMode==6)
			{
			sprintf(gcQuery,"SELECT _rowid FROM {{cTableName}} WHERE {{cTableKey}}=%u"
						,{{cTableKey}});
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			{{funcModuleLoadVars}}
		}

	}//Internal Skip

	HeaderRAD4(":: {{cTableTitle}}",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value={{cTableName}}Tools>");
	printf("<input type=hidden name=gluRowid value=%lu>",gluRowid);
	if(guI)
	{
		if(guMode==6)
			//printf(" Found");
			printf(LANG_NBR_FOUND);
		else if(guMode==5)
			//printf(" Modified");
			printf(LANG_NBR_MODIFIED);
		else if(guMode==4)
			//printf(" New");
			printf(LANG_NBR_NEW);
		printf(LANG_NBRF_SHOWING,gluRowid,guI);
	}
	else
	{
		if(!cResult[0])
		//printf(" No records found");
		printf(LANG_NBR_NORECS);
	}
	if(cResult[0]) printf("%s",cResult);
	printf("</td></tr>");
	printf("<tr><td valign=top width=25%%>");

        Ext{{cTableName}}Buttons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("{{cTableName}} Record Data",100);

	if(guMode==2000 || guMode==2002)
		{{cTableName}}Input(1);
	else
		{{cTableName}}Input(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        Ext{{cTableName}}AuxTable();

	FooterRAD4();

}//end of {{cTableName}}();


void New{{cTableName}}(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT {{cTableKey}} FROM {{cTableName}} WHERE {{cTableKey}}=%u",{{cTableKey}});
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		{{cTableName}}(LANG_NBR_RECEXISTS);

	Insert_{{cTableName}}();
	{{cTableKey}}=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("{{cTableName}}",{{cTableKey}});
	{{cProject}}Log({{cTableKey}},"{{cTableName}}","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,{{cTableKey}});
		{{cTableName}}(gcQuery);
	}

}//New{{cTableName}}(unsigned uMode)


void Delete{{cTableName}}(void)
{
	sprintf(gcQuery,"DELETE FROM {{cTableName}} WHERE {{cTableKey}}=%u AND ( uOwner=%u OR %u>9 )"
					,{{cTableKey}},guLoginClient,guPermLevel);
	macro_mySQLQueryHTMLError;
	if(mysql_affected_rows(&gMysql)>0)
	{
		{{cProject}}Log({{cTableKey}},"{{cTableName}}","Del");
		{{cTableName}}(LANG_NBR_RECDELETED);
	}
	else
	{
		{{cProject}}Log({{cTableKey}},"{{cTableName}}","DelError");
		{{cTableName}}(LANG_NBR_RECNOTDELETED);
	}

}//void Delete{{cTableName}}(void)


void Insert_{{cTableName}}(void)
{
	{{funcModuleInsertQuery}}
	macro_mySQLQueryHTMLError;

}//void Insert_{{cTableName}}(void)


void Update_{{cTableName}}(char *cRowid)
{
	{{funcModuleUpdateQuery}}
	macro_mySQLQueryHTMLError;

}//void Update_{{cTableName}}(void)


void Mod{{cTableName}}(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT {{cTableName}}.{{cTableKey}},"
				" {{cTableName}}.uModDate"
				" FROM {{cTableName}},tClient"
				" WHERE {{cTableName}}.{{cTableKey}}=%u"
				" AND {{cTableName}}.uOwner=tClient.uClient"
				" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,{{cTableKey}},guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT {{cTableKey}},uModDate FROM {{cTableName}}"
				" WHERE {{cTableKey}}=%u"
					,{{cTableKey}});

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) {{cTableName}}(LANG_NBR_RECNOTEXIST);
	if(i>1) {{cTableName}}(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) {{cTableName}}(LANG_NBR_EXTMOD);

	Update_{{cTableName}}(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("{{cTableName}}",{{cTableKey}});
	{{cProject}}Log({{cTableKey}},"{{cTableName}}","Mod");
	{{cTableName}}(gcQuery);

}//Mod{{cTableName}}(void)


void {{cTableName}}List(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	Ext{{cTableName}}ListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("{{cTableName}}List",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	Ext{{cTableName}}ListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	{{funcModuleListTable}}

	mysql_data_seek(res,guStart-1);

	for(guN=0;guN<(guEnd-guStart+1);guN++)
	{
		field=mysql_fetch_row(res);
		if(!field)
		{
			printf("<tr><td><font face=arial,helvetica>End of data</table>");
			FooterRAD4();
		}
			if(guN % 2)
				printf("<tr bgcolor=#BBE1D3>");
			else
				printf("<tr>");
		{{funcModuleListPrint}}
	}

	printf("</table></form>\n");
	FooterRAD4();

}//{{cTableName}}List()


{{funcModuleCreateQuery}}

