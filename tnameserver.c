/*
FILE
	tNameserver source code of unxsVZ.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id$
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tnameserverfunc.h while 
	RAD is still to be used.
AUTHOR/LEGAL
	(C) 2001-2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uNameserver: Primary Key
static unsigned uNameserver=0;
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

#define VAR_LIST_tNameserver "tNameserver.uNameserver,tNameserver.cLabel,tNameserver.uOwner,tNameserver.uCreatedBy,tNameserver.uCreatedDate,tNameserver.uModBy,tNameserver.uModDate"

 //Local only
void Insert_tNameserver(void);
void Update_tNameserver(char *cRowid);
void ProcesstNameserverListVars(pentry entries[], int x);

 //In tNameserverfunc.h file included below
void ExtProcesstNameserverVars(pentry entries[], int x);
void ExttNameserverCommands(pentry entries[], int x);
void ExttNameserverButtons(void);
void ExttNameserverNavBar(void);
void ExttNameserverGetHook(entry gentries[], int x);
void ExttNameserverSelect(void);
void ExttNameserverSelectRow(void);
void ExttNameserverListSelect(void);
void ExttNameserverListFilter(void);
void ExttNameserverAuxTable(void);

#include "tnameserverfunc.h"

 //Table Variables Assignment Function
void ProcesstNameserverVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uNameserver"))
			sscanf(entries[i].val,"%u",&uNameserver);
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
	ExtProcesstNameserverVars(entries,x);

}//ProcesstNameserverVars()


void ProcesstNameserverListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uNameserver);
                        guMode=2002;
                        tNameserver("");
                }
        }
}//void ProcesstNameserverListVars(pentry entries[], int x)


int tNameserverCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttNameserverCommands(entries,x);

	if(!strcmp(gcFunction,"tNameserverTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tNameserverList();
		}

		//Default
		ProcesstNameserverVars(entries,x);
		tNameserver("");
	}
	else if(!strcmp(gcFunction,"tNameserverList"))
	{
		ProcessControlVars(entries,x);
		ProcesstNameserverListVars(entries,x);
		tNameserverList();
	}

	return(0);

}//tNameserverCommands()


void tNameserver(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttNameserverSelectRow();
		else
			ExttNameserverSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetNameserver();
				unxsVZ("New tNameserver table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tNameserver WHERE uNameserver=%u"
						,uNameserver);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uNameserver);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: Container Resolver Nameserver Set",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tNameserverTools>");
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

        ExttNameserverButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tNameserver Record Data",100);

	if(guMode==2000 || guMode==2002)
		tNameserverInput(1);
	else
		tNameserverInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttNameserverAuxTable();

	Footer_ism3();

}//end of tNameserver();


void tNameserverInput(unsigned uMode)
{

//uNameserver
	OpenRow(LANG_FL_tNameserver_uNameserver,"black");
	printf("<input title='%s' type=text name=uNameserver value=%u size=16 maxlength=10 "
,LANG_FT_tNameserver_uNameserver,uNameserver);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uNameserver value=%u >\n",uNameserver);
	}
//cLabel
	OpenRow(LANG_FL_tNameserver_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tNameserver_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tNameserver_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tNameserver_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tNameserver_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tNameserver_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tNameserver_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tNameserverInput(unsigned uMode)


void NewtNameserver(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uNameserver FROM tNameserver\
				WHERE uNameserver=%u"
							,uNameserver);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tNameserver("<blink>Record already exists");
		tNameserver(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tNameserver();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uNameserver=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tNameserver",uNameserver);
	unxsVZLog(uNameserver,"tNameserver","New");

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uNameserver);
	tNameserver(gcQuery);
	}

}//NewtNameserver(unsigned uMode)


void DeletetNameserver(void)
{
	sprintf(gcQuery,"DELETE FROM tNameserver WHERE uNameserver=%u AND ( uOwner=%u OR %u>9 )"
					,uNameserver,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tNameserver("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsVZLog(uNameserver,"tNameserver","Del");
		tNameserver(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsVZLog(uNameserver,"tNameserver","DelError");
		tNameserver(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetNameserver(void)


void Insert_tNameserver(void)
{
	sprintf(gcQuery,"INSERT INTO tNameserver SET uNameserver=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uNameserver
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy);
	MYSQL_RUN;

}//void Insert_tNameserver(void)


void Update_tNameserver(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tNameserver SET uNameserver=%u,cLabel='%s',uModBy=%u,"
			"uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uNameserver
			,TextAreaSave(cLabel)
			,uModBy,
			cRowid);
	MYSQL_RUN;

}//void Update_tNameserver(void)


void ModtNameserver(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tNameserver.uNameserver,\
				tNameserver.uModDate\
				FROM tNameserver,tClient\
				WHERE tNameserver.uNameserver=%u\
				AND tNameserver.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uNameserver,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uNameserver,uModDate FROM tNameserver\
				WHERE uNameserver=%u"
						,uNameserver);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tNameserver("<blink>Record does not exist");
	if(i<1) tNameserver(LANG_NBR_RECNOTEXIST);
	//if(i>1) tNameserver("<blink>Multiple rows!");
	if(i>1) tNameserver(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tNameserver(LANG_NBR_EXTMOD);

	Update_tNameserver(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tNameserver",uNameserver);
	unxsVZLog(uNameserver,"tNameserver","Mod");
	tNameserver(gcQuery);

}//ModtNameserver(void)


void tNameserverList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttNameserverListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tNameserverList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttNameserverListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uNameserver"
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>uOwner"
		"<td><font face=arial,helvetica color=white>uCreatedBy"
		"<td><font face=arial,helvetica color=white>uCreatedDate"
		"<td><font face=arial,helvetica color=white>uModBy"
		"<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		printf("<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tNameserver&uNameserver=%s>%s</a>"
			,field[0]
			,field[0]);
		printf("<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[1]
			,ForeignKey("tClient","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[3],NULL,10))
			,cBuf4
			,ForeignKey("tClient","cLabel",strtoul(field[5],NULL,10))
			,cBuf6);
	}

	printf("</table></form>\n");
	Footer_ism3();

}//tNameserverList()


void CreatetNameserver(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tNameserver ( "
			"uNameserver INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
			"uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner),"
			"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uDatacenter INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetNameserver()

