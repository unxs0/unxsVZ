/*
FILE
	tInvoiceStatus source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tinvoicestatusfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uInvoiceStatus: Primary Key
static unsigned uInvoiceStatus=0;
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



#define VAR_LIST_tInvoiceStatus "tInvoiceStatus.uInvoiceStatus,tInvoiceStatus.cLabel,tInvoiceStatus.uOwner,tInvoiceStatus.uCreatedBy,tInvoiceStatus.uCreatedDate,tInvoiceStatus.uModBy,tInvoiceStatus.uModDate"

 //Local only
void Insert_tInvoiceStatus(void);
void Update_tInvoiceStatus(char *cRowid);
void ProcesstInvoiceStatusListVars(pentry entries[], int x);

 //In tInvoiceStatusfunc.h file included below
void ExtProcesstInvoiceStatusVars(pentry entries[], int x);
void ExttInvoiceStatusCommands(pentry entries[], int x);
void ExttInvoiceStatusButtons(void);
void ExttInvoiceStatusNavBar(void);
void ExttInvoiceStatusGetHook(entry gentries[], int x);
void ExttInvoiceStatusSelect(void);
void ExttInvoiceStatusSelectRow(void);
void ExttInvoiceStatusListSelect(void);
void ExttInvoiceStatusListFilter(void);
void ExttInvoiceStatusAuxTable(void);

#include "tinvoicestatusfunc.h"

 //Table Variables Assignment Function
void ProcesstInvoiceStatusVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uInvoiceStatus"))
			sscanf(entries[i].val,"%u",&uInvoiceStatus);
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
	ExtProcesstInvoiceStatusVars(entries,x);

}//ProcesstInvoiceStatusVars()


void ProcesstInvoiceStatusListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uInvoiceStatus);
                        guMode=2002;
                        tInvoiceStatus("");
                }
        }
}//void ProcesstInvoiceStatusListVars(pentry entries[], int x)


int tInvoiceStatusCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttInvoiceStatusCommands(entries,x);

	if(!strcmp(gcFunction,"tInvoiceStatusTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tInvoiceStatusList();
		}

		//Default
		ProcesstInvoiceStatusVars(entries,x);
		tInvoiceStatus("");
	}
	else if(!strcmp(gcFunction,"tInvoiceStatusList"))
	{
		ProcessControlVars(entries,x);
		ProcesstInvoiceStatusListVars(entries,x);
		tInvoiceStatusList();
	}

	return(0);

}//tInvoiceStatusCommands()


void tInvoiceStatus(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttInvoiceStatusSelectRow();
		else
			ExttInvoiceStatusSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetInvoiceStatus();
				unxsISP("New tInvoiceStatus table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tInvoiceStatus WHERE uInvoiceStatus=%u"
						,uInvoiceStatus);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uInvoiceStatus);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tInvoiceStatus",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tInvoiceStatusTools>");
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

        ExttInvoiceStatusButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tInvoiceStatus Record Data",100);

	if(guMode==2000 || guMode==2002)
		tInvoiceStatusInput(1);
	else
		tInvoiceStatusInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttInvoiceStatusAuxTable();

	Footer_ism3();

}//end of tInvoiceStatus();


void tInvoiceStatusInput(unsigned uMode)
{

//uInvoiceStatus
	OpenRow(LANG_FL_tInvoiceStatus_uInvoiceStatus,"black");
	printf("<input title='%s' type=text name=uInvoiceStatus value=%u size=16 maxlength=10 "
,LANG_FT_tInvoiceStatus_uInvoiceStatus,uInvoiceStatus);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uInvoiceStatus value=%u >\n",uInvoiceStatus);
	}
//cLabel
	OpenRow(LANG_FL_tInvoiceStatus_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tInvoiceStatus_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tInvoiceStatus_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tInvoiceStatus_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tInvoiceStatus_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tInvoiceStatus_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tInvoiceStatus_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tInvoiceStatusInput(unsigned uMode)


void NewtInvoiceStatus(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uInvoiceStatus FROM tInvoiceStatus\
				WHERE uInvoiceStatus=%u"
							,uInvoiceStatus);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tInvoiceStatus("<blink>Record already exists");
		tInvoiceStatus(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tInvoiceStatus();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uInvoiceStatus=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tInvoiceStatus",uInvoiceStatus);
	unxsISPLog(uInvoiceStatus,"tInvoiceStatus","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uInvoiceStatus);
	tInvoiceStatus(gcQuery);
	}

}//NewtInvoiceStatus(unsigned uMode)


void DeletetInvoiceStatus(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tInvoiceStatus WHERE uInvoiceStatus=%u AND ( uOwner=%u OR %u>9 )"
					,uInvoiceStatus,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tInvoiceStatus WHERE uInvoiceStatus=%u"
					,uInvoiceStatus);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tInvoiceStatus("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uInvoiceStatus,"tInvoiceStatus","Del");
#endif
		tInvoiceStatus(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uInvoiceStatus,"tInvoiceStatus","DelError");
#endif
		tInvoiceStatus(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetInvoiceStatus(void)


void Insert_tInvoiceStatus(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tInvoiceStatus SET uInvoiceStatus=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uInvoiceStatus
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tInvoiceStatus(void)


void Update_tInvoiceStatus(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tInvoiceStatus SET uInvoiceStatus=%u,cLabel='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uInvoiceStatus
			,TextAreaSave(cLabel)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tInvoiceStatus(void)


void ModtInvoiceStatus(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uInvoiceStatus,uModDate FROM tInvoiceStatus WHERE uInvoiceStatus=%u"
						,uInvoiceStatus);
#else
	sprintf(gcQuery,"SELECT uInvoiceStatus FROM tInvoiceStatus WHERE uInvoiceStatus=%u"
						,uInvoiceStatus);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tInvoiceStatus("<blink>Record does not exist");
	if(i<1) tInvoiceStatus(LANG_NBR_RECNOTEXIST);
	//if(i>1) tInvoiceStatus("<blink>Multiple rows!");
	if(i>1) tInvoiceStatus(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tInvoiceStatus(LANG_NBR_EXTMOD);
#endif

	Update_tInvoiceStatus(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tInvoiceStatus",uInvoiceStatus);
	unxsISPLog(uInvoiceStatus,"tInvoiceStatus","Mod");
#endif
	tInvoiceStatus(gcQuery);

}//ModtInvoiceStatus(void)


void tInvoiceStatusList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttInvoiceStatusListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tInvoiceStatusList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttInvoiceStatusListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uInvoiceStatus<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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

}//tInvoiceStatusList()


void CreatetInvoiceStatus(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tInvoiceStatus ( uInvoiceStatus INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', UNIQUE (cLabel), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetInvoiceStatus()

