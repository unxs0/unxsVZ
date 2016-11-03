/*
FILE
	tProductType source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tproducttypefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uProductType: Primary Key
static unsigned uProductType=0;
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



#define VAR_LIST_tProductType "tProductType.uProductType,tProductType.cLabel,tProductType.uOwner,tProductType.uCreatedBy,tProductType.uCreatedDate,tProductType.uModBy,tProductType.uModDate"

 //Local only
void Insert_tProductType(void);
void Update_tProductType(char *cRowid);
void ProcesstProductTypeListVars(pentry entries[], int x);

 //In tProductTypefunc.h file included below
void ExtProcesstProductTypeVars(pentry entries[], int x);
void ExttProductTypeCommands(pentry entries[], int x);
void ExttProductTypeButtons(void);
void ExttProductTypeNavBar(void);
void ExttProductTypeGetHook(entry gentries[], int x);
void ExttProductTypeSelect(void);
void ExttProductTypeSelectRow(void);
void ExttProductTypeListSelect(void);
void ExttProductTypeListFilter(void);
void ExttProductTypeAuxTable(void);

#include "tproducttypefunc.h"

 //Table Variables Assignment Function
void ProcesstProductTypeVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uProductType"))
			sscanf(entries[i].val,"%u",&uProductType);
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
	ExtProcesstProductTypeVars(entries,x);

}//ProcesstProductTypeVars()


void ProcesstProductTypeListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uProductType);
                        guMode=2002;
                        tProductType("");
                }
        }
}//void ProcesstProductTypeListVars(pentry entries[], int x)


int tProductTypeCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttProductTypeCommands(entries,x);

	if(!strcmp(gcFunction,"tProductTypeTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tProductTypeList();
		}

		//Default
		ProcesstProductTypeVars(entries,x);
		tProductType("");
	}
	else if(!strcmp(gcFunction,"tProductTypeList"))
	{
		ProcessControlVars(entries,x);
		ProcesstProductTypeListVars(entries,x);
		tProductTypeList();
	}

	return(0);

}//tProductTypeCommands()


void tProductType(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttProductTypeSelectRow();
		else
			ExttProductTypeSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetProductType();
				unxsISP("New tProductType table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tProductType WHERE uProductType=%u"
						,uProductType);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uProductType);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tProductType",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tProductTypeTools>");
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

        ExttProductTypeButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tProductType Record Data",100);

	if(guMode==2000 || guMode==2002)
		tProductTypeInput(1);
	else
		tProductTypeInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttProductTypeAuxTable();

	Footer_ism3();

}//end of tProductType();


void tProductTypeInput(unsigned uMode)
{

//uProductType
	OpenRow(LANG_FL_tProductType_uProductType,"black");
	printf("<input title='%s' type=text name=uProductType value=%u size=16 maxlength=10 "
,LANG_FT_tProductType_uProductType,uProductType);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uProductType value=%u >\n",uProductType);
	}
//cLabel
	OpenRow(LANG_FL_tProductType_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tProductType_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tProductType_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tProductType_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tProductType_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tProductType_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tProductType_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tProductTypeInput(unsigned uMode)


void NewtProductType(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uProductType FROM tProductType\
				WHERE uProductType=%u"
							,uProductType);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tProductType("<blink>Record already exists");
		tProductType(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tProductType();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uProductType=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tProductType",uProductType);
	unxsISPLog(uProductType,"tProductType","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uProductType);
	tProductType(gcQuery);
	}

}//NewtProductType(unsigned uMode)


void DeletetProductType(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tProductType WHERE uProductType=%u AND ( uOwner=%u OR %u>9 )"
					,uProductType,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tProductType WHERE uProductType=%u"
					,uProductType);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tProductType("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uProductType,"tProductType","Del");
#endif
		tProductType(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uProductType,"tProductType","DelError");
#endif
		tProductType(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetProductType(void)


void Insert_tProductType(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tProductType SET uProductType=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uProductType
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tProductType(void)


void Update_tProductType(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tProductType SET uProductType=%u,cLabel='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uProductType
			,TextAreaSave(cLabel)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tProductType(void)


void ModtProductType(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uProductType,uModDate FROM tProductType WHERE uProductType=%u"
						,uProductType);
#else
	sprintf(gcQuery,"SELECT uProductType FROM tProductType WHERE uProductType=%u"
						,uProductType);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tProductType("<blink>Record does not exist");
	if(i<1) tProductType(LANG_NBR_RECNOTEXIST);
	//if(i>1) tProductType("<blink>Multiple rows!");
	if(i>1) tProductType(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tProductType(LANG_NBR_EXTMOD);
#endif

	Update_tProductType(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tProductType",uProductType);
	unxsISPLog(uProductType,"tProductType","Mod");
#endif
	tProductType(gcQuery);

}//ModtProductType(void)


void tProductTypeList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttProductTypeListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tProductTypeList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttProductTypeListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uProductType<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[2],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[3],NULL,10))
			,cBuf4
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,cBuf6
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tProductTypeList()


void CreatetProductType(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tProductType ( uProductType INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetProductType()

