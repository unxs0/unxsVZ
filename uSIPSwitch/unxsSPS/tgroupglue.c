/*
FILE
	$Id: module.c 2115 2012-09-19 14:11:03Z Gary $
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
static char cLabel[33]={""};
static unsigned uOwner=0;
#define StandardFields
static unsigned uCreatedBy=0;
static time_t uCreatedDate=0;
static unsigned uModBy=0;
static time_t uModDate=0;


#define VAR_LIST_tGroupGlue "tGroupGlue.uGroupGlue,tGroupGlue.cLabel,tGroupGlue.uOwner,tGroupGlue.uCreatedBy,tGroupGlue.uCreatedDate,tGroupGlue.uModBy,tGroupGlue.uModDate"

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
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.0s",entries[i].val);
		else if(!strcmp(entries[i].name,"uOwner"))
			sscanf(entries[i].val,"%u",&uOwner);
		else if(!strcmp(entries[i].name,"uCreatedBy"))
			sscanf(entries[i].val,"%u",&uCreatedBy);
		else if(!strcmp(entries[i].name,"uCreatedDate"))
			sscanf(entries[i].val,"%lu",&uCreatedDate);
		else if(!strcmp(entries[i].name,"uModBy"))
			sscanf(entries[i].val,"%u",&uModBy);
		else if(!strcmp(entries[i].name,"uModDate"))
			sscanf(entries[i].val,"%lu",&uModDate);

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
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

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
	printf("<input title='%s' type=text name=uGroupGlue value='%u' size=16 maxlength=10 "
		,LANG_FT_tGroupGlue_uGroupGlue,uGroupGlue);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uGroupGlue value='%u' >\n",uGroupGlue);
	}
	//cLabel uRADType=253
	OpenRow(LANG_FL_tGroupGlue_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value='%s' size=0 maxlength=0 "
		,LANG_FT_tGroupGlue_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value='%s'>\n",EncodeDoubleQuotes(cLabel));
	}
	//uOwner COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tGroupGlue_uOwner,"black");
	printf("%s<input type=hidden name=uOwner value='%u' >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tGroupGlue_uCreatedBy,"black");
	printf("%s<input type=hidden name=uCreatedBy value='%u' >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tGroupGlue_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value='%lu' >\n",uCreatedDate);
	//uModBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tGroupGlue_uModBy,"black");
	printf("%s<input type=hidden name=uModBy value='%u' >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tGroupGlue_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value='%lu' >\n",uModDate);
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
		"cLabel='%s',"
		"uOwner=%u,"
		"uCreatedBy=%u,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tGroupGlue(void)


void Update_tGroupGlue(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tGroupGlue SET "
		"cLabel='%s',"
		"uOwner=%u,"
		"uModBy=%u,"
		"uModDate=UNIX_TIMESTAMP(NOW())"
		" WHERE _rowid=%s"
			,TextAreaSave(cLabel)
			,uOwner
			,uModBy
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
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>uOwner"
		"<td><font face=arial,helvetica color=white>uCreatedBy"
		"<td><font face=arial,helvetica color=white>uCreatedDate"
		"<td><font face=arial,helvetica color=white>uModBy"
		"<td><font face=arial,helvetica color=white>uModDate"
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
				time_t luTime4=strtoul(field[4],NULL,10);
		char cBuf4[32];
		if(luTime4)
			ctime_r(&luTime4,cBuf4);
		else
			sprintf(cBuf4,"---");
		time_t luTime6=strtoul(field[6],NULL,10);
		char cBuf6[32];
		if(luTime6)
			ctime_r(&luTime6,cBuf6);
		else
			sprintf(cBuf6,"---");
		printf("<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tGroupGlue&uGroupGlue=%s>%s</a><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tClient","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[3],NULL,10))
			,cBuf4
			,ForeignKey("tClient","cLabel",strtoul(field[5],NULL,10))
			,cBuf6
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tGroupGlueList()


void CreatetGroupGlue(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tGroupGlue ("
		"uGroupGlue INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
		"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
		"uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetGroupGlue(void)


