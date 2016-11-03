/*
FILE
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in ttimeintervalfunc.h
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
static unsigned uTimeInterval=0;
static char cLabel[33]={""};
static char cStartTime[17]={"00:00:00"};
static char cEndTime[17]={"00:00:00"};
static char cStartDate[17]={"20120101"};
static char cEndDate[17]={""};
static char cDaysOfWeek[17]={"1,2,3,4,5,6,7"};
static unsigned uOwner=0;
#define StandardFields
static unsigned uCreatedBy=0;
static time_t uCreatedDate=0;
static unsigned uModBy=0;
static time_t uModDate=0;


#define VAR_LIST_tTimeInterval "tTimeInterval.uTimeInterval,tTimeInterval.cLabel,tTimeInterval.cStartTime,tTimeInterval.cEndTime,tTimeInterval.cStartDate,tTimeInterval.cEndDate,tTimeInterval.cDaysOfWeek,tTimeInterval.uOwner,tTimeInterval.uCreatedBy,tTimeInterval.uCreatedDate,tTimeInterval.uModBy,tTimeInterval.uModDate"

 //Local only
void Insert_tTimeInterval(void);
void Update_tTimeInterval(char *cRowid);
void ProcesstTimeIntervalListVars(pentry entries[], int x);

 //In tTimeIntervalfunc.h file included below
void ExtProcesstTimeIntervalVars(pentry entries[], int x);
void ExttTimeIntervalCommands(pentry entries[], int x);
void ExttTimeIntervalButtons(void);
void ExttTimeIntervalNavBar(void);
void ExttTimeIntervalGetHook(entry gentries[], int x);
void ExttTimeIntervalSelect(void);
void ExttTimeIntervalSelectRow(void);
void ExttTimeIntervalListSelect(void);
void ExttTimeIntervalListFilter(void);
void ExttTimeIntervalAuxTable(void);

#include "ttimeintervalfunc.h"

 //Table Variables Assignment Function
void ProcesstTimeIntervalVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		
		if(!strcmp(entries[i].name,"uTimeInterval"))
			sscanf(entries[i].val,"%u",&uTimeInterval);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"cStartTime"))
			sprintf(cStartTime,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"cEndTime"))
			sprintf(cEndTime,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"cStartDate"))
			sprintf(cStartDate,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"cEndDate"))
			sprintf(cEndDate,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"cDaysOfWeek"))
			sprintf(cDaysOfWeek,"%.40s",entries[i].val);
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
	ExtProcesstTimeIntervalVars(entries,x);

}//ProcesstTimeIntervalVars()


void ProcesstTimeIntervalListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uTimeInterval);
                        guMode=2002;
                        tTimeInterval("");
                }
        }
}//void ProcesstTimeIntervalListVars(pentry entries[], int x)


int tTimeIntervalCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttTimeIntervalCommands(entries,x);

	if(!strcmp(gcFunction,"tTimeIntervalTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tTimeIntervalList();
		}

		//Default
		ProcesstTimeIntervalVars(entries,x);
		tTimeInterval("");
	}
	else if(!strcmp(gcFunction,"tTimeIntervalList"))
	{
		ProcessControlVars(entries,x);
		ProcesstTimeIntervalListVars(entries,x);
		tTimeIntervalList();
	}

	return(0);

}//tTimeIntervalCommands()


void tTimeInterval(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttTimeIntervalSelectRow();
		else
			ExttTimeIntervalSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetTimeInterval();
				unxsSPS("New tTimeInterval table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tTimeInterval WHERE uTimeInterval=%u"
						,uTimeInterval);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			
		sscanf(field[0],"%u",&uTimeInterval);
		sprintf(cLabel,"%.32s",field[1]);
		sprintf(cStartTime,"%.16s",field[2]);
		sprintf(cEndTime,"%.16s",field[3]);
		sprintf(cStartDate,"%.16s",field[4]);
		sprintf(cEndDate,"%.16s",field[5]);
		sprintf(cDaysOfWeek,"%.16s",field[6]);
		sscanf(field[7],"%u",&uOwner);
		sscanf(field[8],"%u",&uCreatedBy);
		sscanf(field[9],"%lu",&uCreatedDate);
		sscanf(field[10],"%u",&uModBy);
		sscanf(field[11],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: Specify time intervals for rule",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tTimeIntervalTools>");
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

        ExttTimeIntervalButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tTimeInterval Record Data",100);

	if(guMode==2000 || guMode==2002)
		tTimeIntervalInput(1);
	else
		tTimeIntervalInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttTimeIntervalAuxTable();

	Footer_ism3();

}//end of tTimeInterval();


void tTimeIntervalInput(unsigned uMode)
{

	
	//uTimeInterval uRADType=1001
	OpenRow(LANG_FL_tTimeInterval_uTimeInterval,"black");
	printf("<input title='%s' type=text name=uTimeInterval id=uTimeInterval value='%u' size=16 maxlength=10 "
		,LANG_FT_tTimeInterval_uTimeInterval,uTimeInterval);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uTimeInterval id=uTimeInterval value='%u' >\n",uTimeInterval);
	}
	//cLabel uRADType=253
	OpenRow(LANG_FL_tTimeInterval_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel id=cLabel value='%s' size=40 maxlength=32 "
		,LANG_FT_tTimeInterval_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel id=cLabel value='%s'>\n",EncodeDoubleQuotes(cLabel));
	}
	//cStartTime uRADType=253
	OpenRow(LANG_FL_tTimeInterval_cStartTime,"black");
	printf("<input title='%s' type=text name=cStartTime id=cStartTime value='%s' size=40 maxlength=16 "
		,LANG_FT_tTimeInterval_cStartTime,EncodeDoubleQuotes(cStartTime));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cStartTime id=cStartTime value='%s'>\n",EncodeDoubleQuotes(cStartTime));
	}
	//cEndTime uRADType=253
	OpenRow(LANG_FL_tTimeInterval_cEndTime,"black");
	printf("<input title='%s' type=text name=cEndTime id=cEndTime value='%s' size=40 maxlength=16 "
		,LANG_FT_tTimeInterval_cEndTime,EncodeDoubleQuotes(cEndTime));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cEndTime id=cEndTime value='%s'>\n",EncodeDoubleQuotes(cEndTime));
	}
	//cStartDate uRADType=253
	OpenRow(LANG_FL_tTimeInterval_cStartDate,"black");
	printf("<input title='%s' type=text name=cStartDate id=cStartDate value='%s' size=40 maxlength=16 "
		,LANG_FT_tTimeInterval_cStartDate,EncodeDoubleQuotes(cStartDate));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cStartDate id=cStartDate value='%s'>\n",EncodeDoubleQuotes(cStartDate));
	}
	//cEndDate uRADType=253
	OpenRow(LANG_FL_tTimeInterval_cEndDate,"black");
	printf("<input title='%s' type=text name=cEndDate id=cEndDate value='%s' size=40 maxlength=16 "
		,LANG_FT_tTimeInterval_cEndDate,EncodeDoubleQuotes(cEndDate));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cEndDate id=cEndDate value='%s'>\n",EncodeDoubleQuotes(cEndDate));
	}
	//cDaysOfWeek uRADType=253
	OpenRow(LANG_FL_tTimeInterval_cDaysOfWeek,"black");
	printf("<input title='%s' type=text name=cDaysOfWeek id=cDaysOfWeek value='%s' size=40 maxlength=16 "
		,LANG_FT_tTimeInterval_cDaysOfWeek,EncodeDoubleQuotes(cDaysOfWeek));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cDaysOfWeek id=cDaysOfWeek value='%s'>\n",EncodeDoubleQuotes(cDaysOfWeek));
	}
	//uOwner COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tTimeInterval_uOwner,"black");
	printf("%s<input type=hidden name=uOwner id=uOwner value='%u' >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tTimeInterval_uCreatedBy,"black");
	printf("%s<input type=hidden name=uCreatedBy id=uCreatedBy value='%u' >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tTimeInterval_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate id=uCreatedDate value='%lu' >\n",uCreatedDate);
	//uModBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tTimeInterval_uModBy,"black");
	printf("%s<input type=hidden name=uModBy id=uModBy value='%u' >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tTimeInterval_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate id=uModDate value='%lu' >\n",uModDate);
	printf("</tr>\n");

}//void tTimeIntervalInput(unsigned uMode)


void NewtTimeInterval(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uTimeInterval FROM tTimeInterval WHERE uTimeInterval=%u",uTimeInterval);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		tTimeInterval(LANG_NBR_RECEXISTS);

	Insert_tTimeInterval();
	uTimeInterval=mysql_insert_id(&gMysql);
#ifdef StandardFields
	uCreatedDate=luGetCreatedDate("tTimeInterval",uTimeInterval);
#endif
	unxsSPSLog(uTimeInterval,"tTimeInterval","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uTimeInterval);
		tTimeInterval(gcQuery);
	}

}//NewtTimeInterval(unsigned uMode)


void DeletetTimeInterval(void)
{
#ifdef StandardFields
	sprintf(gcQuery,"DELETE FROM tTimeInterval WHERE uTimeInterval=%u AND ( uOwner=%u OR %u>9 )"
					,uTimeInterval,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tTimeInterval WHERE uTimeInterval=%u AND %u>9 )"
					,uTimeInterval,guPermLevel);
#endif
	macro_mySQLQueryHTMLError;
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsSPSLog(uTimeInterval,"tTimeInterval","Del");
		tTimeInterval(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsSPSLog(uTimeInterval,"tTimeInterval","DelError");
		tTimeInterval(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetTimeInterval(void)


void Insert_tTimeInterval(void)
{
	sprintf(gcQuery,"INSERT INTO tTimeInterval SET "
		"cLabel='%s',"
		"cStartTime='%s',"
		"cEndTime='%s',"
		"cStartDate='%s',"
		"cEndDate='%s',"
		"cDaysOfWeek='%s',"
		"uOwner=%u,"
		"uCreatedBy=%u,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,TextAreaSave(cLabel)
			,TextAreaSave(cStartTime)
			,TextAreaSave(cEndTime)
			,TextAreaSave(cStartDate)
			,TextAreaSave(cEndDate)
			,TextAreaSave(cDaysOfWeek)
			,uOwner
			,uCreatedBy
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tTimeInterval(void)


void Update_tTimeInterval(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tTimeInterval SET "
		"cLabel='%s',"
		"cStartTime='%s',"
		"cEndTime='%s',"
		"cStartDate='%s',"
		"cEndDate='%s',"
		"cDaysOfWeek='%s',"
		"uOwner=%u,"
		"uModBy=%u,"
		"uModDate=UNIX_TIMESTAMP(NOW())"
		" WHERE _rowid=%s"
			,TextAreaSave(cLabel)
			,TextAreaSave(cStartTime)
			,TextAreaSave(cEndTime)
			,TextAreaSave(cStartDate)
			,TextAreaSave(cEndDate)
			,TextAreaSave(cDaysOfWeek)
			,uOwner
			,uModBy
			,cRowid
		);

	macro_mySQLQueryHTMLError;

}//void Update_tTimeInterval(void)


void ModtTimeInterval(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

#ifdef StandardFields
	unsigned uPreModDate=0;
	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tTimeInterval.uTimeInterval,"
				" tTimeInterval.uModDate"
				" FROM tTimeInterval,tClient"
				" WHERE tTimeInterval.uTimeInterval=%u"
				" AND tTimeInterval.uOwner=tClient.uClient"
				" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uTimeInterval,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uTimeInterval,uModDate FROM tTimeInterval"
				" WHERE uTimeInterval=%u"
					,uTimeInterval);
#else
	sprintf(gcQuery,"SELECT uTimeInterval FROM tTimeInterval"
				" WHERE uTimeInterval=%u"
					,uTimeInterval);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) tTimeInterval(LANG_NBR_RECNOTEXIST);
	if(i>1) tTimeInterval(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef StandardFields
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tTimeInterval(LANG_NBR_EXTMOD);
#endif

	Update_tTimeInterval(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef StandardFields
	uModDate=luGetModDate("tTimeInterval",uTimeInterval);
#endif
	unxsSPSLog(uTimeInterval,"tTimeInterval","Mod");
	tTimeInterval(gcQuery);

}//ModtTimeInterval(void)


void tTimeIntervalList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttTimeIntervalListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tTimeIntervalList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttTimeIntervalListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uTimeInterval"
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>cStartTime"
		"<td><font face=arial,helvetica color=white>cEndTime"
		"<td><font face=arial,helvetica color=white>cStartDate"
		"<td><font face=arial,helvetica color=white>cEndDate"
		"<td><font face=arial,helvetica color=white>cDaysOfWeek"
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
				time_t luTime9=strtoul(field[9],NULL,10);
		char cBuf9[32];
		if(luTime9)
			ctime_r(&luTime9,cBuf9);
		else
			sprintf(cBuf9,"---");
		time_t luTime11=strtoul(field[11],NULL,10);
		char cBuf11[32];
		if(luTime11)
			ctime_r(&luTime11,cBuf11);
		else
			sprintf(cBuf11,"---");
		printf("<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tTimeInterval&uTimeInterval=%s>%s</a><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,field[4]
			,field[5]
			,field[6]
			,ForeignKey("tClient","cLabel",strtoul(field[7],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[8],NULL,10))
			,cBuf9
			,ForeignKey("tClient","cLabel",strtoul(field[10],NULL,10))
			,cBuf11
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tTimeIntervalList()


void CreatetTimeInterval(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tTimeInterval ("
		"uTimeInterval INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
		"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
		"cStartTime VARCHAR(16) NOT NULL DEFAULT '',"
		"cEndTime VARCHAR(16) NOT NULL DEFAULT '',"
		"cStartDate VARCHAR(16) NOT NULL DEFAULT '',"
		"cEndDate VARCHAR(16) NOT NULL DEFAULT '',"
		"cDaysOfWeek VARCHAR(16) NOT NULL DEFAULT '',"
		"uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetTimeInterval(void)


