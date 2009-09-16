/*
FILE
	tContentType source code of unxsBlog.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id: module.c.template 1292 2007-07-09 23:31:18Z Gary $
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tcontenttypefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uContentType: Primary Key
static unsigned uContentType=0;
//cLabel: Short label
static char cLabel[33]={""};
//uVisible: Togles the visualization of the tab link at the navbar
static unsigned uVisible=1;
static char cYesNouVisible[32]={""};
//uOrder: Position of the element in the nav bar.
static unsigned uOrder=1;
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



#define VAR_LIST_tContentType "tContentType.uContentType,tContentType.cLabel,tContentType.uVisible,tContentType.uOrder,tContentType.uOwner,tContentType.uCreatedBy,tContentType.uCreatedDate,tContentType.uModBy,tContentType.uModDate"

 //Local only
void Insert_tContentType(void);
void Update_tContentType(char *cRowid);
void ProcesstContentTypeListVars(pentry entries[], int x);

 //In tContentTypefunc.h file included below
void ExtProcesstContentTypeVars(pentry entries[], int x);
void ExttContentTypeCommands(pentry entries[], int x);
void ExttContentTypeButtons(void);
void ExttContentTypeNavBar(void);
void ExttContentTypeGetHook(entry gentries[], int x);
void ExttContentTypeSelect(void);
void ExttContentTypeSelectRow(void);
void ExttContentTypeListSelect(void);
void ExttContentTypeListFilter(void);
void ExttContentTypeAuxTable(void);

#include "tcontenttypefunc.h"

 //Table Variables Assignment Function
void ProcesstContentTypeVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uContentType"))
			sscanf(entries[i].val,"%u",&uContentType);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uVisible"))
			sscanf(entries[i].val,"%u",&uVisible);
		else if(!strcmp(entries[i].name,"cYesNouVisible"))
		{
			sprintf(cYesNouVisible,"%.31s",entries[i].val);
			uVisible=ReadYesNoPullDown(cYesNouVisible);
		}
		else if(!strcmp(entries[i].name,"uOrder"))
			sscanf(entries[i].val,"%u",&uOrder);
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
	ExtProcesstContentTypeVars(entries,x);

}//ProcesstContentTypeVars()


void ProcesstContentTypeListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uContentType);
                        guMode=2002;
                        tContentType("");
                }
        }
}//void ProcesstContentTypeListVars(pentry entries[], int x)


int tContentTypeCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttContentTypeCommands(entries,x);

	if(!strcmp(gcFunction,"tContentTypeTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tContentTypeList();
		}

		//Default
		ProcesstContentTypeVars(entries,x);
		tContentType("");
	}
	else if(!strcmp(gcFunction,"tContentTypeList"))
	{
		ProcessControlVars(entries,x);
		ProcesstContentTypeListVars(entries,x);
		tContentTypeList();
	}

	return(0);

}//tContentTypeCommands()


void tContentType(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttContentTypeSelectRow();
		else
			ExttContentTypeSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetContentType();
				unxsBlog("New tContentType table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tContentType WHERE uContentType=%u"
						,uContentType);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uContentType);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uVisible);
		sscanf(field[3],"%u",&uOrder);
		sscanf(field[4],"%u",&uOwner);
		sscanf(field[5],"%u",&uCreatedBy);
		sscanf(field[6],"%lu",&uCreatedDate);
		sscanf(field[7],"%u",&uModBy);
		sscanf(field[8],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tContentType",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tContentTypeTools>");
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

        ExttContentTypeButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tContentType Record Data",100);

	if(guMode==2000 || guMode==2002)
		tContentTypeInput(1);
	else
		tContentTypeInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttContentTypeAuxTable();

	Footer_ism3();

}//end of tContentType();


void tContentTypeInput(unsigned uMode)
{

//uContentType
	OpenRow(LANG_FL_tContentType_uContentType,"black");
	printf("<input title='%s' type=text name=uContentType value=%u size=16 maxlength=10 "
,LANG_FT_tContentType_uContentType,uContentType);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uContentType value=%u >\n",uContentType);
	}
//cLabel
	OpenRow(LANG_FL_tContentType_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tContentType_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uVisible
	OpenRow(LANG_FL_tContentType_uVisible,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uVisible",uVisible,1);
	else
		YesNoPullDown("uVisible",uVisible,0);
//uOrder
	OpenRow(LANG_FL_tContentType_uOrder,"black");
	printf("<input title='%s' type=text name=uOrder value=%u size=16 maxlength=10 "
,LANG_FT_tContentType_uOrder,uOrder);
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uOrder value=%u >\n",uOrder);
	}
//uOwner
	OpenRow(LANG_FL_tContentType_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tContentType_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tContentType_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tContentType_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tContentType_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tContentTypeInput(unsigned uMode)


void NewtContentType(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uContentType FROM tContentType\
				WHERE uContentType=%u"
							,uContentType);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tContentType("<blink>Record already exists");
		tContentType(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tContentType();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uContentType=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tContentType",uContentType);
	unxsBlogLog(uContentType,"tContentType","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uContentType);
	tContentType(gcQuery);
	}

}//NewtContentType(unsigned uMode)


void DeletetContentType(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tContentType WHERE uContentType=%u AND ( uOwner=%u OR %u>9 )"
					,uContentType,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tContentType WHERE uContentType=%u"
					,uContentType);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tContentType("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsBlogLog(uContentType,"tContentType","Del");
#endif
		tContentType(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsBlogLog(uContentType,"tContentType","DelError");
#endif
		tContentType(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetContentType(void)


void Insert_tContentType(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tContentType SET uContentType=%u,cLabel='%s',uVisible=%u,uOrder=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uContentType
			,TextAreaSave(cLabel)
			,uVisible
			,uOrder
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tContentType(void)


void Update_tContentType(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tContentType SET uContentType=%u,cLabel='%s',uVisible=%u,uOrder=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uContentType
			,TextAreaSave(cLabel)
			,uVisible
			,uOrder
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tContentType(void)


void ModtContentType(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tContentType.uContentType,\
				tContentType.uModDate\
				FROM tContentType,tClient\
				WHERE tContentType.uContentType=%u\
				AND tContentType.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uContentType,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uContentType,uModDate FROM tContentType\
				WHERE uContentType=%u"
						,uContentType);
#else
	sprintf(gcQuery,"SELECT uContentType FROM tContentType\
				WHERE uContentType=%u"
						,uContentType);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tContentType("<blink>Record does not exist");
	if(i<1) tContentType(LANG_NBR_RECNOTEXIST);
	//if(i>1) tContentType("<blink>Multiple rows!");
	if(i>1) tContentType(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tContentType(LANG_NBR_EXTMOD);
#endif

	Update_tContentType(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tContentType",uContentType);
	unxsBlogLog(uContentType,"tContentType","Mod");
#endif
	tContentType(gcQuery);

}//ModtContentType(void)


void tContentTypeList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttContentTypeListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tContentTypeList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttContentTypeListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uContentType<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uVisible<td><font face=arial,helvetica color=white>uOrder<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luYesNo2=strtoul(field[2],NULL,10);
		char cBuf2[4];
		if(luYesNo2)
			sprintf(cBuf2,"Yes");
		else
			sprintf(cBuf2,"No");
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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,cBuf2
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

}//tContentTypeList()


void CreatetContentType(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tContentType ( uContentType INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, uVisible INT UNSIGNED NOT NULL DEFAULT 0, uOrder INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetContentType()

