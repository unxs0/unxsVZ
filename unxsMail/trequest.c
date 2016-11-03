/*
FILE
	tRequest source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in trequestfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uRequest: Primary Key
static unsigned uRequest=0;
//cId: MD5 sum of default login
static char cId[65]={""};
//cLogin: Default login
static char cLogin[65]={""};
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



#define VAR_LIST_tRequest "tRequest.uRequest,tRequest.cId,tRequest.cLogin,tRequest.uOwner,tRequest.uCreatedBy,tRequest.uCreatedDate,tRequest.uModBy,tRequest.uModDate"

 //Local only
void Insert_tRequest(void);
void Update_tRequest(char *cRowid);
void ProcesstRequestListVars(pentry entries[], int x);

 //In tRequestfunc.h file included below
void ExtProcesstRequestVars(pentry entries[], int x);
void ExttRequestCommands(pentry entries[], int x);
void ExttRequestButtons(void);
void ExttRequestNavBar(void);
void ExttRequestGetHook(entry gentries[], int x);
void ExttRequestSelect(void);
void ExttRequestSelectRow(void);
void ExttRequestListSelect(void);
void ExttRequestListFilter(void);
void ExttRequestAuxTable(void);

#include "trequestfunc.h"

 //Table Variables Assignment Function
void ProcesstRequestVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uRequest"))
			sscanf(entries[i].val,"%u",&uRequest);
		else if(!strcmp(entries[i].name,"cId"))
			sprintf(cId,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLogin"))
			sprintf(cLogin,"%.64s",entries[i].val);
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
	ExtProcesstRequestVars(entries,x);

}//ProcesstRequestVars()


void ProcesstRequestListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uRequest);
                        guMode=2002;
                        tRequest("");
                }
        }
}//void ProcesstRequestListVars(pentry entries[], int x)


int tRequestCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttRequestCommands(entries,x);

	if(!strcmp(gcFunction,"tRequestTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tRequestList();
		}

		//Default
		ProcesstRequestVars(entries,x);
		tRequest("");
	}
	else if(!strcmp(gcFunction,"tRequestList"))
	{
		ProcessControlVars(entries,x);
		ProcesstRequestListVars(entries,x);
		tRequestList();
	}

	return(0);

}//tRequestCommands()


void tRequest(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttRequestSelectRow();
		else
			ExttRequestSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetRequest();
				unxsMail("New tRequest table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tRequest WHERE uRequest=%u"
						,uRequest);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uRequest);
		sprintf(cId,"%.64s",field[1]);
		sprintf(cLogin,"%.64s",field[2]);
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uCreatedBy);
		sscanf(field[5],"%lu",&uCreatedDate);
		sscanf(field[6],"%u",&uModBy);
		sscanf(field[7],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tRequest",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tRequestTools>");
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

        ExttRequestButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tRequest Record Data",100);

	if(guMode==2000 || guMode==2002)
		tRequestInput(1);
	else
		tRequestInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttRequestAuxTable();

	Footer_ism3();

}//end of tRequest();


void tRequestInput(unsigned uMode)
{

//uRequest
	OpenRow(LANG_FL_tRequest_uRequest,"black");
	printf("<input title='%s' type=text name=uRequest value=%u size=16 maxlength=10 "
,LANG_FT_tRequest_uRequest,uRequest);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uRequest value=%u >\n",uRequest);
	}
//cId
	OpenRow(LANG_FL_tRequest_cId,"black");
	printf("<input title='%s' type=text name=cId value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tRequest_cId,EncodeDoubleQuotes(cId));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cId value=\"%s\">\n",EncodeDoubleQuotes(cId));
	}
//cLogin
	OpenRow(LANG_FL_tRequest_cLogin,"black");
	printf("<input title='%s' type=text name=cLogin value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tRequest_cLogin,EncodeDoubleQuotes(cLogin));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLogin value=\"%s\">\n",EncodeDoubleQuotes(cLogin));
	}
//uOwner
	OpenRow(LANG_FL_tRequest_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tRequest_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tRequest_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tRequest_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tRequest_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tRequestInput(unsigned uMode)


void NewtRequest(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uRequest FROM tRequest\
				WHERE uRequest=%u"
							,uRequest);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tRequest("<blink>Record already exists");
		tRequest(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tRequest();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uRequest=mysql_insert_id(&gMysql);

	uCreatedDate=luGetCreatedDate("tRequest",uRequest);
	unxsMailLog(uRequest,"tRequest","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uRequest);
	tRequest(gcQuery);
	}

}//NewtRequest(unsigned uMode)


void DeletetRequest(void)
{

	sprintf(gcQuery,"DELETE FROM tRequest WHERE uRequest=%u AND ( uOwner=%u OR %u>9 )"
					,uRequest,guLoginClient,guPermLevel);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tRequest("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uRequest,"tRequest","Del");

		tRequest(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uRequest,"tRequest","DelError");

		tRequest(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetRequest(void)


void Insert_tRequest(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tRequest SET uRequest=%u,cId='%s',cLogin='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uRequest
			,TextAreaSave(cId)
			,TextAreaSave(cLogin)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tRequest(void)


void Update_tRequest(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tRequest SET uRequest=%u,cId='%s',cLogin='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uRequest
			,TextAreaSave(cId)
			,TextAreaSave(cLogin)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tRequest(void)


void ModtRequest(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uRequest,uModDate FROM tRequest WHERE uRequest=%u"
						,uRequest);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tRequest("<blink>Record does not exist");
	if(i<1) tRequest(LANG_NBR_RECNOTEXIST);
	//if(i>1) tRequest("<blink>Multiple rows!");
	if(i>1) tRequest(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tRequest(LANG_NBR_EXTMOD);


	Update_tRequest(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	uModDate=luGetModDate("tRequest",uRequest);
	unxsMailLog(uRequest,"tRequest","Mod");

	tRequest(gcQuery);

}//ModtRequest(void)


void tRequestList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttRequestListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tRequestList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttRequestListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uRequest<td><font face=arial,helvetica color=white>cId<td><font face=arial,helvetica color=white>cLogin<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime5=strtoul(field[5],NULL,10);
		char cBuf5[32];
		if(luTime5)
			ctime_r(&luTime5,cBuf5);
		else
			sprintf(cBuf5,"---");
		time_t luTime7=strtoul(field[7],NULL,10);
		char cBuf7[32];
		if(luTime7)
			ctime_r(&luTime7,cBuf7);
		else
			sprintf(cBuf7,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[3],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[4],NULL,10))
			,cBuf5
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,cBuf7
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tRequestList()


void CreatetRequest(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tRequest ( uRequest INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cId VARCHAR(64) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cLogin VARCHAR(64) NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetRequest()

