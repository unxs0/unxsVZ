/*
FILE
	tFWStatus source code of unxsVZ.cgi
PURPOSE
AUTHOR
	(C) 2016, Gary Wallis for Unixservice, LLC.
	See LICENSE
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uFWStatus: Primary Key
static unsigned uFWStatus=0;
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



#define VAR_LIST_tFWStatus "tFWStatus.uFWStatus,tFWStatus.cLabel,tFWStatus.uOwner,tFWStatus.uCreatedBy,tFWStatus.uCreatedDate,tFWStatus.uModBy,tFWStatus.uModDate"

 //Local only
void Insert_tFWStatus(void);
void Update_tFWStatus(char *cRowid);
void ProcesstFWStatusListVars(pentry entries[], int x);

 //In tFWStatusfunc.h file included below
void ExtProcesstFWStatusVars(pentry entries[], int x);
void ExttFWStatusCommands(pentry entries[], int x);
void ExttFWStatusButtons(void);
void ExttFWStatusNavBar(void);
void ExttFWStatusGetHook(entry gentries[], int x);
void ExttFWStatusSelect(void);
void ExttFWStatusSelectRow(void);
void ExttFWStatusListSelect(void);
void ExttFWStatusListFilter(void);
void ExttFWStatusAuxTable(void);

#include "tfwstatusfunc.h"

 //Table Variables Assignment Function
void ProcesstFWStatusVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uFWStatus"))
			sscanf(entries[i].val,"%u",&uFWStatus);
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
	ExtProcesstFWStatusVars(entries,x);

}//ProcesstFWStatusVars()


void ProcesstFWStatusListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uFWStatus);
                        guMode=2002;
                        tFWStatus("");
                }
        }
}//void ProcesstFWStatusListVars(pentry entries[], int x)


int tFWStatusCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttFWStatusCommands(entries,x);

	if(!strcmp(gcFunction,"tFWStatusTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tFWStatusList();
		}

		//Default
		ProcesstFWStatusVars(entries,x);
		tFWStatus("");
	}
	else if(!strcmp(gcFunction,"tFWStatusList"))
	{
		ProcessControlVars(entries,x);
		ProcesstFWStatusListVars(entries,x);
		tFWStatusList();
	}

	return(0);

}//tFWStatusCommands()


void tFWStatus(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttFWStatusSelectRow();
		else
			ExttFWStatusSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetFWStatus();
				unxsVZ("New tFWStatus table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tFWStatus WHERE uFWStatus=%u"
						,uFWStatus);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uFWStatus);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tFWStatus",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tFWStatusTools>");
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

        ExttFWStatusButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tFWStatus Record Data",100);

	if(guMode==2000 || guMode==2002)
		tFWStatusInput(1);
	else
		tFWStatusInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttFWStatusAuxTable();

	Footer_ism3();

}//end of tFWStatus();


void tFWStatusInput(unsigned uMode)
{

//uFWStatus
	OpenRow(LANG_FL_tFWStatus_uFWStatus,"black");
	printf("<input title='%s' type=text name=uFWStatus value=%u size=16 maxlength=10 "
,LANG_FT_tFWStatus_uFWStatus,uFWStatus);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uFWStatus value=%u >\n",uFWStatus);
	}
//cLabel
	OpenRow(LANG_FL_tFWStatus_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tFWStatus_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tFWStatus_uOwner,"black");
	if(guPermLevel>=1 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tFWStatus_uCreatedBy,"black");
	if(guPermLevel>=1 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tFWStatus_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tFWStatus_uModBy,"black");
	if(guPermLevel>=1 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tFWStatus_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tFWStatusInput(unsigned uMode)


void NewtFWStatus(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uFWStatus FROM tFWStatus\
				WHERE uFWStatus=%u"
							,uFWStatus);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tFWStatus("<blink>Record already exists");
		tFWStatus(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tFWStatus();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uFWStatus=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tFWStatus",uFWStatus);
	unxsVZLog(uFWStatus,"tFWStatus","New");
	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uFWStatus);
	tFWStatus(gcQuery);
	}

}//NewtFWStatus(unsigned uMode)


void DeletetFWStatus(void)
{
	sprintf(gcQuery,"DELETE FROM tFWStatus WHERE uFWStatus=%u AND ( uOwner=%u OR %u>9 )"
					,uFWStatus,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tFWStatus("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsVZLog(uFWStatus,"tFWStatus","Del");
		tFWStatus(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsVZLog(uFWStatus,"tFWStatus","DelError");
		tFWStatus(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetFWStatus(void)


void Insert_tFWStatus(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tFWStatus SET uFWStatus=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uFWStatus
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
			);

	MYSQL_RUN;

}//void Insert_tFWStatus(void)


void Update_tFWStatus(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tFWStatus SET uFWStatus=%u,cLabel='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uFWStatus
			,TextAreaSave(cLabel)
			,uModBy
			,cRowid);

	MYSQL_RUN;

}//void Update_tFWStatus(void)


void ModtFWStatus(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tFWStatus.uFWStatus,\
				tFWStatus.uModDate\
				FROM tFWStatus,tClient\
				WHERE tFWStatus.uFWStatus=%u\
				AND tFWStatus.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uFWStatus,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uFWStatus,uModDate FROM tFWStatus\
				WHERE uFWStatus=%u"
						,uFWStatus);

	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tFWStatus("<blink>Record does not exist");
	if(i<1) tFWStatus(LANG_NBR_RECNOTEXIST);
	//if(i>1) tFWStatus("<blink>Multiple rows!");
	if(i>1) tFWStatus(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tFWStatus(LANG_NBR_EXTMOD);

	Update_tFWStatus(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tFWStatus",uFWStatus);
	unxsVZLog(uFWStatus,"tFWStatus","Mod");
	tFWStatus(gcQuery);

}//ModtFWStatus(void)


void tFWStatusList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttFWStatusListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tFWStatusList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttFWStatusListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uFWStatus<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		printf("<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tFWStatus&uFWStatus=%s>%s</a>"
			,field[0]
			,field[0]);
		printf("<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
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

}//tFWStatusList()


void CreatetFWStatus(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tFWStatus ("
			" uFWStatus INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			" cLabel VARCHAR(32) NOT NULL DEFAULT '',"
			" uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner),"
			" uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	MYSQL_RUN;

}//CreatetFWStatus()

