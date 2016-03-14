/*
FILE
	tContainerType source code of unxsVZ.cgi
	$Id: tcontainertype.c 2690 2014-08-11 21:12:00Z Colin $
PURPOSE
	Simple lookup table.
AUTHOR
	(C) 2016-2016 Gary Wallis for Unixservice, LLC.
	GPLv2 applies.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uType: Primary Key
static unsigned uType=0;
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



#define VAR_LIST_tContainerType "tContainerType.uType,tContainerType.cLabel,tContainerType.uOwner,tContainerType.uCreatedBy,tContainerType.uCreatedDate,tContainerType.uModBy,tContainerType.uModDate"

 //Local only
void Insert_tContainerType(void);
void Update_tContainerType(char *cRowid);
void ProcesstContainerTypeListVars(pentry entries[], int x);

 //In tContainerTypefunc.h file included below
void ExtProcesstContainerTypeVars(pentry entries[], int x);
void ExttContainerTypeCommands(pentry entries[], int x);
void ExttContainerTypeButtons(void);
void ExttContainerTypeNavBar(void);
void ExttContainerTypeGetHook(entry gentries[], int x);
void ExttContainerTypeSelect(void);
void ExttContainerTypeSelectRow(void);
void ExttContainerTypeListSelect(void);
void ExttContainerTypeListFilter(void);
void ExttContainerTypeAuxTable(void);

#include "tcontainertypefunc.h"

 //Table Variables Assignment Function
void ProcesstContainerTypeVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uType"))
			sscanf(entries[i].val,"%u",&uType);
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
	ExtProcesstContainerTypeVars(entries,x);

}//ProcesstContainerTypeVars()


void ProcesstContainerTypeListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uType);
                        guMode=2002;
                        tContainerType("");
                }
        }
}//void ProcesstContainerTypeListVars(pentry entries[], int x)


int tContainerTypeCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttContainerTypeCommands(entries,x);

	if(!strcmp(gcFunction,"tContainerTypeTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tContainerTypeList();
		}

		//Default
		ProcesstContainerTypeVars(entries,x);
		tContainerType("");
	}
	else if(!strcmp(gcFunction,"tContainerTypeList"))
	{
		ProcessControlVars(entries,x);
		ProcesstContainerTypeListVars(entries,x);
		tContainerTypeList();
	}

	return(0);

}//tContainerTypeCommands()


void tContainerType(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttContainerTypeSelectRow();
		else
			ExttContainerTypeSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetContainerType();
				unxsVZ("New tContainerType table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tContainerType WHERE uType=%u"
						,uType);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uType);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: Property Type",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tContainerTypeTools>");
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

        ExttContainerTypeButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tContainerType Record Data",100);

	if(guMode==2000 || guMode==2002)
		tContainerTypeInput(1);
	else
		tContainerTypeInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttContainerTypeAuxTable();

	Footer_ism3();

}//end of tContainerType();


void tContainerTypeInput(unsigned uMode)
{

//uType
	OpenRow(LANG_FL_tContainerType_uType,"black");
	printf("<input title='%s' type=text name=uType value=%u size=16 maxlength=10 "
,LANG_FT_tContainerType_uType,uType);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uType value=%u >\n",uType);
	}
//cLabel
	OpenRow(LANG_FL_tContainerType_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tContainerType_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uOwner
	OpenRow(LANG_FL_tContainerType_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tContainerType_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tContainerType_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tContainerType_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tContainerType_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tContainerTypeInput(unsigned uMode)


void NewtContainerType(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uType FROM tContainerType\
				WHERE uType=%u"
							,uType);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tContainerType("<blink>Record already exists");
		tContainerType(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tContainerType();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uType=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tContainerType",uType);
	unxsVZLog(uType,"tContainerType","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uType);
	tContainerType(gcQuery);
	}

}//NewtContainerType(unsigned uMode)


void DeletetContainerType(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tContainerType WHERE uType=%u AND ( uOwner=%u OR %u>9 )"
					,uType,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tContainerType WHERE uType=%u"
					,uType);
#endif
	MYSQL_RUN;
	//tContainerType("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsVZLog(uType,"tContainerType","Del");
#endif
		tContainerType(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsVZLog(uType,"tContainerType","DelError");
#endif
		tContainerType(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetContainerType(void)


void Insert_tContainerType(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tContainerType SET uType=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uType
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
			);

	MYSQL_RUN;

}//void Insert_tContainerType(void)


void Update_tContainerType(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tContainerType SET uType=%u,cLabel='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uType
			,TextAreaSave(cLabel)
			,uModBy
			,cRowid);

	MYSQL_RUN;

}//void Update_tContainerType(void)


void ModtContainerType(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tContainerType.uType,\
				tContainerType.uModDate\
				FROM tContainerType,tClient\
				WHERE tContainerType.uType=%u\
				AND tContainerType.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uType,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uType,uModDate FROM tContainerType\
				WHERE uType=%u"
						,uType);
#else
	sprintf(gcQuery,"SELECT uType FROM tContainerType\
				WHERE uType=%u"
						,uType);
#endif

	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tContainerType("<blink>Record does not exist");
	if(i<1) tContainerType(LANG_NBR_RECNOTEXIST);
	//if(i>1) tContainerType("<blink>Multiple rows!");
	if(i>1) tContainerType(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tContainerType(LANG_NBR_EXTMOD);
#endif

	Update_tContainerType(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tContainerType",uType);
	unxsVZLog(uType,"tContainerType","Mod");
#endif
	tContainerType(gcQuery);

}//ModtContainerType(void)


void tContainerTypeList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttContainerTypeListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tContainerTypeList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttContainerTypeListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uType<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		printf("<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tContainerType&uType=%s>%s</a>"
			"<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
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

}//tContainerTypeList()


void CreatetContainerType(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tContainerType ("
			" uType INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			" cLabel VARCHAR(32) NOT NULL DEFAULT '',"
			" uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner),"
			" uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetContainerType()

