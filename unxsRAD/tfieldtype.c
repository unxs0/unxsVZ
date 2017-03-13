/*
FILE
	tFieldType source code of unxsRAD.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2009 for Unixservice
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tfieldtypefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uFieldType: Primary Key
static unsigned uFieldType=0;
//cLabel: Short label
static char cLabel[33]={""};
//uRADType: Internal Define Number
static unsigned uRADType=0;
//cSQLCreatePart: Create Table SQL Snippet
static char *cSQLCreatePart={""};
//uOwner: Record owner
static unsigned uOwner=0;
//uCreatedBy: uClient for last insert
static unsigned uCreatedBy=0;
#define ISM3FIELDS
//uCreatedDate: Unix seconds date last insert
static time_t uCreatedDate=0;
//uModBy: uClient for last update
static unsigned uModBy=0;
//uModDate: Unix seconds date last update
static time_t uModDate=0;



#define VAR_LIST_tFieldType "tFieldType.uFieldType,tFieldType.cLabel,tFieldType.uRADType,tFieldType.cSQLCreatePart,tFieldType.uOwner,tFieldType.uCreatedBy,tFieldType.uCreatedDate,tFieldType.uModBy,tFieldType.uModDate"

 //Local only
void Insert_tFieldType(void);
void Update_tFieldType(char *cRowid);
void ProcesstFieldTypeListVars(pentry entries[], int x);

 //In tfieldtypefunc.c file included below
void ExtProcesstFieldTypeVars(pentry entries[], int x);
void ExttFieldTypeCommands(pentry entries[], int x);
void ExttFieldTypeButtons(void);
void ExttFieldTypeNavBar(void);
void ExttFieldTypeGetHook(entry gentries[], int x);
void ExttFieldTypeSelect(void);
void ExttFieldTypeSelectRow(void);
void ExttFieldTypeListSelect(void);
void ExttFieldTypeListFilter(void);
void ExttFieldTypeAuxTable(void);

#include "tfieldtypefunc.c"

 //Table Variables Assignment Function
void ProcesstFieldTypeVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uFieldType"))
			sscanf(entries[i].val,"%u",&uFieldType);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uRADType"))
			sscanf(entries[i].val,"%u",&uRADType);
		else if(!strcmp(entries[i].name,"cSQLCreatePart"))
			cSQLCreatePart=entries[i].val;
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
	ExtProcesstFieldTypeVars(entries,x);

}//ProcesstFieldTypeVars()


void ProcesstFieldTypeListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uFieldType);
                        guMode=2002;
                        tFieldType("");
                }
        }
}//void ProcesstFieldTypeListVars(pentry entries[], int x)


int tFieldTypeCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttFieldTypeCommands(entries,x);

	if(!strcmp(gcFunction,"tFieldTypeTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tFieldTypeList();
		}

		//Default
		ProcesstFieldTypeVars(entries,x);
		tFieldType("");
	}
	else if(!strcmp(gcFunction,"tFieldTypeList"))
	{
		ProcessControlVars(entries,x);
		ProcesstFieldTypeListVars(entries,x);
		tFieldTypeList();
	}

	return(0);

}//tFieldTypeCommands()


void tFieldType(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttFieldTypeSelectRow();
		else
			ExttFieldTypeSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetFieldType();
				unxsRAD("New tFieldType table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tFieldType WHERE uFieldType=%u"
						,uFieldType);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uFieldType);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uRADType);
		cSQLCreatePart=field[3];
		sscanf(field[4],"%u",&uOwner);
		sscanf(field[5],"%u",&uCreatedBy);
		sscanf(field[6],"%lu",&uCreatedDate);
		sscanf(field[7],"%u",&uModBy);
		sscanf(field[8],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tFieldType",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tFieldTypeTools>");
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

        ExttFieldTypeButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tFieldType Record Data",100);

	if(guMode==2000 || guMode==2002)
		tFieldTypeInput(1);
	else
		tFieldTypeInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttFieldTypeAuxTable();

	Footer_ism3();

}//end of tFieldType();


void tFieldTypeInput(unsigned uMode)
{

//uFieldType
	OpenRow(LANG_FL_tFieldType_uFieldType,"black");
	printf("<input title='%s' type=text name=uFieldType value=%u size=16 maxlength=10 "
,LANG_FT_tFieldType_uFieldType,uFieldType);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uFieldType value=%u >\n",uFieldType);
	}
//cLabel
	OpenRow(LANG_FL_tFieldType_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tFieldType_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uRADType
	OpenRow(LANG_FL_tFieldType_uRADType,"black");
	printf("<input title='%s' type=text name=uRADType value=%u size=16 maxlength=10 "
,LANG_FT_tFieldType_uRADType,uRADType);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uRADType value=%u >\n",uRADType);
	}
//cSQLCreatePart
	OpenRow(LANG_FL_tFieldType_cSQLCreatePart,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cSQLCreatePart "
,LANG_FT_tFieldType_cSQLCreatePart);
	if(guPermLevel>=7 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cSQLCreatePart);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cSQLCreatePart);
		printf("<input type=hidden name=cSQLCreatePart value=\"%s\" >\n",EncodeDoubleQuotes(cSQLCreatePart));
	}
//uOwner
	OpenRow(LANG_FL_tFieldType_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tFieldType_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tFieldType_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tFieldType_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tFieldType_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tFieldTypeInput(unsigned uMode)


void NewtFieldType(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uFieldType FROM tFieldType\
				WHERE uFieldType=%u"
							,uFieldType);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tFieldType("<blink>Record already exists");
		tFieldType(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tFieldType();
	//sprintf(gcQuery,"New record %u added");
	uFieldType=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tFieldType",uFieldType);
	unxsRADLog(uFieldType,"tFieldType","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uFieldType);
	tFieldType(gcQuery);
	}

}//NewtFieldType(unsigned uMode)


void DeletetFieldType(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tFieldType WHERE uFieldType=%u AND ( uOwner=%u OR %u>9 )"
					,uFieldType,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tFieldType WHERE uFieldType=%u"
					,uFieldType);
#endif
	macro_mySQLQueryHTMLError;
	//tFieldType("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsRADLog(uFieldType,"tFieldType","Del");
#endif
		tFieldType(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsRADLog(uFieldType,"tFieldType","DelError");
#endif
		tFieldType(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetFieldType(void)


void Insert_tFieldType(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tFieldType SET uFieldType=%u,cLabel='%s',uRADType=%u,cSQLCreatePart='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uFieldType
			,TextAreaSave(cLabel)
			,uRADType
			,TextAreaSave(cSQLCreatePart)
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tFieldType(void)


void Update_tFieldType(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tFieldType SET uFieldType=%u,cLabel='%s',uRADType=%u,cSQLCreatePart='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uFieldType
			,TextAreaSave(cLabel)
			,uRADType
			,TextAreaSave(cSQLCreatePart)
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tFieldType(void)


void ModtFieldType(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tFieldType.uFieldType,\
				tFieldType.uModDate\
				FROM tFieldType,tClient\
				WHERE tFieldType.uFieldType=%u\
				AND tFieldType.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uFieldType,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uFieldType,uModDate FROM tFieldType\
				WHERE uFieldType=%u"
						,uFieldType);
#else
	sprintf(gcQuery,"SELECT uFieldType FROM tFieldType\
				WHERE uFieldType=%u"
						,uFieldType);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tFieldType("<blink>Record does not exist");
	if(i<1) tFieldType(LANG_NBR_RECNOTEXIST);
	//if(i>1) tFieldType("<blink>Multiple rows!");
	if(i>1) tFieldType(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tFieldType(LANG_NBR_EXTMOD);
#endif

	Update_tFieldType(field[0]);
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tFieldType",uFieldType);
	unxsRADLog(uFieldType,"tFieldType","Mod");
#endif
	tFieldType(gcQuery);

}//ModtFieldType(void)


void tFieldTypeList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttFieldTypeListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tFieldTypeList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttFieldTypeListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uFieldType<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uRADType<td><font face=arial,helvetica color=white>cSQLCreatePart<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime6=strtoul(field[6],NULL,10);
		char cBuf6[32];
		if(luTime6)
			ctime_r(&luTime6,cBuf6);
		else
			sprintf(cBuf6,"---");
		time_t luTime8=strtoul(field[8],NULL,10);
		char cBuf8[32];
		if(luTime8)
			ctime_r(&luTime8,cBuf8);
		else
			sprintf(cBuf8,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,ForeignKey("tClient","cLabel",strtoul(field[4],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[5],NULL,10))
			,cBuf6
			,ForeignKey("tClient","cLabel",strtoul(field[7],NULL,10))
			,cBuf8
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tFieldTypeList()


void CreatetFieldType(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tFieldType ( uFieldType INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uRADType INT UNSIGNED NOT NULL DEFAULT 0, cSQLCreatePart TEXT NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetFieldType()

