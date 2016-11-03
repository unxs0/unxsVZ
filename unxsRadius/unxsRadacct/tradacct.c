/*
FILE
	tRadacct source code of unxsRadacct.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tradacctfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uRadacct: Primary key for interface use only
static unsigned uRadacct=0;
//cLogin: Login name
static char cLogin[101]={""};
//cSessionID: Radius session ID
static char cSessionID[33]={""};
//cNAS: NAS server IP
static char cNAS[33]={""};
//uConnectTime: Seconds from stop record
static time_t uConnectTime=0;
//uPort: NAS port used during session
static unsigned uPort=0;
//uPortType: NAS port_type
static unsigned uPortType=0;
//uService: Service type PPP/Telnet/Etc
static unsigned uService=0;
//uProtocol: Session Protocol
static unsigned uProtocol=0;
//cIP: IP assigned
static char cIP[33]={""};
//cLine: PoP number called
static char cLine[33]={""};
//cCallerID: Caller ID
static char cCallerID[33]={""};
//uInBytes: Bytes from user
static time_t uInBytes=0;
//uOutBytes: Bytes to user
static unsigned uOutBytes=0;
//uTermCause: Termination cause code
static unsigned uTermCause=0;
//uStartTime: Start record timestamp
static time_t uStartTime=0;
//uStopTime: Stop record timestamp
static time_t uStopTime=0;
//cInfo: Connect info
static char cInfo[33]={""};



#define VAR_LIST_tRadacct "tRadacct.uRadacct,tRadacct.cLogin,tRadacct.cSessionID,tRadacct.cNAS,tRadacct.uConnectTime,tRadacct.uPort,tRadacct.uPortType,tRadacct.uService,tRadacct.uProtocol,tRadacct.cIP,tRadacct.cLine,tRadacct.cCallerID,tRadacct.uInBytes,tRadacct.uOutBytes,tRadacct.uTermCause,tRadacct.uStartTime,tRadacct.uStopTime,tRadacct.cInfo"

 //Local only
void Insert_tRadacct(void);
void Update_tRadacct(char *cRowid);
void ProcesstRadacctListVars(pentry entries[], int x);

 //In tRadacctfunc.h file included below
void ExtProcesstRadacctVars(pentry entries[], int x);
void ExttRadacctCommands(pentry entries[], int x);
void ExttRadacctButtons(void);
void ExttRadacctNavBar(void);
void ExttRadacctGetHook(entry gentries[], int x);
void ExttRadacctSelect(void);
void ExttRadacctSelectRow(void);
void ExttRadacctListSelect(void);
void ExttRadacctListFilter(void);
void ExttRadacctAuxTable(void);

#include "tradacctfunc.h"

 //Table Variables Assignment Function
void ProcesstRadacctVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uRadacct"))
			sscanf(entries[i].val,"%u",&uRadacct);
		else if(!strcmp(entries[i].name,"cLogin"))
			sprintf(cLogin,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cSessionID"))
			sprintf(cSessionID,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cNAS"))
			sprintf(cNAS,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uConnectTime"))
			sscanf(entries[i].val,"%lu",&uConnectTime);
		else if(!strcmp(entries[i].name,"uPort"))
			sscanf(entries[i].val,"%u",&uPort);
		else if(!strcmp(entries[i].name,"uPortType"))
			sscanf(entries[i].val,"%u",&uPortType);
		else if(!strcmp(entries[i].name,"uService"))
			sscanf(entries[i].val,"%u",&uService);
		else if(!strcmp(entries[i].name,"uProtocol"))
			sscanf(entries[i].val,"%u",&uProtocol);
		else if(!strcmp(entries[i].name,"cIP"))
			sprintf(cIP,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLine"))
			sprintf(cLine,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cCallerID"))
			sprintf(cCallerID,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uInBytes"))
			sscanf(entries[i].val,"%lu",&uInBytes);
		else if(!strcmp(entries[i].name,"uOutBytes"))
			sscanf(entries[i].val,"%u",&uOutBytes);
		else if(!strcmp(entries[i].name,"uTermCause"))
			sscanf(entries[i].val,"%u",&uTermCause);
		else if(!strcmp(entries[i].name,"uStartTime"))
			sscanf(entries[i].val,"%lu",&uStartTime);
		else if(!strcmp(entries[i].name,"uStopTime"))
			sscanf(entries[i].val,"%lu",&uStopTime);
		else if(!strcmp(entries[i].name,"cInfo"))
			sprintf(cInfo,"%.32s",entries[i].val);

	}

	//After so we can overwrite form data if needed.
	ExtProcesstRadacctVars(entries,x);

}//ProcesstRadacctVars()


void ProcesstRadacctListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uRadacct);
                        guMode=2002;
                        tRadacct("");
                }
        }
}//void ProcesstRadacctListVars(pentry entries[], int x)


int tRadacctCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttRadacctCommands(entries,x);

	if(!strcmp(gcFunction,"tRadacctTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tRadacctList();
		}

		//Default
		ProcesstRadacctVars(entries,x);
		tRadacct("");
	}
	else if(!strcmp(gcFunction,"tRadacctList"))
	{
		ProcessControlVars(entries,x);
		ProcesstRadacctListVars(entries,x);
		tRadacctList();
	}

	return(0);

}//tRadacctCommands()


void tRadacct(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttRadacctSelectRow();
		else
			ExttRadacctSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetRadacct();
				unxsRadacct("New tRadacct table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tRadacct WHERE uRadacct=%u"
						,uRadacct);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uRadacct);
		sprintf(cLogin,"%.100s",field[1]);
		sprintf(cSessionID,"%.32s",field[2]);
		sprintf(cNAS,"%.32s",field[3]);
		sscanf(field[4],"%lu",&uConnectTime);
		sscanf(field[5],"%u",&uPort);
		sscanf(field[6],"%u",&uPortType);
		sscanf(field[7],"%u",&uService);
		sscanf(field[8],"%u",&uProtocol);
		sprintf(cIP,"%.32s",field[9]);
		sprintf(cLine,"%.32s",field[10]);
		sprintf(cCallerID,"%.32s",field[11]);
		sscanf(field[12],"%lu",&uInBytes);
		sscanf(field[13],"%u",&uOutBytes);
		sscanf(field[14],"%u",&uTermCause);
		sscanf(field[15],"%lu",&uStartTime);
		sscanf(field[16],"%lu",&uStopTime);
		sprintf(cInfo,"%.32s",field[17]);

		}

	}//Internal Skip

	Header_ism3(":: tRadacct",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tRadacctTools>");
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

        ExttRadacctButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tRadacct Record Data",100);

	if(guMode==2000 || guMode==2002)
		tRadacctInput(1);
	else
		tRadacctInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttRadacctAuxTable();

	Footer_ism3();

}//end of tRadacct();


void tRadacctInput(unsigned uMode)
{

//uRadacct
	OpenRow(LANG_FL_tRadacct_uRadacct,"black");
	printf("<input title='%s' type=text name=uRadacct value=%u size=16 maxlength=11 "
,LANG_FT_tRadacct_uRadacct,uRadacct);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uRadacct value=%u >\n",uRadacct);
	}
//cLogin
	OpenRow(LANG_FL_tRadacct_cLogin,"black");
	printf("<input title='%s' type=text name=cLogin value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tRadacct_cLogin,EncodeDoubleQuotes(cLogin));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLogin value=\"%s\">\n",EncodeDoubleQuotes(cLogin));
	}
//cSessionID
	OpenRow(LANG_FL_tRadacct_cSessionID,"black");
	printf("<input title='%s' type=text name=cSessionID value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tRadacct_cSessionID,EncodeDoubleQuotes(cSessionID));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cSessionID value=\"%s\">\n",EncodeDoubleQuotes(cSessionID));
	}
//cNAS
	OpenRow(LANG_FL_tRadacct_cNAS,"black");
	printf("<input title='%s' type=text name=cNAS value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tRadacct_cNAS,EncodeDoubleQuotes(cNAS));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cNAS value=\"%s\">\n",EncodeDoubleQuotes(cNAS));
	}
//uConnectTime
	OpenRow(LANG_FL_tRadacct_uConnectTime,"black");
	printf("<input title='%s' type=text name=uConnectTime value=%lu size=20 maxlength=11 "
,LANG_FT_tRadacct_uConnectTime,uConnectTime);
	if(guPermLevel>=30 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uConnectTime value=%lu >\n",uConnectTime);
	}
//uPort
	OpenRow(LANG_FL_tRadacct_uPort,"black");
	printf("<input title='%s' type=text name=uPort value=%u size=16 maxlength=11 "
,LANG_FT_tRadacct_uPort,uPort);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uPort value=%u >\n",uPort);
	}
//uPortType
	OpenRow(LANG_FL_tRadacct_uPortType,"black");
	printf("<input title='%s' type=text name=uPortType value=%u size=16 maxlength=11 "
,LANG_FT_tRadacct_uPortType,uPortType);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uPortType value=%u >\n",uPortType);
	}
//uService
	OpenRow(LANG_FL_tRadacct_uService,"black");
	printf("<input title='%s' type=text name=uService value=%u size=16 maxlength=11 "
,LANG_FT_tRadacct_uService,uService);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uService value=%u >\n",uService);
	}
//uProtocol
	OpenRow(LANG_FL_tRadacct_uProtocol,"black");
	printf("<input title='%s' type=text name=uProtocol value=%u size=16 maxlength=11 "
,LANG_FT_tRadacct_uProtocol,uProtocol);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uProtocol value=%u >\n",uProtocol);
	}
//cIP
	OpenRow(LANG_FL_tRadacct_cIP,"black");
	printf("<input title='%s' type=text name=cIP value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tRadacct_cIP,EncodeDoubleQuotes(cIP));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cIP value=\"%s\">\n",EncodeDoubleQuotes(cIP));
	}
//cLine
	OpenRow(LANG_FL_tRadacct_cLine,"black");
	printf("<input title='%s' type=text name=cLine value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tRadacct_cLine,EncodeDoubleQuotes(cLine));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLine value=\"%s\">\n",EncodeDoubleQuotes(cLine));
	}
//cCallerID
	OpenRow(LANG_FL_tRadacct_cCallerID,"black");
	printf("<input title='%s' type=text name=cCallerID value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tRadacct_cCallerID,EncodeDoubleQuotes(cCallerID));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cCallerID value=\"%s\">\n",EncodeDoubleQuotes(cCallerID));
	}
//uInBytes
	OpenRow(LANG_FL_tRadacct_uInBytes,"black");
	printf("<input title='%s' type=text name=uInBytes value=%lu size=20 maxlength=11 "
,LANG_FT_tRadacct_uInBytes,uInBytes);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uInBytes value=%lu >\n",uInBytes);
	}
//uOutBytes
	OpenRow(LANG_FL_tRadacct_uOutBytes,"black");
	printf("<input title='%s' type=text name=uOutBytes value=%u size=16 maxlength=11 "
,LANG_FT_tRadacct_uOutBytes,uOutBytes);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uOutBytes value=%u >\n",uOutBytes);
	}
//uTermCause
	OpenRow(LANG_FL_tRadacct_uTermCause,"black");
	printf("<input title='%s' type=text name=uTermCause value=%u size=16 maxlength=11 "
,LANG_FT_tRadacct_uTermCause,uTermCause);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uTermCause value=%u >\n",uTermCause);
	}
//uStartTime
	OpenRow(LANG_FL_tRadacct_uStartTime,"black");
	if(uStartTime)
		printf("<input type=text name=cuStartTime value='%s' disabled>\n",ctime(&uStartTime));
	else
		printf("<input type=text name=cuStartTime value='---' disabled>\n");
	printf("<input type=hidden name=uStartTime value=%lu>\n",uStartTime);
//uStopTime
	OpenRow(LANG_FL_tRadacct_uStopTime,"black");
	if(uStopTime)
		printf("<input type=text name=cuStopTime value='%s' disabled>\n",ctime(&uStopTime));
	else
		printf("<input type=text name=cuStopTime value='---' disabled>\n");
	printf("<input type=hidden name=uStopTime value=%lu>\n",uStopTime);
//cInfo
	OpenRow(LANG_FL_tRadacct_cInfo,"black");
	printf("<input title='%s' type=text name=cInfo value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tRadacct_cInfo,EncodeDoubleQuotes(cInfo));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cInfo value=\"%s\">\n",EncodeDoubleQuotes(cInfo));
	}
	printf("</tr>\n");



}//void tRadacctInput(unsigned uMode)


void NewtRadacct(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uRadacct FROM tRadacct\
				WHERE uRadacct=%u"
							,uRadacct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tRadacct("<blink>Record already exists");
		tRadacct(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tRadacct();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uRadacct=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tRadacct",uRadacct);
	unxsRadacctLog(uRadacct,"tRadacct","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uRadacct);
	tRadacct(gcQuery);
	}

}//NewtRadacct(unsigned uMode)


void DeletetRadacct(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tRadacct WHERE uRadacct=%u AND ( uOwner=%u OR %u>9 )"
					,uRadacct,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tRadacct WHERE uRadacct=%u"
					,uRadacct);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tRadacct("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsRadacctLog(uRadacct,"tRadacct","Del");
#endif
		tRadacct(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsRadacctLog(uRadacct,"tRadacct","DelError");
#endif
		tRadacct(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetRadacct(void)


void Insert_tRadacct(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tRadacct SET uRadacct=%u,cLogin='%s',cSessionID='%s',cNAS='%s',uConnectTime=%lu,uPort=%u,uPortType=%u,uService=%u,uProtocol=%u,cIP='%s',cLine='%s',cCallerID='%s',uInBytes=%lu,uOutBytes=%u,uTermCause=%u,uStartTime=%lu,uStopTime=%lu,cInfo='%s'",
			uRadacct
			,TextAreaSave(cLogin)
			,TextAreaSave(cSessionID)
			,TextAreaSave(cNAS)
			,uConnectTime
			,uPort
			,uPortType
			,uService
			,uProtocol
			,TextAreaSave(cIP)
			,TextAreaSave(cLine)
			,TextAreaSave(cCallerID)
			,uInBytes
			,uOutBytes
			,uTermCause
			,uStartTime
			,uStopTime
			,TextAreaSave(cInfo)
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tRadacct(void)


void Update_tRadacct(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tRadacct SET uRadacct=%u,cLogin='%s',cSessionID='%s',cNAS='%s',uConnectTime=%lu,uPort=%u,uPortType=%u,uService=%u,uProtocol=%u,cIP='%s',cLine='%s',cCallerID='%s',uInBytes=%lu,uOutBytes=%u,uTermCause=%u,uStartTime=%lu,uStopTime=%lu,cInfo='%s' WHERE _rowid=%s",
			uRadacct
			,TextAreaSave(cLogin)
			,TextAreaSave(cSessionID)
			,TextAreaSave(cNAS)
			,uConnectTime
			,uPort
			,uPortType
			,uService
			,uProtocol
			,TextAreaSave(cIP)
			,TextAreaSave(cLine)
			,TextAreaSave(cCallerID)
			,uInBytes
			,uOutBytes
			,uTermCause
			,uStartTime
			,uStopTime
			,TextAreaSave(cInfo)
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tRadacct(void)


void ModtRadacct(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uRadacct,uModDate FROM tRadacct WHERE uRadacct=%u"
						,uRadacct);
#else
	sprintf(gcQuery,"SELECT uRadacct FROM tRadacct WHERE uRadacct=%u"
						,uRadacct);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tRadacct("<blink>Record does not exist");
	if(i<1) tRadacct(LANG_NBR_RECNOTEXIST);
	//if(i>1) tRadacct("<blink>Multiple rows!");
	if(i>1) tRadacct(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tRadacct(LANG_NBR_EXTMOD);
#endif

	Update_tRadacct(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tRadacct",uRadacct);
	unxsRadacctLog(uRadacct,"tRadacct","Mod");
#endif
	tRadacct(gcQuery);

}//ModtRadacct(void)


void tRadacctList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttRadacctListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tRadacctList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttRadacctListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uRadacct<td><font face=arial,helvetica color=white>cLogin<td><font face=arial,helvetica color=white>cSessionID<td><font face=arial,helvetica color=white>cNAS<td><font face=arial,helvetica color=white>uConnectTime<td><font face=arial,helvetica color=white>uPort<td><font face=arial,helvetica color=white>uPortType<td><font face=arial,helvetica color=white>uService<td><font face=arial,helvetica color=white>uProtocol<td><font face=arial,helvetica color=white>cIP<td><font face=arial,helvetica color=white>cLine<td><font face=arial,helvetica color=white>cCallerID<td><font face=arial,helvetica color=white>uInBytes<td><font face=arial,helvetica color=white>uOutBytes<td><font face=arial,helvetica color=white>uTermCause<td><font face=arial,helvetica color=white>uStartTime<td><font face=arial,helvetica color=white>uStopTime<td><font face=arial,helvetica color=white>cInfo</tr>");



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
		time_t luTime15=strtoul(field[15],NULL,10);
		char cBuf15[32];
		if(luTime15)
			ctime_r(&luTime15,cBuf15);
		else
			sprintf(cBuf15,"---");
		time_t luTime16=strtoul(field[16],NULL,10);
		char cBuf16[32];
		if(luTime16)
			ctime_r(&luTime16,cBuf16);
		else
			sprintf(cBuf16,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,field[4]
			,field[5]
			,field[6]
			,field[7]
			,field[8]
			,field[9]
			,field[10]
			,field[11]
			,field[12]
			,field[13]
			,field[14]
			,cBuf15
			,cBuf16
			,field[17]
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tRadacctList()


void CreatetRadacct(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tRadacct ( uPortType INT UNSIGNED NOT NULL DEFAULT 0, uPort INT UNSIGNED NOT NULL DEFAULT 0, cNAS VARCHAR(32) NOT NULL DEFAULT '',index (cNAS), cLogin VARCHAR(100) NOT NULL DEFAULT '',index (cLogin), uConnectTime BIGINT UNSIGNED NOT NULL DEFAULT 0,index (uConnectTime), cSessionID VARCHAR(32) NOT NULL DEFAULT '',index (cSessionID), uService INT UNSIGNED NOT NULL DEFAULT 0, uProtocol INT UNSIGNED NOT NULL DEFAULT 0, cIP VARCHAR(32) NOT NULL DEFAULT '', cLine VARCHAR(32) NOT NULL DEFAULT '', cCallerID VARCHAR(32) NOT NULL DEFAULT '', uInBytes BIGINT UNSIGNED NOT NULL DEFAULT 0, uOutBytes INT UNSIGNED NOT NULL DEFAULT 0, uTermCause INT UNSIGNED NOT NULL DEFAULT 0, uStopTime INT UNSIGNED NOT NULL DEFAULT 0,index (uStopTime), uStartTime INT UNSIGNED NOT NULL DEFAULT 0,index (uStartTime), cInfo VARCHAR(32) NOT NULL DEFAULT '', uRadacct INT UNSIGNED PRIMARY KEY AUTO_INCREMENT )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetRadacct()

