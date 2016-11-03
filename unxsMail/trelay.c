/*
FILE
	tRelay source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in trelayfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uRelay: Primary Key
static unsigned uRelay=0;
//cDomain: Domain name
static char cDomain[33]={""};
//cTransport: Mail Transport
static char cTransport[65]={""};
//uServerGroup: ServerGroup account is on
static unsigned uServerGroup=0;
static char cuServerGroupPullDown[256]={""};
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



#define VAR_LIST_tRelay "tRelay.uRelay,tRelay.cDomain,tRelay.cTransport,tRelay.uServerGroup,tRelay.uOwner,tRelay.uCreatedBy,tRelay.uCreatedDate,tRelay.uModBy,tRelay.uModDate"

 //Local only
void Insert_tRelay(void);
void Update_tRelay(char *cRowid);
void ProcesstRelayListVars(pentry entries[], int x);

 //In tRelayfunc.h file included below
void ExtProcesstRelayVars(pentry entries[], int x);
void ExttRelayCommands(pentry entries[], int x);
void ExttRelayButtons(void);
void ExttRelayNavBar(void);
void ExttRelayGetHook(entry gentries[], int x);
void ExttRelaySelect(void);
void ExttRelaySelectRow(void);
void ExttRelayListSelect(void);
void ExttRelayListFilter(void);
void ExttRelayAuxTable(void);

#include "trelayfunc.h"

 //Table Variables Assignment Function
void ProcesstRelayVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uRelay"))
			sscanf(entries[i].val,"%u",&uRelay);
		else if(!strcmp(entries[i].name,"cDomain"))
			sprintf(cDomain,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cTransport"))
			sprintf(cTransport,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uServerGroup"))
			sscanf(entries[i].val,"%u",&uServerGroup);
		else if(!strcmp(entries[i].name,"cuServerGroupPullDown"))
		{
			sprintf(cuServerGroupPullDown,"%.255s",entries[i].val);
			uServerGroup=ReadPullDown("tServerGroup","cLabel",cuServerGroupPullDown);
		}
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
	ExtProcesstRelayVars(entries,x);

}//ProcesstRelayVars()


void ProcesstRelayListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uRelay);
                        guMode=2002;
                        tRelay("");
                }
        }
}//void ProcesstRelayListVars(pentry entries[], int x)


int tRelayCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttRelayCommands(entries,x);

	if(!strcmp(gcFunction,"tRelayTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tRelayList();
		}

		//Default
		ProcesstRelayVars(entries,x);
		tRelay("");
	}
	else if(!strcmp(gcFunction,"tRelayList"))
	{
		ProcessControlVars(entries,x);
		ProcesstRelayListVars(entries,x);
		tRelayList();
	}

	return(0);

}//tRelayCommands()


void tRelay(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttRelaySelectRow();
		else
			ExttRelaySelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetRelay();
				unxsMail("New tRelay table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tRelay WHERE uRelay=%u"
						,uRelay);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uRelay);
		sprintf(cDomain,"%.32s",field[1]);
		sprintf(cTransport,"%.64s",field[2]);
		sscanf(field[3],"%u",&uServerGroup);
		sscanf(field[4],"%u",&uOwner);
		sscanf(field[5],"%u",&uCreatedBy);
		sscanf(field[6],"%lu",&uCreatedDate);
		sscanf(field[7],"%u",&uModBy);
		sscanf(field[8],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tRelay",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tRelayTools>");
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

        ExttRelayButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tRelay Record Data",100);

	if(guMode==2000 || guMode==2002)
		tRelayInput(1);
	else
		tRelayInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttRelayAuxTable();

	Footer_ism3();

}//end of tRelay();


void tRelayInput(unsigned uMode)
{

//uRelay
	OpenRow(LANG_FL_tRelay_uRelay,"black");
	printf("<input title='%s' type=text name=uRelay value=%u size=16 maxlength=10 "
,LANG_FT_tRelay_uRelay,uRelay);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uRelay value=%u >\n",uRelay);
	}
//cDomain
	OpenRow(LANG_FL_tRelay_cDomain,"black");
	printf("<input title='%s' type=text name=cDomain value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tRelay_cDomain,EncodeDoubleQuotes(cDomain));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cDomain value=\"%s\">\n",EncodeDoubleQuotes(cDomain));
	}
//cTransport
	OpenRow(LANG_FL_tRelay_cTransport,"black");
	printf("<input title='%s' type=text name=cTransport value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tRelay_cTransport,EncodeDoubleQuotes(cTransport));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cTransport value=\"%s\">\n",EncodeDoubleQuotes(cTransport));
	}
//uServerGroup
	OpenRow(LANG_FL_tRelay_uServerGroup,"black");
	if(guPermLevel>=8 && uMode)
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,1);
	else
		tTablePullDown("tServerGroup;cuServerGroupPullDown","cLabel","cLabel",uServerGroup,0);
//uOwner
	OpenRow(LANG_FL_tRelay_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tRelay_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tRelay_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tRelay_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tRelay_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tRelayInput(unsigned uMode)


void NewtRelay(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uRelay FROM tRelay\
				WHERE uRelay=%u"
							,uRelay);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tRelay("<blink>Record already exists");
		tRelay(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tRelay();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uRelay=mysql_insert_id(&gMysql);

	uCreatedDate=luGetCreatedDate("tRelay",uRelay);
	unxsMailLog(uRelay,"tRelay","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uRelay);
	tRelay(gcQuery);
	}

}//NewtRelay(unsigned uMode)


void DeletetRelay(void)
{

	sprintf(gcQuery,"DELETE FROM tRelay WHERE uRelay=%u AND ( uOwner=%u OR %u>9 )"
					,uRelay,guLoginClient,guPermLevel);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tRelay("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uRelay,"tRelay","Del");

		tRelay(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uRelay,"tRelay","DelError");

		tRelay(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetRelay(void)


void Insert_tRelay(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tRelay SET uRelay=%u,cDomain='%s',cTransport='%s',uServerGroup=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uRelay
			,TextAreaSave(cDomain)
			,TextAreaSave(cTransport)
			,uServerGroup
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tRelay(void)


void Update_tRelay(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tRelay SET uRelay=%u,cDomain='%s',cTransport='%s',uServerGroup=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uRelay
			,TextAreaSave(cDomain)
			,TextAreaSave(cTransport)
			,uServerGroup
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tRelay(void)


void ModtRelay(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uRelay,uModDate FROM tRelay WHERE uRelay=%u",uRelay);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tRelay("<blink>Record does not exist");
	if(i<1) tRelay(LANG_NBR_RECNOTEXIST);
	//if(i>1) tRelay("<blink>Multiple rows!");
	if(i>1) tRelay(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tRelay(LANG_NBR_EXTMOD);


	Update_tRelay(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	uModDate=luGetModDate("tRelay",uRelay);
	unxsMailLog(uRelay,"tRelay","Mod");

	tRelay(gcQuery);

}//ModtRelay(void)


void tRelayList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttRelayListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tRelayList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttRelayListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uRelay<td><font face=arial,helvetica color=white>cDomain<td><font face=arial,helvetica color=white>cTransport<td><font face=arial,helvetica color=white>uServerGroup<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime6=strtoul(field[6],NULL,10);
		char cBuf6[32];
		if(luTime6)
			ctime_r(&luTime6,cBuf6);
		else
			sprintf(cBuf6,"---");
		time_t luTime8=strtoul(field[8],NULL,10);
		char cBuf8[32];
		if(luTime8)
			ctime_r(&luTime8,cBuf8);
		else
			sprintf(cBuf8,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,ForeignKey("tServerGroup","cLabel",strtoul(field[3],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[4],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,cBuf6
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[7],NULL,10))
			,cBuf8
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tRelayList()


void CreatetRelay(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tRelay ( uRelay INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cDomain VARCHAR(32) NOT NULL DEFAULT '',index (cDomain,uServerGroup), uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cTransport VARCHAR(64) NOT NULL DEFAULT '', uServerGroup INT UNSIGNED NOT NULL DEFAULT 0,index (uServerGroup) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetRelay()

