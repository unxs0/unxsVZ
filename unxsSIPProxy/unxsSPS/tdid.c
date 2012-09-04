/*
FILE
	tDID source code of unxsSPS.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id: tdid.c 1922 2012-04-30 14:46:24Z Dylan $
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tdidfunc.h while 
	RAD is still to be used.
AUTHOR/LEGAL
	(C) 2001-2012 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uDID: Primary Key
static unsigned uDID=0;
//cLabel: Short label
static char cLabel[33]={""};
//uAvailable: DID is available for use
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
static char cDIDv4Search[16]={""};
static unsigned uAvailableSearch=0;
static char cYesNouAvailableSearch[8]={""};
static unsigned uServerSearch=0;
static char cuServerSearchPullDown[256]={""};
static unsigned uServerSearchNot=0;
static unsigned uDIDv4Exclude=0;
static unsigned uOwnerSearch=0;
static unsigned uDatacenterSearch=0;
static char cuDatacenterSearchPullDown[256]={""};
int ReadYesNoPullDownTriState(const char *cLabel);
void YesNoPullDownTriState(char *cFieldName, unsigned uSelect, unsigned uMode);

#define VAR_LIST_tDID "tDID.uDID,tDID.cLabel,tDID.uAvailable,tDID.uOwner,tDID.uCreatedBy,tDID.uCreatedDate,tDID.uModBy,tDID.uModDate,tDID.uDatacenter,tDID.cComment"

 //Local only
void tDIDSearchSet(unsigned uStep);
void Insert_tDID(void);
void Update_tDID(char *cRowid);
void ProcesstDIDListVars(pentry entries[], int x);

 //In tDIDfunc.h file included below
void ExtProcesstDIDVars(pentry entries[], int x);
void ExttDIDCommands(pentry entries[], int x);
void ExttDIDButtons(void);
void ExttDIDNavBar(void);
void ExttDIDGetHook(entry gentries[], int x);
void ExttDIDSelect(void);
void ExttDIDSelectRow(void);
void ExttDIDListSelect(void);
void ExttDIDListFilter(void);
void ExttDIDAuxTable(void);

#include "tdidfunc.h"

 //Table Variables Assignment Function
void ProcesstDIDVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uDID"))
			sscanf(entries[i].val,"%u",&uDID);
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
		else if(!strcmp(entries[i].name,"cDIDv4Search"))
			sprintf(cDIDv4Search,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"cYesNouAvailableSearch"))
		{
			sprintf(cYesNouAvailableSearch,"%.8s",entries[i].val);
			uAvailableSearch=ReadYesNoPullDownTriState(cYesNouAvailableSearch);
		}
		else if(!strcmp(entries[i].name,"uServerSearchNotNoCA"))
			uServerSearchNot=1;
		else if(!strcmp(entries[i].name,"uDIDv4ExcludeNoCA"))
			uDIDv4Exclude=1;

	}

	//After so we can overwrite form data if needed.
	ExtProcesstDIDVars(entries,x);

}//ProcesstDIDVars()


void ProcesstDIDListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uDID);
                        guMode=2002;
                        tDID("");
                }
        }
}//void ProcesstDIDListVars(pentry entries[], int x)


int tDIDCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttDIDCommands(entries,x);

	if(!strcmp(gcFunction,"tDIDTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tDIDList();
		}

		//Default
		ProcesstDIDVars(entries,x);
		tDID("");
	}
	else if(!strcmp(gcFunction,"tDIDList"))
	{
		ProcessControlVars(entries,x);
		ProcesstDIDListVars(entries,x);
		tDIDList();
	}

	return(0);

}//tDIDCommands()


void tDID(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttDIDSelectRow();
		else
			ExttDIDSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetDID();
				unxsSPS("New tDID table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tDID WHERE uDID=%u"
						,uDID);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uDID);
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

	Header_ism3(":: DIDs used and reserved for use",2);//checkbox js = 2
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tDIDTools>");
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

        ExttDIDButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tDID Record Data",100);

	//Custom right panel for creating search sets
	if(guMode==12001)
		tDIDSearchSet(1);
	else if(guMode==12002)
		tDIDSearchSet(2);
	else if(guMode==2000 || guMode==2002)
		tDIDInput(1);
	else if(1)
		tDIDInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttDIDAuxTable();

	Footer_ism3();

}//end of tDID();


void tDIDSearchSet(unsigned uStep)
{
	OpenRow("<u>Set search parameters</u>","black");

	OpenRow("DIDv4 pattern","black");
	printf("<input title='SQL search pattern %% and _ allowed' type=text name=cDIDv4Search"
			" value=\"%s\" size=40 maxlength=15 >",cDIDv4Search);
	printf("<input title='Exclude 10/8, 172.16/12 and 192.168/16 DIDs' type=checkbox name=uDIDv4ExcludeNoCA ");
	if(uDIDv4Exclude)
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

}//void tDIDSearchSet(unsigned uStep)


void tDIDInput(unsigned uMode)
{

//uDID
	OpenRow(LANG_FL_tDID_uDID,"black");
	printf("<input title='%s' type=text name=uDID value=%u size=16 maxlength=10 "
,LANG_FT_tDID_uDID,uDID);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uDID value=%u >\n",uDID);
	}
//cLabel
	OpenRow(LANG_FL_tDID_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tDID_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tDID_uAvailable,"black");
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
	OpenRow(LANG_FL_tDID_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tDID_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tDID_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tDID_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tDID_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tDIDInput(unsigned uMode)


void NewtDID(unsigned uMode)
{
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uDID FROM tDID WHERE uDID=%u",uDID);
	MYSQL_RUN_STORE(res);
	if(mysql_num_rows(res)) 
		//tDID("<blink>Record already exists");
		tDID(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tDID();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uDID=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tDID",uDID);
	unxsSPSLog(uDID,"tDID","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uDID);
		tDID(gcQuery);
	}

}//NewtDID(unsigned uMode)


void DeletetDID(void)
{
	sprintf(gcQuery,"DELETE FROM tDID WHERE uDID=%u AND ( uOwner=%u OR %u>9 )",uDID,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tDID("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsSPSLog(uDID,"tDID","Del");
		tDID(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsSPSLog(uDID,"tDID","DelError");
		tDID(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetDID(void)


void Insert_tDID(void)
{
	sprintf(gcQuery,"INSERT INTO tDID SET uDID=%u,cLabel='%s',uAvailable=%u,uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW()),uDatacenter=%u,cComment='%s'",
			uDID
			,TextAreaSave(cLabel)
			,uAvailable
			,uOwner
			,uCreatedBy
			,uDatacenter
			,cComment);
	MYSQL_RUN;

}//void Insert_tDID(void)


void Update_tDID(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tDID SET uDID=%u,cLabel='%s',uAvailable=%u,uModBy=%u,"
				"uModDate=UNIX_TIMESTAMP(NOW()),uDatacenter=%u,cComment='%s' WHERE _rowid=%s",
			uDID
			,TextAreaSave(cLabel)
			,uAvailable
			,uModBy
			,uDatacenter
			,cComment
			,cRowid);
	MYSQL_RUN;

}//void Update_tDID(void)


void ModtDID(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT tDID.uDID,tDID.uModDate FROM tDID,tClient WHERE tDID.uDID=%u"
				" AND tDID.uOwner=tClient.uClient AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uDID,guLoginClient,guLoginClient);
	else
		sprintf(gcQuery,"SELECT uDID,uModDate FROM tDID WHERE uDID=%u",uDID);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tDID("<blink>Record does not exist");
	if(i<1) tDID(LANG_NBR_RECNOTEXIST);
	//if(i>1) tDID("<blink>Multdidle rows!");
	if(i>1) tDID(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tDID(LANG_NBR_EXTMOD);

	Update_tDID(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tDID",uDID);
	unxsSPSLog(uDID,"tDID","Mod");
	tDID(gcQuery);

}//ModtDID(void)


void tDIDList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttDIDListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tDIDList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttDIDListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uDID"
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

}//tDIDList()


void CreatetDID(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tDID ( "
			"uDID INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
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
}//CreatetDID()

