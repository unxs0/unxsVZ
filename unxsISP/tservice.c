/*
FILE
	tService source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tservicefunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uService: Primary Key
static unsigned uService=0;
//cLabel: Short label
static char cLabel[33]={""};
//uAvailable: Available for use
static unsigned uAvailable=0;
static char cYesNouAvailable[32]={""};
//cServer: Server domain name or IP
static char cServer[65]={""};
//cJobName: Job name used by target application
static char cJobName[33]={""};
//cComment: Customer use and other comments
static char *cComment={""};
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



#define VAR_LIST_tService "tService.uService,tService.cLabel,tService.uAvailable,tService.cServer,tService.cJobName,tService.cComment,tService.uOwner,tService.uCreatedBy,tService.uCreatedDate,tService.uModBy,tService.uModDate"

 //Local only
void Insert_tService(void);
void Update_tService(char *cRowid);
void ProcesstServiceListVars(pentry entries[], int x);

 //In tServicefunc.h file included below
void ExtProcesstServiceVars(pentry entries[], int x);
void ExttServiceCommands(pentry entries[], int x);
void ExttServiceButtons(void);
void ExttServiceNavBar(void);
void ExttServiceGetHook(entry gentries[], int x);
void ExttServiceSelect(void);
void ExttServiceSelectRow(void);
void ExttServiceListSelect(void);
void ExttServiceListFilter(void);
void ExttServiceAuxTable(void);

#include "tservicefunc.h"

 //Table Variables Assignment Function
void ProcesstServiceVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uService"))
			sscanf(entries[i].val,"%u",&uService);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uAvailable"))
			sscanf(entries[i].val,"%u",&uAvailable);
		else if(!strcmp(entries[i].name,"cYesNouAvailable"))
		{
			sprintf(cYesNouAvailable,"%.31s",entries[i].val);
			uAvailable=ReadYesNoPullDown(cYesNouAvailable);
		}
		else if(!strcmp(entries[i].name,"cServer"))
			sprintf(cServer,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cJobName"))
			sprintf(cJobName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cComment"))
			cComment=entries[i].val;
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
	ExtProcesstServiceVars(entries,x);

}//ProcesstServiceVars()


void ProcesstServiceListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uService);
                        guMode=2002;
                        tService("");
                }
        }
}//void ProcesstServiceListVars(pentry entries[], int x)


int tServiceCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttServiceCommands(entries,x);

	if(!strcmp(gcFunction,"tServiceTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tServiceList();
		}

		//Default
		ProcesstServiceVars(entries,x);
		tService("");
	}
	else if(!strcmp(gcFunction,"tServiceList"))
	{
		ProcessControlVars(entries,x);
		ProcesstServiceListVars(entries,x);
		tServiceList();
	}

	return(0);

}//tServiceCommands()


void tService(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttServiceSelectRow();
		else
			ExttServiceSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetService();
				unxsISP("New tService table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tService WHERE uService=%u"
						,uService);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uService);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uAvailable);
		sprintf(cServer,"%.64s",field[3]);
		sprintf(cJobName,"%.32s",field[4]);
		cComment=field[5];
		sscanf(field[6],"%u",&uOwner);
		sscanf(field[7],"%u",&uCreatedBy);
		sscanf(field[8],"%lu",&uCreatedDate);
		sscanf(field[9],"%u",&uModBy);
		sscanf(field[10],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tService",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tServiceTools>");
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

        ExttServiceButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tService Record Data",100);

	if(guMode==2000 || guMode==2002)
		tServiceInput(1);
	else
		tServiceInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttServiceAuxTable();

	Footer_ism3();

}//end of tService();


void tServiceInput(unsigned uMode)
{

//uService
	OpenRow(LANG_FL_tService_uService,"black");
	printf("<input title='%s' type=text name=uService value=%u size=16 maxlength=10 "
,LANG_FT_tService_uService,uService);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uService value=%u >\n",uService);
	}
//cLabel
	OpenRow(LANG_FL_tService_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tService_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uAvailable
	OpenRow(LANG_FL_tService_uAvailable,"black");
	if(guPermLevel>=10 && uMode)
		YesNoPullDown("uAvailable",uAvailable,1);
	else
		YesNoPullDown("uAvailable",uAvailable,0);
//cServer
	OpenRow(LANG_FL_tService_cServer,"black");
	printf("<input title='%s' type=text name=cServer value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tService_cServer,EncodeDoubleQuotes(cServer));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cServer value=\"%s\">\n",EncodeDoubleQuotes(cServer));
	}
//cJobName
	OpenRow(LANG_FL_tService_cJobName,"black");
	printf("<input title='%s' type=text name=cJobName value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tService_cJobName,EncodeDoubleQuotes(cJobName));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cJobName value=\"%s\">\n",EncodeDoubleQuotes(cJobName));
	}
//cComment
	OpenRow(LANG_FL_tService_cComment,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cComment "
,LANG_FT_tService_cComment);
	if(guPermLevel>=0 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cComment);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cComment);
		printf("<input type=hidden name=cComment value=\"%s\" >\n",EncodeDoubleQuotes(cComment));
	}
//uOwner
	OpenRow(LANG_FL_tService_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tService_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tService_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tService_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tService_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tServiceInput(unsigned uMode)


void NewtService(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uService FROM tService\
				WHERE uService=%u"
							,uService);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tService("<blink>Record already exists");
		tService(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tService();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uService=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tService",uService);
	unxsISPLog(uService,"tService","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uService);
	tService(gcQuery);
	}

}//NewtService(unsigned uMode)


void DeletetService(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tService WHERE uService=%u AND ( uOwner=%u OR %u>9 )"
					,uService,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tService WHERE uService=%u"
					,uService);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tService("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uService,"tService","Del");
#endif
		tService(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uService,"tService","DelError");
#endif
		tService(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetService(void)


void Insert_tService(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tService SET uService=%u,cLabel='%s',uAvailable=%u,cServer='%s',cJobName='%s',cComment='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uService
			,TextAreaSave(cLabel)
			,uAvailable
			,TextAreaSave(cServer)
			,TextAreaSave(cJobName)
			,TextAreaSave(cComment)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tService(void)


void Update_tService(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tService SET uService=%u,cLabel='%s',uAvailable=%u,cServer='%s',cJobName='%s',cComment='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uService
			,TextAreaSave(cLabel)
			,uAvailable
			,TextAreaSave(cServer)
			,TextAreaSave(cJobName)
			,TextAreaSave(cComment)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tService(void)


void ModtService(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uService,uModDate FROM tService WHERE uService=%u"
						,uService);
#else
	sprintf(gcQuery,"SELECT uService FROM tService WHERE uService=%u"
						,uService);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tService("<blink>Record does not exist");
	if(i<1) tService(LANG_NBR_RECNOTEXIST);
	//if(i>1) tService("<blink>Multiple rows!");
	if(i>1) tService(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tService(LANG_NBR_EXTMOD);
#endif

	Update_tService(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tService",uService);
	unxsISPLog(uService,"tService","Mod");
#endif
	tService(gcQuery);

}//ModtService(void)


void tServiceList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttServiceListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tServiceList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttServiceListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uService<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uAvailable<td><font face=arial,helvetica color=white>cServer<td><font face=arial,helvetica color=white>cJobName<td><font face=arial,helvetica color=white>cComment<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime8=strtoul(field[8],NULL,10);
		char cBuf8[32];
		if(luTime8)
			ctime_r(&luTime8,cBuf8);
		else
			sprintf(cBuf8,"---");
		time_t luTime10=strtoul(field[10],NULL,10);
		char cBuf10[32];
		if(luTime10)
			ctime_r(&luTime10,cBuf10);
		else
			sprintf(cBuf10,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,cBuf2
			,field[3]
			,field[4]
			,field[5]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[7],NULL,10))
			,cBuf8
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[9],NULL,10))
			,cBuf10
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tServiceList()


void CreatetService(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tService ( uService INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cServer VARCHAR(64) NOT NULL DEFAULT '', cComment TEXT NOT NULL DEFAULT '', cJobName VARCHAR(32) NOT NULL DEFAULT '', uAvailable INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetService()

