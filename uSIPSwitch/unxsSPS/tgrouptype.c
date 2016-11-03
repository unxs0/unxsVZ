/*
FILE
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in tgrouptypefunc.h
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
static unsigned uGroupType=0;
static char cLabel[33]={""};
static unsigned uOwner=0;
#define StandardFields
static unsigned uCreatedBy=0;
static time_t uCreatedDate=0;
static unsigned uModBy=0;
static time_t uModDate=0;


#define VAR_LIST_tGroupType "tGroupType.uGroupType,tGroupType.cLabel,tGroupType.uOwner,tGroupType.uCreatedBy,tGroupType.uCreatedDate,tGroupType.uModBy,tGroupType.uModDate"

 //Local only
void Insert_tGroupType(void);
void Update_tGroupType(char *cRowid);
void ProcesstGroupTypeListVars(pentry entries[], int x);

 //In tGroupTypefunc.h file included below
void ExtProcesstGroupTypeVars(pentry entries[], int x);
void ExttGroupTypeCommands(pentry entries[], int x);
void ExttGroupTypeButtons(void);
void ExttGroupTypeNavBar(void);
void ExttGroupTypeGetHook(entry gentries[], int x);
void ExttGroupTypeSelect(void);
void ExttGroupTypeSelectRow(void);
void ExttGroupTypeListSelect(void);
void ExttGroupTypeListFilter(void);
void ExttGroupTypeAuxTable(void);

#include "tgrouptypefunc.h"

 //Table Variables Assignment Function
void ProcesstGroupTypeVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		
		if(!strcmp(entries[i].name,"uGroupType"))
			sscanf(entries[i].val,"%u",&uGroupType);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.40s",entries[i].val);
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
	ExtProcesstGroupTypeVars(entries,x);

}//ProcesstGroupTypeVars()


void ProcesstGroupTypeListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uGroupType);
                        guMode=2002;
                        tGroupType("");
                }
        }
}//void ProcesstGroupTypeListVars(pentry entries[], int x)


int tGroupTypeCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttGroupTypeCommands(entries,x);

	if(!strcmp(gcFunction,"tGroupTypeTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tGroupTypeList();
		}

		//Default
		ProcesstGroupTypeVars(entries,x);
		tGroupType("");
	}
	else if(!strcmp(gcFunction,"tGroupTypeList"))
	{
		ProcessControlVars(entries,x);
		ProcesstGroupTypeListVars(entries,x);
		tGroupTypeList();
	}

	return(0);

}//tGroupTypeCommands()


void tGroupType(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttGroupTypeSelectRow();
		else
			ExttGroupTypeSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetGroupType();
				unxsSPS("New tGroupType table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tGroupType WHERE uGroupType=%u"
						,uGroupType);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			
		sscanf(field[0],"%u",&uGroupType);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: Group types",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tGroupTypeTools>");
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

        ExttGroupTypeButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tGroupType Record Data",100);

	if(guMode==2000 || guMode==2002)
		tGroupTypeInput(1);
	else
		tGroupTypeInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttGroupTypeAuxTable();

	Footer_ism3();

}//end of tGroupType();


void tGroupTypeInput(unsigned uMode)
{

	
	//uGroupType uRADType=1001
	OpenRow(LANG_FL_tGroupType_uGroupType,"black");
	printf("<input title='%s' type=text name=uGroupType id=uGroupType value='%u' size=16 maxlength=10 "
		,LANG_FT_tGroupType_uGroupType,uGroupType);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uGroupType id=uGroupType value='%u' >\n",uGroupType);
	}
	//cLabel uRADType=253
	OpenRow(LANG_FL_tGroupType_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel id=cLabel value='%s' size=40 maxlength=32 "
		,LANG_FT_tGroupType_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel id=cLabel value='%s'>\n",EncodeDoubleQuotes(cLabel));
	}
	//uOwner COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tGroupType_uOwner,"black");
	printf("%s<input type=hidden name=uOwner id=uOwner value='%u' >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tGroupType_uCreatedBy,"black");
	printf("%s<input type=hidden name=uCreatedBy id=uCreatedBy value='%u' >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tGroupType_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate id=uCreatedDate value='%lu' >\n",uCreatedDate);
	//uModBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tGroupType_uModBy,"black");
	printf("%s<input type=hidden name=uModBy id=uModBy value='%u' >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tGroupType_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate id=uModDate value='%lu' >\n",uModDate);
	printf("</tr>\n");

}//void tGroupTypeInput(unsigned uMode)


void NewtGroupType(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uGroupType FROM tGroupType WHERE uGroupType=%u",uGroupType);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		tGroupType(LANG_NBR_RECEXISTS);

	Insert_tGroupType();
	uGroupType=mysql_insert_id(&gMysql);
#ifdef StandardFields
	uCreatedDate=luGetCreatedDate("tGroupType",uGroupType);
#endif
	unxsSPSLog(uGroupType,"tGroupType","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uGroupType);
		tGroupType(gcQuery);
	}

}//NewtGroupType(unsigned uMode)


void DeletetGroupType(void)
{
#ifdef StandardFields
	sprintf(gcQuery,"DELETE FROM tGroupType WHERE uGroupType=%u AND ( uOwner=%u OR %u>9 )"
					,uGroupType,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tGroupType WHERE uGroupType=%u AND %u>9 )"
					,uGroupType,guPermLevel);
#endif
	macro_mySQLQueryHTMLError;
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsSPSLog(uGroupType,"tGroupType","Del");
		tGroupType(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsSPSLog(uGroupType,"tGroupType","DelError");
		tGroupType(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetGroupType(void)


void Insert_tGroupType(void)
{
	sprintf(gcQuery,"INSERT INTO tGroupType SET "
		"cLabel='%s',"
		"uOwner=%u,"
		"uCreatedBy=%u,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tGroupType(void)


void Update_tGroupType(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tGroupType SET "
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

}//void Update_tGroupType(void)


void ModtGroupType(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

#ifdef StandardFields
	unsigned uPreModDate=0;
	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tGroupType.uGroupType,"
				" tGroupType.uModDate"
				" FROM tGroupType,tClient"
				" WHERE tGroupType.uGroupType=%u"
				" AND tGroupType.uOwner=tClient.uClient"
				" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uGroupType,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uGroupType,uModDate FROM tGroupType"
				" WHERE uGroupType=%u"
					,uGroupType);
#else
	sprintf(gcQuery,"SELECT uGroupType FROM tGroupType"
				" WHERE uGroupType=%u"
					,uGroupType);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) tGroupType(LANG_NBR_RECNOTEXIST);
	if(i>1) tGroupType(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef StandardFields
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tGroupType(LANG_NBR_EXTMOD);
#endif

	Update_tGroupType(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef StandardFields
	uModDate=luGetModDate("tGroupType",uGroupType);
#endif
	unxsSPSLog(uGroupType,"tGroupType","Mod");
	tGroupType(gcQuery);

}//ModtGroupType(void)


void tGroupTypeList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttGroupTypeListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tGroupTypeList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttGroupTypeListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uGroupType"
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
		printf("<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tGroupType&uGroupType=%s>%s</a><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
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

}//tGroupTypeList()


void CreatetGroupType(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tGroupType ("
		"uGroupType INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
		"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
		"uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetGroupType(void)


