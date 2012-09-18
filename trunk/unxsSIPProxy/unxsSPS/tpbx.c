/*
FILE
	$Id: module.c 1955 2012-05-22 21:28:32Z Colin $
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in tpbxfunc.h
	while unxsRAD can still to be used to change this schema dependent file.
AUTHOR
	(C) 2001-2012 Gary Wallis for Unixservice, LLC.
TEMPLATE VARS AND FUNCTIONS
	funcModuleCreateQuery
	funcModuleInsertQuery
	funcModuleListPrint
	funcModuleListTable
	funcModuleLoadVars
	funcModuleProcVars
	funcModuleRAD3Input
	funcModuleUpdateQuery
	funcModuleVars
	funcModuleVarList
	cProject
	cTableKey
	cTableName
	cTableNameLC
	cTableTitle
*/


#include "mysqlrad.h"

//Table Variables
static unsigned uPBX=0;
static char cHostname[65]={""};
static unsigned uServer=0;
static char cuServerPullDown[256]={""};
static char *cComment={""};
static unsigned uOwner=0;
static unsigned uCreatedBy=0;
static time_t uCreatedDate=0;
static unsigned uModBy=0;
static time_t uModDate=0;


#define VAR_LIST_tPBX "tPBX.uPBX,tPBX.cHostname,tPBX.uServer,tPBX.cComment,tPBX.uOwner,tPBX.uCreatedBy,tPBX.uCreatedDate,tPBX.uModBy,tPBX.uModDate"

 //Local only
void Insert_tPBX(void);
void Update_tPBX(char *cRowid);
void ProcesstPBXListVars(pentry entries[], int x);

 //In tPBXfunc.h file included below
void ExtProcesstPBXVars(pentry entries[], int x);
void ExttPBXCommands(pentry entries[], int x);
void ExttPBXButtons(void);
void ExttPBXNavBar(void);
void ExttPBXGetHook(entry gentries[], int x);
void ExttPBXSelect(void);
void ExttPBXSelectRow(void);
void ExttPBXListSelect(void);
void ExttPBXListFilter(void);
void ExttPBXAuxTable(void);

#include "tpbxfunc.h"

 //Table Variables Assignment Function
void ProcesstPBXVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		
		if(!strcmp(entries[i].name,"uPBX"))
			sscanf(entries[i].val,"%u",&uPBX);
		else if(!strcmp(entries[i].name,"cHostname"))
			sprintf(cHostname,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"uServer"))
			sscanf(entries[i].val,"%u",&uServer);
		else if(!strcmp(entries[i].name,"cuServerPullDown"))
		{
			sprintf(cuServerPullDown,"%.255s",entries[i].val);
			uServer=ReadPullDown("tServer","cLabel",cuServerPullDown);
		}
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
	ExtProcesstPBXVars(entries,x);

}//ProcesstPBXVars()


void ProcesstPBXListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uPBX);
                        guMode=2002;
                        tPBX("");
                }
        }
}//void ProcesstPBXListVars(pentry entries[], int x)


int tPBXCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttPBXCommands(entries,x);

	if(!strcmp(gcFunction,"tPBXTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tPBXList();
		}

		//Default
		ProcesstPBXVars(entries,x);
		tPBX("");
	}
	else if(!strcmp(gcFunction,"tPBXList"))
	{
		ProcessControlVars(entries,x);
		ProcesstPBXListVars(entries,x);
		tPBXList();
	}

	return(0);

}//tPBXCommands()


void tPBX(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttPBXSelectRow();
		else
			ExttPBXSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetPBX();
				unxsSPS("New tPBX table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tPBX WHERE uPBX=%u"
						,uPBX);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			
		sscanf(field[0],"%u",&uPBX);
		sprintf(cHostname,"%.40s",field[1]);
		sscanf(field[2],"%u",&uServer);
		cComment=field[3];
		sscanf(field[4],"%u",&uOwner);
		sscanf(field[5],"%u",&uCreatedBy);
		sscanf(field[6],"%lu",&uCreatedDate);
		sscanf(field[7],"%u",&uModBy);
		sscanf(field[8],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: PBX Gateways",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tPBXTools>");
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

        ExttPBXButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tPBX Record Data",100);

	if(guMode==2000 || guMode==2002)
		tPBXInput(1);
	else
		tPBXInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttPBXAuxTable();

	Footer_ism3();

}//end of tPBX();


void tPBXInput(unsigned uMode)
{

	
	//uPBX uRADType=1001
	OpenRow(LANG_FL_tPBX_uPBX,"black");
	printf("<input title='%s' type=text name=uTable value=%u size=16 maxlength=10 "
		,LANG_FT_tPBX_uPBX,uPBX);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uTable value=%u >\n",uPBX);
	}
	//cHostname uRADType=253
	OpenRow(LANG_FL_tPBX_cHostname,"black");
	printf("<input title='%s' type=text name=uTable value=%s size=40 maxlength=63 "
		,LANG_FT_tPBX_cHostname,EncodeDoubleQuotes(cHostname));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uTable value='%s'>\n",EncodeDoubleQuotes(cHostname));
	}
	//uServer COLTYPE_SELECTTABLE
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tServer;cuServerPullDown","cLabel","cLabel",uServer,1);
	else
		tTablePullDown("tServer;cuServerPullDown","cLabel","cLabel",uServer,0);
	//cComment COLTYPE_TEXT
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cComment "
,LANG_FT_tPBX_cComment);
	if(guPermLevel>=10 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cComment);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cComment);
		printf("<input type=hidden name=cComment value=\"%s\" >\n",EncodeDoubleQuotes(cComment));
	}
	//uOwner COLTYPE_FOREIGNKEY
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy COLTYPE_FOREIGNKEY
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
	//uModBy COLTYPE_FOREIGNKEY
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);


}//void tPBXInput(unsigned uMode)


void NewtPBX(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uPBX FROM tPBX WHERE uPBX=%u",uPBX);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		tPBX(LANG_NBR_RECEXISTS);

	Insert_tPBX();
	uPBX=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tPBX",uPBX);
	unxsSPSLog(uPBX,"tPBX","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uPBX);
		tPBX(gcQuery);
	}

}//NewtPBX(unsigned uMode)


void DeletetPBX(void)
{
	sprintf(gcQuery,"DELETE FROM tPBX WHERE uPBX=%u AND ( uOwner=%u OR %u>9 )"
					,uPBX,guLoginClient,guPermLevel);
	macro_mySQLQueryHTMLError;
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsSPSLog(uPBX,"tPBX","Del");
		tPBX(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsSPSLog(uPBX,"tPBX","DelError");
		tPBX(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetPBX(void)


void Insert_tPBX(void)
{
	sprintf(gcQuery,"INSERT INTO tPBX SET"
		"uPBX=%u,"
		"cHostname='%s',"
		"uServer=%u,"
		"cComment='%s',"
		"uOwner=%u,"
		"uCreatedBy=%u,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,uPBX
			,TextAreaSave(cHostname)
			,uServer
			,TextAreaSave(cComment)
			,uOwner
			,uCreatedBy
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tPBX(void)


void Update_tPBX(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tPBX SET"
		"uPBX=%u,"
		"cHostname='%s',"
		"uServer=%u,"
		"cComment='%s',"
		"uOwner=%u,"
		"uModBy=%u,"
		"uModDate=UNIX_TIMESTAMP(NOW())"
			,uPBX
			,TextAreaSave(cHostname)
			,uServer
			,TextAreaSave(cComment)
			,uOwner
			,uModBy
		);

	macro_mySQLQueryHTMLError;

}//void Update_tPBX(void)


void ModtPBX(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tPBX.uPBX,"
				" tPBX.uModDate"
				" FROM tPBX,tClient"
				" WHERE tPBX.uPBX=%u"
				" AND tPBX.uOwner=tClient.uClient"
				" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uPBX,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uPBX,uModDate FROM tPBX"
				" WHERE uPBX=%u"
					,uPBX);

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) tPBX(LANG_NBR_RECNOTEXIST);
	if(i>1) tPBX(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tPBX(LANG_NBR_EXTMOD);

	Update_tPBX(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tPBX",uPBX);
	unxsSPSLog(uPBX,"tPBX","Mod");
	tPBX(gcQuery);

}//ModtPBX(void)


void tPBXList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttPBXListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tPBXList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttPBXListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uPBX"
		"<td><font face=arial,helvetica color=white>cHostname"
		"<td><font face=arial,helvetica color=white>uServer"
		"<td><font face=arial,helvetica color=white>cComment"
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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tServer","cLabel",strtoul(field[2],NULL,10))
			,field[3]
			,ForeignKey("tClient","cLabel",strtoul(field[4],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[5],NULL,10))
			,cBuf6
			,ForeignKey("tClient","cLabel",strtoul(field[7],NULL,10))
			,cBuf8
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tPBXList()


void CreatetPBX(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tPBX ("
		"uPBX INT UNSIGNED NOT NULL DEFAULT 0 PRIMARY KEY AUTO INCREMENT,"
		"cHostname VARCHAR(64) NOT NULL DEFAULT '',"
		"uServer INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uServer),"
		"cComment INT UNSIGNED NOT NULL DEFAULT '',"
		"uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner),"
		"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetPBX(void)


