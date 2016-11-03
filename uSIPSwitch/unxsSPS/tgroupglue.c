/*
FILE
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in tgroupgluefunc.h
	while unxsSPS can still to be used to change this schema dependent file.
AUTHOR
	(C) 2001-2012 Gary Wallis for Unixservice, LLC.
TEMPLATE VARS AND FUNCTIONS
	ModuleCreateQuery
	ModuleInsertQuery
	ModuleListPrint
	ModuleListTable
	ModuleLoadVars
	ModuleProcVars
	ModuleInput
	ModuleUpdateQuery
	ModuleVars
	ModuleVarList
	cProject
	cTableKey
	cTableName
	cTableNameLC
	cTableTitle
*/


#include "mysqlrad.h"

//Table Variables
static unsigned uGroupGlue=0;
static unsigned uGroupType=0;
static char cuGroupTypePullDown[256]={""};
static unsigned uGroup=0;
static unsigned uKey=0;


#define VAR_LIST_tGroupGlue "tGroupGlue.uGroupGlue,tGroupGlue.uGroupType,tGroupGlue.uGroup,tGroupGlue.uKey"

 //Local only
void Insert_tGroupGlue(void);
void Update_tGroupGlue(char *cRowid);
void ProcesstGroupGlueListVars(pentry entries[], int x);

 //In tGroupGluefunc.h file included below
void ExtProcesstGroupGlueVars(pentry entries[], int x);
void ExttGroupGlueCommands(pentry entries[], int x);
void ExttGroupGlueButtons(void);
void ExttGroupGlueNavBar(void);
void ExttGroupGlueGetHook(entry gentries[], int x);
void ExttGroupGlueSelect(void);
void ExttGroupGlueSelectRow(void);
void ExttGroupGlueListSelect(void);
void ExttGroupGlueListFilter(void);
void ExttGroupGlueAuxTable(void);

#include "tgroupgluefunc.h"

 //Table Variables Assignment Function
void ProcesstGroupGlueVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		
		if(!strcmp(entries[i].name,"uGroupGlue"))
			sscanf(entries[i].val,"%u",&uGroupGlue);
		else if(!strcmp(entries[i].name,"uGroupType"))
			sscanf(entries[i].val,"%u",&uGroupType);
		else if(!strcmp(entries[i].name,"cuGroupTypePullDown"))
		{
			sprintf(cuGroupTypePullDown,"%.255s",entries[i].val);
			uGroupType=ReadPullDown("tGroupType","cLabel",cuGroupTypePullDown);
		}
		else if(!strcmp(entries[i].name,"uGroup"))
			sscanf(entries[i].val,"%u",&uGroup);
		else if(!strcmp(entries[i].name,"uKey"))
			sscanf(entries[i].val,"%u",&uKey);

	}

	//After so we can overwrite form data if needed.
	ExtProcesstGroupGlueVars(entries,x);

}//ProcesstGroupGlueVars()


void ProcesstGroupGlueListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uGroupGlue);
                        guMode=2002;
                        tGroupGlue("");
                }
        }
}//void ProcesstGroupGlueListVars(pentry entries[], int x)


int tGroupGlueCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttGroupGlueCommands(entries,x);

	if(!strcmp(gcFunction,"tGroupGlueTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tGroupGlueList();
		}

		//Default
		ProcesstGroupGlueVars(entries,x);
		tGroupGlue("");
	}
	else if(!strcmp(gcFunction,"tGroupGlueList"))
	{
		ProcessControlVars(entries,x);
		ProcesstGroupGlueListVars(entries,x);
		tGroupGlueList();
	}

	return(0);

}//tGroupGlueCommands()


void tGroupGlue(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttGroupGlueSelectRow();
		else
			ExttGroupGlueSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetGroupGlue();
				unxsSPS("New tGroupGlue table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tGroupGlue WHERE uGroupGlue=%u"
						,uGroupGlue);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			
		sscanf(field[0],"%u",&uGroupGlue);
		sscanf(field[1],"%u",&uGroupType);
		sscanf(field[2],"%u",&uGroup);
		sscanf(field[3],"%u",&uKey);

		}

	}//Internal Skip

	Header_ism3(":: Group glue table",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tGroupGlueTools>");
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

        ExttGroupGlueButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tGroupGlue Record Data",100);

	if(guMode==2000 || guMode==2002)
		tGroupGlueInput(1);
	else
		tGroupGlueInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttGroupGlueAuxTable();

	Footer_ism3();

}//end of tGroupGlue();


void tGroupGlueInput(unsigned uMode)
{

	
	//uGroupGlue uRADType=1001
	OpenRow(LANG_FL_tGroupGlue_uGroupGlue,"black");
	printf("<input title='%s' type=text name=uGroupGlue id=uGroupGlue value='%u' size=16 maxlength=10 "
		,LANG_FT_tGroupGlue_uGroupGlue,uGroupGlue);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uGroupGlue id=uGroupGlue value='%u' >\n",uGroupGlue);
	}
	//uGroupType COLTYPE_SELECTTABLE
	OpenRow(LANG_FL_tGroupGlue_uGroupType,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tGroupType;cuGroupTypePullDown","cLabel","cLabel",uGroupType,1);
	else
		tTablePullDown("tGroupType;cuGroupTypePullDown","cLabel","cLabel",uGroupType,0);
	//uGroup uRADType=3
	OpenRow(LANG_FL_tGroupGlue_uGroup,"black");
	printf("<input title='%s' type=text name=uGroup id=uGroup value='%u' size=16 maxlength=10 "
		,LANG_FT_tGroupGlue_uGroup,uGroup);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uGroup id=uGroup value='%u' >\n",uGroup);
	}
	//uKey uRADType=3
	OpenRow(LANG_FL_tGroupGlue_uKey,"black");
	printf("<input title='%s' type=text name=uKey id=uKey value='%u' size=16 maxlength=10 "
		,LANG_FT_tGroupGlue_uKey,uKey);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uKey id=uKey value='%u' >\n",uKey);
	}
	printf("</tr>\n");

}//void tGroupGlueInput(unsigned uMode)


void NewtGroupGlue(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uGroupGlue FROM tGroupGlue WHERE uGroupGlue=%u",uGroupGlue);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		tGroupGlue(LANG_NBR_RECEXISTS);

	Insert_tGroupGlue();
	uGroupGlue=mysql_insert_id(&gMysql);
#ifdef StandardFields
	uCreatedDate=luGetCreatedDate("tGroupGlue",uGroupGlue);
#endif
	unxsSPSLog(uGroupGlue,"tGroupGlue","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uGroupGlue);
		tGroupGlue(gcQuery);
	}

}//NewtGroupGlue(unsigned uMode)


void DeletetGroupGlue(void)
{
#ifdef StandardFields
	sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroupGlue=%u AND ( uOwner=%u OR %u>9 )"
					,uGroupGlue,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uGroupGlue=%u AND %u>9 )"
					,uGroupGlue,guPermLevel);
#endif
	macro_mySQLQueryHTMLError;
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsSPSLog(uGroupGlue,"tGroupGlue","Del");
		tGroupGlue(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsSPSLog(uGroupGlue,"tGroupGlue","DelError");
		tGroupGlue(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetGroupGlue(void)


void Insert_tGroupGlue(void)
{
	sprintf(gcQuery,"INSERT INTO tGroupGlue SET "
		"uGroupType=%u,"
		"uGroup=%u,"
		"uKey=%u"
			,uGroupType
			,uGroup
			,uKey
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tGroupGlue(void)


void Update_tGroupGlue(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tGroupGlue SET "
		"uGroupType=%u,"
		"uGroup=%u,"
		"uKey=%u"
		" WHERE _rowid=%s"
			,uGroupType
			,uGroup
			,uKey
			,cRowid
		);

	macro_mySQLQueryHTMLError;

}//void Update_tGroupGlue(void)


void ModtGroupGlue(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

#ifdef StandardFields
	unsigned uPreModDate=0;
	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tGroupGlue.uGroupGlue,"
				" tGroupGlue.uModDate"
				" FROM tGroupGlue,tClient"
				" WHERE tGroupGlue.uGroupGlue=%u"
				" AND tGroupGlue.uOwner=tClient.uClient"
				" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uGroupGlue,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uGroupGlue,uModDate FROM tGroupGlue"
				" WHERE uGroupGlue=%u"
					,uGroupGlue);
#else
	sprintf(gcQuery,"SELECT uGroupGlue FROM tGroupGlue"
				" WHERE uGroupGlue=%u"
					,uGroupGlue);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) tGroupGlue(LANG_NBR_RECNOTEXIST);
	if(i>1) tGroupGlue(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef StandardFields
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tGroupGlue(LANG_NBR_EXTMOD);
#endif

	Update_tGroupGlue(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef StandardFields
	uModDate=luGetModDate("tGroupGlue",uGroupGlue);
#endif
	unxsSPSLog(uGroupGlue,"tGroupGlue","Mod");
	tGroupGlue(gcQuery);

}//ModtGroupGlue(void)


void tGroupGlueList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttGroupGlueListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tGroupGlueList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttGroupGlueListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uGroupGlue"
		"<td><font face=arial,helvetica color=white>uGroupType"
		"<td><font face=arial,helvetica color=white>uGroup"
		"<td><font face=arial,helvetica color=white>uKey"
		"</tr>");



	mysql_data_seek(res,guStart-1);

	for(guN=0;guN<(guEnd-guStart+1);guN++)
	{
		field=mysql_fetch_row(res);
		if(!field)
		{
			printf("<tr><td><font face=arial,helvetica>End of data</table>");
			Footer_ism3();
		}
			if(guN % 2)
				printf("<tr bgcolor=#BBE1D3>");
			else
				printf("<tr>");
				printf("<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tGroupGlue&uGroupGlue=%s>%s</a><td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,ForeignKey("tGroupType","cLabel",strtoul(field[1],NULL,10))
			,field[2]
			,field[3]
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tGroupGlueList()


void CreatetGroupGlue(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tGroupGlue ("
		"uGroupGlue INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
		"uGroupType INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uGroupType),"
		"uGroup INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uGroup),"
		"uKey INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uKey) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetGroupGlue(void)


