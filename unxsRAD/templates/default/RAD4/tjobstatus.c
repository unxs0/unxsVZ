/*
FILE
	tJobStatus source code of {{cProject}}.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id: tjobstatus.c 1953 2012-05-22 15:03:17Z Colin $
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tjobstatusfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uJobStatus: Primary Key
static unsigned uJobStatus=0;
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



#define VAR_LIST_tJobStatus "tJobStatus.uJobStatus,tJobStatus.cLabel,tJobStatus.uOwner,tJobStatus.uCreatedBy,tJobStatus.uCreatedDate,tJobStatus.uModBy,tJobStatus.uModDate"

 //Local only
void Insert_tJobStatus(void);
void Update_tJobStatus(char *cRowid);
void ProcesstJobStatusListVars(pentry entries[], int x);

 //In tJobStatusfunc.h file included below
void ExtProcesstJobStatusVars(pentry entries[], int x);
void ExttJobStatusCommands(pentry entries[], int x);
void ExttJobStatusButtons(void);
void ExttJobStatusNavBar(void);
void ExttJobStatusGetHook(entry gentries[], int x);
void ExttJobStatusSelect(void);
void ExttJobStatusSelectRow(void);
void ExttJobStatusListSelect(void);
void ExttJobStatusListFilter(void);
void ExttJobStatusAuxTable(void);

#include "tjobstatusfunc.h"

 //Table Variables Assignment Function
void ProcesstJobStatusVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uJobStatus"))
			sscanf(entries[i].val,"%u",&uJobStatus);
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
	ExtProcesstJobStatusVars(entries,x);

}//ProcesstJobStatusVars()


void ProcesstJobStatusListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uJobStatus);
                        guMode=2002;
                        tJobStatus("");
                }
        }
}//void ProcesstJobStatusListVars(pentry entries[], int x)


int tJobStatusCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttJobStatusCommands(entries,x);

	if(!strcmp(gcFunction,"tJobStatusTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tJobStatusList();
		}

		//Default
		ProcesstJobStatusVars(entries,x);
		tJobStatus("");
	}
	else if(!strcmp(gcFunction,"tJobStatusList"))
	{
		ProcessControlVars(entries,x);
		ProcesstJobStatusListVars(entries,x);
		tJobStatusList();
	}

	return(0);

}//tJobStatusCommands()


void tJobStatus(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttJobStatusSelectRow();
		else
			ExttJobStatusSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetJobStatus();
				{{cProject}}("New tJobStatus table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tJobStatus WHERE uJobStatus=%u"
						,uJobStatus);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uJobStatus);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	HeaderRAD4(":: tJobStatus",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tJobStatusTools>");
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

        ExttJobStatusButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tJobStatus Record Data",100);

	if(guMode==2000 || guMode==2002)
		tJobStatusInput(1);
	else
		tJobStatusInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttJobStatusAuxTable();

	FooterRAD4();

}//end of tJobStatus();


void tJobStatusInput(unsigned uMode)
{

//uJobStatus
	OpenRow(LANG_FL_tJobStatus_uJobStatus,"black");
	printf("<input title='%s' type=text name=uJobStatus value=%u size=16 maxlength=10 "
,LANG_FT_tJobStatus_uJobStatus,uJobStatus);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uJobStatus value=%u >\n",uJobStatus);
	}
//cLabel
	OpenRow(LANG_FL_tJobStatus_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tJobStatus_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tJobStatus_uOwner,"black");
	if(guPermLevel>=1 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tJobStatus_uCreatedBy,"black");
	if(guPermLevel>=1 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tJobStatus_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tJobStatus_uModBy,"black");
	if(guPermLevel>=1 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tJobStatus_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tJobStatusInput(unsigned uMode)


void NewtJobStatus(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uJobStatus FROM tJobStatus\
				WHERE uJobStatus=%u"
							,uJobStatus);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tJobStatus("<blink>Record already exists");
		tJobStatus(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tJobStatus();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uJobStatus=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tJobStatus",uJobStatus);
	{{cProject}}Log(uJobStatus,"tJobStatus","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uJobStatus);
	tJobStatus(gcQuery);
	}

}//NewtJobStatus(unsigned uMode)


void DeletetJobStatus(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tJobStatus WHERE uJobStatus=%u AND ( uOwner=%u OR %u>9 )"
					,uJobStatus,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tJobStatus WHERE uJobStatus=%u"
					,uJobStatus);
#endif
	macro_mySQLQueryHTMLError;
	//tJobStatus("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		{{cProject}}Log(uJobStatus,"tJobStatus","Del");
#endif
		tJobStatus(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		{{cProject}}Log(uJobStatus,"tJobStatus","DelError");
#endif
		tJobStatus(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetJobStatus(void)


void Insert_tJobStatus(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tJobStatus SET uJobStatus=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uJobStatus
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tJobStatus(void)


void Update_tJobStatus(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tJobStatus SET uJobStatus=%u,cLabel='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uJobStatus
			,TextAreaSave(cLabel)
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tJobStatus(void)


void ModtJobStatus(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tJobStatus.uJobStatus,\
				tJobStatus.uModDate\
				FROM tJobStatus,tClient\
				WHERE tJobStatus.uJobStatus=%u\
				AND tJobStatus.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uJobStatus,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uJobStatus,uModDate FROM tJobStatus\
				WHERE uJobStatus=%u"
						,uJobStatus);
#else
	sprintf(gcQuery,"SELECT uJobStatus FROM tJobStatus\
				WHERE uJobStatus=%u"
						,uJobStatus);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tJobStatus("<blink>Record does not exist");
	if(i<1) tJobStatus(LANG_NBR_RECNOTEXIST);
	//if(i>1) tJobStatus("<blink>Multiple rows!");
	if(i>1) tJobStatus(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tJobStatus(LANG_NBR_EXTMOD);
#endif

	Update_tJobStatus(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tJobStatus",uJobStatus);
	{{cProject}}Log(uJobStatus,"tJobStatus","Mod");
#endif
	tJobStatus(gcQuery);

}//ModtJobStatus(void)


void tJobStatusList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttJobStatusListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tJobStatusList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttJobStatusListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uJobStatus<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



	mysql_data_seek(res,guStart-1);

	for(guN=0;guN<(guEnd-guStart+1);guN++)
	{
		field=mysql_fetch_row(res);
		if(!field)
		{
			printf("<tr><td><font face=arial,helvetica>End of data</table>");
			FooterRAD4();
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
	FooterRAD4();

}//tJobStatusList()


void CreatetJobStatus(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tJobStatus ( uJobStatus INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	macro_mySQLQueryHTMLError;
	
}//CreatetJobStatus()

//perlSAR patch1
