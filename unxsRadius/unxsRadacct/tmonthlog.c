/*
FILE
	tMonthLog source code of unxsRadacct.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tmonthlogfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uMonthLog: Primary Key
static unsigned uMonthLog=0;
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



#define VAR_LIST_tMonthLog "tMonthLog.uMonthLog,tMonthLog.cLabel,tMonthLog.uOwner,tMonthLog.uCreatedBy,tMonthLog.uCreatedDate,tMonthLog.uModBy,tMonthLog.uModDate"

 //Local only
void Insert_tMonthLog(void);
void Update_tMonthLog(char *cRowid);
void ProcesstMonthLogListVars(pentry entries[], int x);

 //In tMonthLogfunc.h file included below
void ExtProcesstMonthLogVars(pentry entries[], int x);
void ExttMonthLogCommands(pentry entries[], int x);
void ExttMonthLogButtons(void);
void ExttMonthLogNavBar(void);
void ExttMonthLogGetHook(entry gentries[], int x);
void ExttMonthLogSelect(void);
void ExttMonthLogSelectRow(void);
void ExttMonthLogListSelect(void);
void ExttMonthLogListFilter(void);
void ExttMonthLogAuxTable(void);

#include "tmonthlogfunc.h"

 //Table Variables Assignment Function
void ProcesstMonthLogVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uMonthLog"))
			sscanf(entries[i].val,"%u",&uMonthLog);
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
	ExtProcesstMonthLogVars(entries,x);

}//ProcesstMonthLogVars()


void ProcesstMonthLogListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uMonthLog);
                        guMode=2002;
                        tMonthLog("");
                }
        }
}//void ProcesstMonthLogListVars(pentry entries[], int x)


int tMonthLogCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttMonthLogCommands(entries,x);

	if(!strcmp(gcFunction,"tMonthLogTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tMonthLogList();
		}

		//Default
		ProcesstMonthLogVars(entries,x);
		tMonthLog("");
	}
	else if(!strcmp(gcFunction,"tMonthLogList"))
	{
		ProcessControlVars(entries,x);
		ProcesstMonthLogListVars(entries,x);
		tMonthLogList();
	}

	return(0);

}//tMonthLogCommands()


void tMonthLog(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttMonthLogSelectRow();
		else
			ExttMonthLogSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetMonthLog();
				unxsRadacct("New tMonthLog table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tMonthLog WHERE uMonthLog=%u"
						,uMonthLog);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uMonthLog);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tMonthLog",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tMonthLogTools>");
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

        ExttMonthLogButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tMonthLog Record Data",100);

	if(guMode==2000 || guMode==2002)
		tMonthLogInput(1);
	else
		tMonthLogInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttMonthLogAuxTable();

	Footer_ism3();

}//end of tMonthLog();


void tMonthLogInput(unsigned uMode)
{

//uMonthLog
	OpenRow(LANG_FL_tMonthLog_uMonthLog,"black");
	printf("<input title='%s' type=text name=uMonthLog value=%u size=16 maxlength=10 "
,LANG_FT_tMonthLog_uMonthLog,uMonthLog);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uMonthLog value=%u >\n",uMonthLog);
	}
//cLabel
	OpenRow(LANG_FL_tMonthLog_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tMonthLog_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tMonthLog_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tMonthLog_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tMonthLog_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tMonthLog_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tMonthLog_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tMonthLogInput(unsigned uMode)


void NewtMonthLog(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uMonthLog FROM tMonthLog\
				WHERE uMonthLog=%u"
							,uMonthLog);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tMonthLog("<blink>Record already exists");
		tMonthLog(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tMonthLog();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uMonthLog=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tMonthLog",uMonthLog);
	unxsRadacctLog(uMonthLog,"tMonthLog","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uMonthLog);
	tMonthLog(gcQuery);
	}

}//NewtMonthLog(unsigned uMode)


void DeletetMonthLog(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tMonthLog WHERE uMonthLog=%u AND ( uOwner=%u OR %u>9 )"
					,uMonthLog,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tMonthLog WHERE uMonthLog=%u"
					,uMonthLog);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tMonthLog("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsRadacctLog(uMonthLog,"tMonthLog","Del");
#endif
		tMonthLog(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsRadacctLog(uMonthLog,"tMonthLog","DelError");
#endif
		tMonthLog(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetMonthLog(void)


void Insert_tMonthLog(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tMonthLog SET uMonthLog=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uMonthLog
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tMonthLog(void)


void Update_tMonthLog(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tMonthLog SET uMonthLog=%u,cLabel='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uMonthLog
			,TextAreaSave(cLabel)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tMonthLog(void)


void ModtMonthLog(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tMonthLog.uMonthLog,\
				tMonthLog.uModDate\
				FROM tMonthLog,tClient\
				WHERE tMonthLog.uMonthLog=%u\
				AND tMonthLog.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uMonthLog,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uMonthLog,uModDate FROM tMonthLog\
				WHERE uMonthLog=%u"
						,uMonthLog);
#else
	sprintf(gcQuery,"SELECT uMonthLog FROM tMonthLog\
				WHERE uMonthLog=%u"
						,uMonthLog);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tMonthLog("<blink>Record does not exist");
	if(i<1) tMonthLog(LANG_NBR_RECNOTEXIST);
	//if(i>1) tMonthLog("<blink>Multiple rows!");
	if(i>1) tMonthLog(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tMonthLog(LANG_NBR_EXTMOD);
#endif

	Update_tMonthLog(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tMonthLog",uMonthLog);
	unxsRadacctLog(uMonthLog,"tMonthLog","Mod");
#endif
	tMonthLog(gcQuery);

}//ModtMonthLog(void)


void tMonthLogList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttMonthLogListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tMonthLogList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttMonthLogListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uMonthLog<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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

}//tMonthLogList()


void CreatetMonthLog(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tMonthLog ( uMonthLog INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetMonthLog()

