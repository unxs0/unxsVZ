/*
FILE
	tUserType source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tusertypefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uUserType: Primary Key
static unsigned uUserType=0;
//cLabel: Short label
static char cLabel[33]={""};
//uOwner: Record owner
static unsigned uOwner=0;
//uCreatedBy: uClient for last insert
static unsigned uCreatedBy=0;

//uCreatedDate: Unix seconds date last insert
static time_t uCreatedDate=0;
//uModBy: uClient for last update
static unsigned uModBy=0;
//uModDate: Unix seconds date last update
static time_t uModDate=0;



#define VAR_LIST_tUserType "tUserType.uUserType,tUserType.cLabel,tUserType.uOwner,tUserType.uCreatedBy,tUserType.uCreatedDate,tUserType.uModBy,tUserType.uModDate"

 //Local only
void Insert_tUserType(void);
void Update_tUserType(char *cRowid);
void ProcesstUserTypeListVars(pentry entries[], int x);

 //In tUserTypefunc.h file included below
void ExtProcesstUserTypeVars(pentry entries[], int x);
void ExttUserTypeCommands(pentry entries[], int x);
void ExttUserTypeButtons(void);
void ExttUserTypeNavBar(void);
void ExttUserTypeGetHook(entry gentries[], int x);
void ExttUserTypeSelect(void);
void ExttUserTypeSelectRow(void);
void ExttUserTypeListSelect(void);
void ExttUserTypeListFilter(void);
void ExttUserTypeAuxTable(void);

#include "tusertypefunc.h"

 //Table Variables Assignment Function
void ProcesstUserTypeVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uUserType"))
			sscanf(entries[i].val,"%u",&uUserType);
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
	ExtProcesstUserTypeVars(entries,x);

}//ProcesstUserTypeVars()


void ProcesstUserTypeListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uUserType);
                        guMode=2002;
                        tUserType("");
                }
        }
}//void ProcesstUserTypeListVars(pentry entries[], int x)


int tUserTypeCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttUserTypeCommands(entries,x);

	if(!strcmp(gcFunction,"tUserTypeTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tUserTypeList();
		}

		//Default
		ProcesstUserTypeVars(entries,x);
		tUserType("");
	}
	else if(!strcmp(gcFunction,"tUserTypeList"))
	{
		ProcessControlVars(entries,x);
		ProcesstUserTypeListVars(entries,x);
		tUserTypeList();
	}

	return(0);

}//tUserTypeCommands()


void tUserType(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttUserTypeSelectRow();
		else
			ExttUserTypeSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetUserType();
				unxsMail("New tUserType table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tUserType WHERE uUserType=%u"
						,uUserType);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uUserType);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tUserType",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tUserTypeTools>");
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

        ExttUserTypeButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tUserType Record Data",100);

	if(guMode==2000 || guMode==2002)
		tUserTypeInput(1);
	else
		tUserTypeInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttUserTypeAuxTable();

	Footer_ism3();

}//end of tUserType();


void tUserTypeInput(unsigned uMode)
{

//uUserType
	OpenRow(LANG_FL_tUserType_uUserType,"black");
	printf("<input title='%s' type=text name=uUserType value=%u size=16 maxlength=10 "
,LANG_FT_tUserType_uUserType,uUserType);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uUserType value=%u >\n",uUserType);
	}
//cLabel
	OpenRow(LANG_FL_tUserType_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tUserType_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tUserType_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tUserType_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tUserType_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tUserType_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tUserType_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tUserTypeInput(unsigned uMode)


void NewtUserType(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uUserType FROM tUserType\
				WHERE uUserType=%u"
							,uUserType);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tUserType("<blink>Record already exists");
		tUserType(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tUserType();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uUserType=mysql_insert_id(&gMysql);

	uCreatedDate=luGetCreatedDate("tUserType",uUserType);
	unxsMailLog(uUserType,"tUserType","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uUserType);
	tUserType(gcQuery);
	}

}//NewtUserType(unsigned uMode)


void DeletetUserType(void)
{

	sprintf(gcQuery,"DELETE FROM tUserType WHERE uUserType=%u AND ( uOwner=%u OR %u>9 )"
					,uUserType,guLoginClient,guPermLevel);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tUserType("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uUserType,"tUserType","Del");

		tUserType(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uUserType,"tUserType","DelError");

		tUserType(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetUserType(void)


void Insert_tUserType(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tUserType SET uUserType=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uUserType
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tUserType(void)


void Update_tUserType(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tUserType SET uUserType=%u,cLabel='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uUserType
			,TextAreaSave(cLabel)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tUserType(void)


void ModtUserType(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uUserType,uModDate FROM tUserType WHERE uUserType=%u"
						,uUserType);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tUserType("<blink>Record does not exist");
	if(i<1) tUserType(LANG_NBR_RECNOTEXIST);
	//if(i>1) tUserType("<blink>Multiple rows!");
	if(i>1) tUserType(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tUserType(LANG_NBR_EXTMOD);


	Update_tUserType(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	uModDate=luGetModDate("tUserType",uUserType);
	unxsMailLog(uUserType,"tUserType","Mod");

	tUserType(gcQuery);

}//ModtUserType(void)


void tUserTypeList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttUserTypeListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tUserTypeList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttUserTypeListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uUserType<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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

}//tUserTypeList()


void CreatetUserType(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tUserType ( uUserType INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetUserType()

