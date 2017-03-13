/*
FILE
	tIndexType source code of unxsRAD.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2009 for Unixservice
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tindextypefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uIndexType: Primary Key
static unsigned uIndexType=0;
//cLabel: Short label
static char cLabel[33]={""};
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



#define VAR_LIST_tIndexType "tIndexType.uIndexType,tIndexType.cLabel,tIndexType.uOwner,tIndexType.uCreatedBy,tIndexType.uCreatedDate,tIndexType.uModBy,tIndexType.uModDate"

 //Local only
void Insert_tIndexType(void);
void Update_tIndexType(char *cRowid);
void ProcesstIndexTypeListVars(pentry entries[], int x);

 //In tIndexTypefunc.c file included below
void ExtProcesstIndexTypeVars(pentry entries[], int x);
void ExttIndexTypeCommands(pentry entries[], int x);
void ExttIndexTypeButtons(void);
void ExttIndexTypeNavBar(void);
void ExttIndexTypeGetHook(entry gentries[], int x);
void ExttIndexTypeSelect(void);
void ExttIndexTypeSelectRow(void);
void ExttIndexTypeListSelect(void);
void ExttIndexTypeListFilter(void);
void ExttIndexTypeAuxTable(void);

#include "tindextypefunc.c"

 //Table Variables Assignment Function
void ProcesstIndexTypeVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uIndexType"))
			sscanf(entries[i].val,"%u",&uIndexType);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
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
	ExtProcesstIndexTypeVars(entries,x);

}//ProcesstIndexTypeVars()


void ProcesstIndexTypeListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uIndexType);
                        guMode=2002;
                        tIndexType("");
                }
        }
}//void ProcesstIndexTypeListVars(pentry entries[], int x)


int tIndexTypeCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttIndexTypeCommands(entries,x);

	if(!strcmp(gcFunction,"tIndexTypeTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tIndexTypeList();
		}

		//Default
		ProcesstIndexTypeVars(entries,x);
		tIndexType("");
	}
	else if(!strcmp(gcFunction,"tIndexTypeList"))
	{
		ProcessControlVars(entries,x);
		ProcesstIndexTypeListVars(entries,x);
		tIndexTypeList();
	}

	return(0);

}//tIndexTypeCommands()


void tIndexType(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttIndexTypeSelectRow();
		else
			ExttIndexTypeSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetIndexType();
				unxsRAD("New tIndexType table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tIndexType WHERE uIndexType=%u"
						,uIndexType);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uIndexType);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: Table Field Index Types",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tIndexTypeTools>");
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

        ExttIndexTypeButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tIndexType Record Data",100);

	if(guMode==2000 || guMode==2002)
		tIndexTypeInput(1);
	else
		tIndexTypeInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttIndexTypeAuxTable();

	Footer_ism3();

}//end of tIndexType();


void tIndexTypeInput(unsigned uMode)
{

//uIndexType
	OpenRow(LANG_FL_tIndexType_uIndexType,"black");
	printf("<input title='%s' type=text name=uIndexType value=%u size=16 maxlength=10 "
,LANG_FT_tIndexType_uIndexType,uIndexType);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uIndexType value=%u >\n",uIndexType);
	}
//cLabel
	OpenRow(LANG_FL_tIndexType_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tIndexType_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uOwner
	OpenRow(LANG_FL_tIndexType_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tIndexType_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tIndexType_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tIndexType_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tIndexType_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tIndexTypeInput(unsigned uMode)


void NewtIndexType(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uIndexType FROM tIndexType\
				WHERE uIndexType=%u"
							,uIndexType);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tIndexType("<blink>Record already exists");
		tIndexType(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tIndexType();
	//sprintf(gcQuery,"New record %u added");
	uIndexType=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tIndexType",uIndexType);
	unxsRADLog(uIndexType,"tIndexType","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uIndexType);
	tIndexType(gcQuery);
	}

}//NewtIndexType(unsigned uMode)


void DeletetIndexType(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tIndexType WHERE uIndexType=%u AND ( uOwner=%u OR %u>9 )"
					,uIndexType,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tIndexType WHERE uIndexType=%u"
					,uIndexType);
#endif
	macro_mySQLQueryHTMLError;
	//tIndexType("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsRADLog(uIndexType,"tIndexType","Del");
#endif
		tIndexType(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsRADLog(uIndexType,"tIndexType","DelError");
#endif
		tIndexType(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetIndexType(void)


void Insert_tIndexType(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tIndexType SET uIndexType=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uIndexType
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tIndexType(void)


void Update_tIndexType(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tIndexType SET uIndexType=%u,cLabel='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uIndexType
			,TextAreaSave(cLabel)
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tIndexType(void)


void ModtIndexType(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tIndexType.uIndexType,\
				tIndexType.uModDate\
				FROM tIndexType,tClient\
				WHERE tIndexType.uIndexType=%u\
				AND tIndexType.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uIndexType,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uIndexType,uModDate FROM tIndexType\
				WHERE uIndexType=%u"
						,uIndexType);
#else
	sprintf(gcQuery,"SELECT uIndexType FROM tIndexType\
				WHERE uIndexType=%u"
						,uIndexType);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tIndexType("<blink>Record does not exist");
	if(i<1) tIndexType(LANG_NBR_RECNOTEXIST);
	//if(i>1) tIndexType("<blink>Multiple rows!");
	if(i>1) tIndexType(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tIndexType(LANG_NBR_EXTMOD);
#endif

	Update_tIndexType(field[0]);
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tIndexType",uIndexType);
	unxsRADLog(uIndexType,"tIndexType","Mod");
#endif
	tIndexType(gcQuery);

}//ModtIndexType(void)


void tIndexTypeList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttIndexTypeListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tIndexTypeList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttIndexTypeListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uIndexType<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
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

}//tIndexTypeList()


void CreatetIndexType(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tIndexType ( uIndexType INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetIndexType()

