/*
FILE
	tPBX source code of unxsSPS.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id: tpbx.c 1922 2012-04-30 14:46:24Z Dylan $
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tpbxfunc.h while 
	RAD is still to be used.
AUTHOR/LEGAL
	(C) 2001-2012 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uPBX: Primary Key
static unsigned uPBX=0;
//cLabel: Short label
static char cLabel[33]={""};
//uAvailable: PBX is available for use
static unsigned uAvailable=0;
static char cYesNouAvailable[32]={""};
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
//uDatacenter: Belongs to this Datacenter
static unsigned uDatacenter=0;
static char cuDatacenterPullDown[256]={""};

//cComment
static char *cComment={""};

//Extensions for searching
static char cPBXSearch[16]={""};
static unsigned uAvailableSearch=0;
static char cYesNouAvailableSearch[8]={""};
static unsigned uServerSearch=0;
static char cuServerSearchPullDown[256]={""};
static unsigned uServerSearchNot=0;
static unsigned uPBXv4Exclude=0;
static unsigned uOwnerSearch=0;
static unsigned uDatacenterSearch=0;
static char cuDatacenterSearchPullDown[256]={""};
int ReadYesNoPullDownTriState(const char *cLabel);
void YesNoPullDownTriState(char *cFieldName, unsigned uSelect, unsigned uMode);

#define VAR_LIST_tPBX "tPBX.uPBX,tPBX.cLabel,tPBX.uAvailable,tPBX.uOwner,tPBX.uCreatedBy,tPBX.uCreatedDate,tPBX.uModBy,tPBX.uModDate,tPBX.uDatacenter,tPBX.cComment"

 //Local only
void tPBXSearchSet(unsigned uStep);
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
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uAvailable"))
			sscanf(entries[i].val,"%u",&uAvailable);
		else if(!strcmp(entries[i].name,"cYesNouAvailable"))
		{
			sprintf(cYesNouAvailable,"%.31s",entries[i].val);
			uAvailable=ReadYesNoPullDown(cYesNouAvailable);
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
		else if(!strcmp(entries[i].name,"uDatacenter"))
			sscanf(entries[i].val,"%u",&uDatacenter);
		else if(!strcmp(entries[i].name,"cComment"))
			cComment=entries[i].val;
		else if(!strcmp(entries[i].name,"cuDatacenterPullDown"))
		{
			sprintf(cuDatacenterPullDown,"%.255s",entries[i].val);
			uDatacenter=ReadPullDown("tDatacenter","cLabel",cuDatacenterPullDown);
		}
		else if(!strcmp(entries[i].name,"uOwnerSearch"))
			sscanf(entries[i].val,"%u",&uOwnerSearch);
		else if(!strcmp(entries[i].name,"cForClientPullDown"))
		{
			sprintf(cForClientPullDown,"%.255s",entries[i].val);
			uOwnerSearch=ReadPullDown("tClient","cLabel",cForClientPullDown);
		}
		else if(!strcmp(entries[i].name,"uDatacenterSearch"))
			sscanf(entries[i].val,"%u",&uDatacenterSearch);
		else if(!strcmp(entries[i].name,"cuDatacenterSearchPullDown"))
		{
			sprintf(cuDatacenterSearchPullDown,"%.255s",entries[i].val);
			uDatacenterSearch=ReadPullDown("tDatacenter","cLabel",cuDatacenterSearchPullDown);
		}
		else if(!strcmp(entries[i].name,"uServerSearch"))
			sscanf(entries[i].val,"%u",&uServerSearch);
		else if(!strcmp(entries[i].name,"cuServerSearchPullDown"))
		{
			sprintf(cuServerSearchPullDown,"%.255s",entries[i].val);
			uServerSearch=ReadPullDown("tServer","cLabel",cuServerSearchPullDown);
		}
		else if(!strcmp(entries[i].name,"cYesNouAvailableSearch"))
		{
			sprintf(cYesNouAvailableSearch,"%.8s",entries[i].val);
			uAvailableSearch=ReadYesNoPullDownTriState(cYesNouAvailableSearch);
		}
		else if(!strcmp(entries[i].name,"uServerSearchNotNoCA"))
			uServerSearchNot=1;
		else if(!strcmp(entries[i].name,"uPBXv4ExcludeNoCA"))
			uPBXv4Exclude=1;

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
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uPBX);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uAvailable);
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uCreatedBy);
		sscanf(field[5],"%lu",&uCreatedDate);
		sscanf(field[6],"%u",&uModBy);
		sscanf(field[7],"%lu",&uModDate);
		sscanf(field[8],"%u",&uDatacenter);
		cComment=field[9];

		}

	}//Internal Skip

	Header_ism3(":: PBXs used and reserved for use",2);//checkbox js = 2
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

	//Custom right panel for creating search sets
	if(guMode==12001)
		tPBXSearchSet(1);
	else if(guMode==12002)
		tPBXSearchSet(2);
	else if(guMode==2000 || guMode==2002)
		tPBXInput(1);
	else if(1)
		tPBXInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttPBXAuxTable();

	Footer_ism3();

}//end of tPBX();


void tPBXSearchSet(unsigned uStep)
{
	OpenRow("<u>Set search parameters</u>","black");

	OpenRow("PBX pattern","black");
	printf("<input title='SQL search pattern %% and _ allowed' type=text name=cPBXSearch"
			" value=\"%s\" size=40 maxlength=15 >",cPBXSearch);
	printf("<input title='Exclude 10/8, 172.16/12 and 192.168/16 DIDs' type=checkbox name=uPBXv4ExcludeNoCA ");
	if(uPBXv4Exclude)
		printf(" checked");
	printf("> Exclude RFC1918 DIDs");

	OpenRow("Datacenter","black");
	tTablePullDown("tDatacenter;cuDatacenterSearchPullDown","cLabel","cLabel",uDatacenterSearch,1);

	OpenRow("Node","black");
	tTablePullDown("tServer;cuServerSearchPullDown","cLabel","cLabel",uServerSearch,1);
	printf("<input title='Logical NOT of selected server if any. Including default any server (no server)' type=checkbox name=uServerSearchNotNoCA ");
	if(uServerSearchNot)
		printf(" checked");
	printf("> Not");

	OpenRow("Owner","black");
	tTablePullDownResellers(uOwnerSearch,0);

	OpenRow("Available","black");
	YesNoPullDownTriState("uAvailableSearch",uAvailableSearch,1);

	if(uStep==1)
	{
		;
	}
	else if(uStep==2)
	{
		;
	}

}//void tPBXSearchSet(unsigned uStep)


void tPBXInput(unsigned uMode)
{

//uPBX
	OpenRow(LANG_FL_tPBX_uPBX,"black");
	printf("<input title='%s' type=text name=uPBX value=%u size=16 maxlength=10 "
,LANG_FT_tPBX_uPBX,uPBX);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uPBX value=%u >\n",uPBX);
	}
//cLabel
	OpenRow(LANG_FL_tPBX_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tPBX_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tPBX_uAvailable,"black");
	if(guPermLevel>=10 && uMode)
		YesNoPullDown("uAvailable",uAvailable,1);
	else
		YesNoPullDown("uAvailable",uAvailable,0);
//uDatacenter
	OpenRow(LANG_FL_tDatacenter_uDatacenter,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDownOwner("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,1);
	else
		tTablePullDownOwner("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,0);
//cComment
	OpenRow("cComment","black");
	printf("<textarea title='Additional information about DID use' cols=80 wrap=hard rows=4 name=cComment ");
	if(guPermLevel>=7 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",TransformAngleBrackets(cComment));
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",TransformAngleBrackets(cComment));
		printf("<input type=hidden name=cComment value=\"%s\" >\n",EncodeDoubleQuotes(cComment));
	}
//uOwner
	OpenRow(LANG_FL_tPBX_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tPBX_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tPBX_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tPBX_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tPBX_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tPBXInput(unsigned uMode)


void NewtPBX(unsigned uMode)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uPBX FROM tPBX WHERE uPBX=%u",uPBX);
	MYSQL_RUN_STORE(res);
	if(mysql_num_rows(res)) 
		//tPBX("<blink>Record already exists");
		tPBX(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tPBX();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
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
	sprintf(gcQuery,"DELETE FROM tPBX WHERE uPBX=%u AND ( uOwner=%u OR %u>9 )",uPBX,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tPBX("Record Deleted");
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
	sprintf(gcQuery,"INSERT INTO tPBX SET uPBX=%u,cLabel='%s',uAvailable=%u,uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW()),uDatacenter=%u,cComment='%s'",
			uPBX
			,TextAreaSave(cLabel)
			,uAvailable
			,uOwner
			,uCreatedBy
			,uDatacenter
			,cComment);
	MYSQL_RUN;

}//void Insert_tPBX(void)


void Update_tPBX(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tPBX SET uPBX=%u,cLabel='%s',uAvailable=%u,uModBy=%u,"
				"uModDate=UNIX_TIMESTAMP(NOW()),uDatacenter=%u,cComment='%s' WHERE _rowid=%s",
			uPBX
			,TextAreaSave(cLabel)
			,uAvailable
			,uModBy
			,uDatacenter
			,cComment
			,cRowid);
	MYSQL_RUN;

}//void Update_tPBX(void)


void ModtPBX(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT tPBX.uPBX,tPBX.uModDate FROM tPBX,tClient WHERE tPBX.uPBX=%u"
				" AND tPBX.uOwner=tClient.uClient AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uPBX,guLoginClient,guLoginClient);
	else
		sprintf(gcQuery,"SELECT uPBX,uModDate FROM tPBX WHERE uPBX=%u",uPBX);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tPBX("<blink>Record does not exist");
	if(i<1) tPBX(LANG_NBR_RECNOTEXIST);
	//if(i>1) tPBX("<blink>Multpbxle rows!");
	if(i>1) tPBX(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tPBX(LANG_NBR_EXTMOD);

	Update_tPBX(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
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

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tPBXList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttPBXListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uPBX"
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>uAvailable"
		"<td><font face=arial,helvetica color=white>uDatacenter"
		"<td><font face=arial,helvetica color=white>cComment"
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
		long unsigned luYesNo2=strtoul(field[2],NULL,10);
		char cBuf2[4];
		if(luYesNo2)
			sprintf(cBuf2,"Yes");
		else
			sprintf(cBuf2,"No");
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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,cBuf2
			,ForeignKey("tDatacenter","cLabel",strtoul(field[8],NULL,10))
			,field[9]
			,ForeignKey("tClient","cLabel",strtoul(field[3],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[4],NULL,10))
			,cBuf5
			,ForeignKey("tClient","cLabel",strtoul(field[6],NULL,10))
			,cBuf7);
	}

	printf("</table></form>\n");
	Footer_ism3();

}//tPBXList()


void CreatetPBX(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tPBX ( "
			"uPBX INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
			"cComment VARCHAR(255) NOT NULL DEFAULT '',"
			"uOwner INT UNSIGNED NOT NULL DEFAULT 0,INDEX (uOwner),"
			"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uAvailable INT UNSIGNED NOT NULL DEFAULT 0,"
			"uDatacenter INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetPBX()

