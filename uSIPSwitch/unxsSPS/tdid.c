/*
FILE
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in tdidfunc.h
	while unxsSPS can still to be used to change this schema dependent file.
AUTHOR
	(C) 2001-2012 Gary Wallis for Unixservice, LLC.
TEMPLATE VARS AND FUNCTIONS
	ModuleCreateQuery
	ModuleInsertQuery
	ModuleListPrint
	ModuleListTable
	ModuleLoadVars
	ModuleProcVars
	ModuleInput
	ModuleUpdateQuery
	ModuleVars
	ModuleVarList
	cProject
	cTableKey
	cTableName
	cTableNameLC
	cTableTitle
*/


#include "mysqlrad.h"

//Table Variables
static unsigned uDID=0;
static char cLabel[33]={""};
static char cDID[33]={""};
static unsigned uPBX=0;
static unsigned uCarrier=0;
static char cuCarrierPullDown[256]={""};
static unsigned uCluster=0;
static char cuClusterPullDown[256]={""};
static char cComment[33]={""};
static unsigned uOwner=0;
#define StandardFields
static unsigned uCreatedBy=0;
static time_t uCreatedDate=0;
static unsigned uModBy=0;
static time_t uModDate=0;


#define VAR_LIST_tDID "tDID.uDID,tDID.cLabel,tDID.cDID,tDID.uPBX,tDID.uCarrier,tDID.uCluster,tDID.cComment,tDID.uOwner,tDID.uCreatedBy,tDID.uCreatedDate,tDID.uModBy,tDID.uModDate"

 //Local only
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
			sprintf(cLabel,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"cDID"))
			sprintf(cDID,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"uPBX"))
			sscanf(entries[i].val,"%u",&uPBX);
		else if(!strcmp(entries[i].name,"uCarrier"))
			sscanf(entries[i].val,"%u",&uCarrier);
		else if(!strcmp(entries[i].name,"cuCarrierPullDown"))
		{
			sprintf(cuCarrierPullDown,"%.255s",entries[i].val);
			uCarrier=ReadPullDown("tCarrier","cLabel",cuCarrierPullDown);
		}
		else if(!strcmp(entries[i].name,"uCluster"))
			sscanf(entries[i].val,"%u",&uCluster);
		else if(!strcmp(entries[i].name,"cuClusterPullDown"))
		{
			sprintf(cuClusterPullDown,"%.255s",entries[i].val);
			uCluster=ReadPullDown("tCluster","cLabel",cuClusterPullDown);
		}
		else if(!strcmp(entries[i].name,"cComment"))
			sprintf(cComment,"%.40s",entries[i].val);
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
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			
		sscanf(field[0],"%u",&uDID);
		sprintf(cLabel,"%.32s",field[1]);
		sprintf(cDID,"%.32s",field[2]);
		sscanf(field[3],"%u",&uPBX);
		sscanf(field[4],"%u",&uCarrier);
		sscanf(field[5],"%u",&uCluster);
		sprintf(cComment,"%.32s",field[6]);
		sscanf(field[7],"%u",&uOwner);
		sscanf(field[8],"%u",&uCreatedBy);
		sscanf(field[9],"%lu",&uCreatedDate);
		sscanf(field[10],"%u",&uModBy);
		sscanf(field[11],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: Direct inward dialing numbers",0);
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

	if(guMode==2000 || guMode==2002)
		tDIDInput(1);
	else
		tDIDInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttDIDAuxTable();

	Footer_ism3();

}//end of tDID();


void tDIDInput(unsigned uMode)
{

	
	//uDID uRADType=1001
	OpenRow(LANG_FL_tDID_uDID,"black");
	printf("<input title='%s' type=text name=uDID id=uDID value='%u' size=16 maxlength=10 "
		,LANG_FT_tDID_uDID,uDID);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uDID id=uDID value='%u' >\n",uDID);
	}
	//cLabel uRADType=253
	OpenRow(LANG_FL_tDID_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel id=cLabel value='%s' size=40 maxlength=32 "
		,LANG_FT_tDID_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel id=cLabel value='%s'>\n",EncodeDoubleQuotes(cLabel));
	}
	//cDID uRADType=253
	OpenRow(LANG_FL_tDID_cDID,"black");
	printf("<input title='%s' type=text name=cDID id=cDID value='%s' size=40 maxlength=31 "
		,LANG_FT_tDID_cDID,EncodeDoubleQuotes(cDID));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cDID id=cDID value='%s'>\n",EncodeDoubleQuotes(cDID));
	}
	//uPBX COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tDID_uPBX,"black");
	if(guPermLevel>=10 && uMode)
		printf("<!--FK AllowMod-->\n<input title='%s' type=text size=16 maxlength=20 name=uPBX id=uPBX value='%u' >\n",LANG_FT_tDID_uPBX,uPBX);
	else
		printf("<input title='%s' type=text value='%s' size=0 disabled><input type=hidden name='uPBX' value='%u' >\n",LANG_FT_tDID_uPBX,ForeignKey("tPBX","cLabel",uPBX),uPBX);
	//uCarrier COLTYPE_SELECTTABLE
	OpenRow(LANG_FL_tDID_uCarrier,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tCarrier;cuCarrierPullDown","cLabel","cLabel",uCarrier,1);
	else
		tTablePullDown("tCarrier;cuCarrierPullDown","cLabel","cLabel",uCarrier,0);
	//uCluster COLTYPE_SELECTTABLE
	OpenRow(LANG_FL_tDID_uCluster,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tCluster;cuClusterPullDown","cLabel","cLabel",uCluster,1);
	else
		tTablePullDown("tCluster;cuClusterPullDown","cLabel","cLabel",uCluster,0);
	//cComment uRADType=253
	OpenRow(LANG_FL_tDID_cComment,"black");
	printf("<input title='%s' type=text name=cComment id=cComment value='%s' size=40 maxlength=31 "
		,LANG_FT_tDID_cComment,EncodeDoubleQuotes(cComment));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cComment id=cComment value='%s'>\n",EncodeDoubleQuotes(cComment));
	}
	//uOwner COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tDID_uOwner,"black");
	printf("%s<input type=hidden name=uOwner id=uOwner value='%u' >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tDID_uCreatedBy,"black");
	printf("%s<input type=hidden name=uCreatedBy id=uCreatedBy value='%u' >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tDID_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate id=uCreatedDate value='%lu' >\n",uCreatedDate);
	//uModBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tDID_uModBy,"black");
	printf("%s<input type=hidden name=uModBy id=uModBy value='%u' >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tDID_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate id=uModDate value='%lu' >\n",uModDate);
	printf("</tr>\n");

}//void tDIDInput(unsigned uMode)


void NewtDID(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uDID FROM tDID WHERE uDID=%u",uDID);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		tDID(LANG_NBR_RECEXISTS);

	Insert_tDID();
	uDID=mysql_insert_id(&gMysql);
#ifdef StandardFields
	uCreatedDate=luGetCreatedDate("tDID",uDID);
#endif
	unxsSPSLog(uDID,"tDID","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uDID);
		tDID(gcQuery);
	}

}//NewtDID(unsigned uMode)


void DeletetDID(void)
{
#ifdef StandardFields
	sprintf(gcQuery,"DELETE FROM tDID WHERE uDID=%u AND ( uOwner=%u OR %u>9 )"
					,uDID,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tDID WHERE uDID=%u AND %u>9 )"
					,uDID,guPermLevel);
#endif
	macro_mySQLQueryHTMLError;
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
	sprintf(gcQuery,"INSERT INTO tDID SET "
		"cLabel='%s',"
		"cDID='%s',"
		"uPBX=%u,"
		"uCarrier=%u,"
		"uCluster=%u,"
		"cComment='%s',"
		"uOwner=%u,"
		"uCreatedBy=%u,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,TextAreaSave(cLabel)
			,TextAreaSave(cDID)
			,uPBX
			,uCarrier
			,uCluster
			,TextAreaSave(cComment)
			,uOwner
			,uCreatedBy
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tDID(void)


void Update_tDID(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tDID SET "
		"cLabel='%s',"
		"cDID='%s',"
		"uPBX=%u,"
		"uCarrier=%u,"
		"uCluster=%u,"
		"cComment='%s',"
		"uOwner=%u,"
		"uModBy=%u,"
		"uModDate=UNIX_TIMESTAMP(NOW())"
		" WHERE _rowid=%s"
			,TextAreaSave(cLabel)
			,TextAreaSave(cDID)
			,uPBX
			,uCarrier
			,uCluster
			,TextAreaSave(cComment)
			,uOwner
			,uModBy
			,cRowid
		);

	macro_mySQLQueryHTMLError;

}//void Update_tDID(void)


void ModtDID(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

#ifdef StandardFields
	unsigned uPreModDate=0;
	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tDID.uDID,"
				" tDID.uModDate"
				" FROM tDID,tClient"
				" WHERE tDID.uDID=%u"
				" AND tDID.uOwner=tClient.uClient"
				" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uDID,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uDID,uModDate FROM tDID"
				" WHERE uDID=%u"
					,uDID);
#else
	sprintf(gcQuery,"SELECT uDID FROM tDID"
				" WHERE uDID=%u"
					,uDID);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) tDID(LANG_NBR_RECNOTEXIST);
	if(i>1) tDID(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef StandardFields
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tDID(LANG_NBR_EXTMOD);
#endif

	Update_tDID(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef StandardFields
	uModDate=luGetModDate("tDID",uDID);
#endif
	unxsSPSLog(uDID,"tDID","Mod");
	tDID(gcQuery);

}//ModtDID(void)


void tDIDList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttDIDListSelect();

	macro_mySQLRunAndStore(res);
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
		"<td><font face=arial,helvetica color=white>cDID"
		"<td><font face=arial,helvetica color=white>uPBX"
		"<td><font face=arial,helvetica color=white>uCarrier"
		"<td><font face=arial,helvetica color=white>uCluster"
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
				time_t luTime9=strtoul(field[9],NULL,10);
		char cBuf9[32];
		if(luTime9)
			ctime_r(&luTime9,cBuf9);
		else
			sprintf(cBuf9,"---");
		time_t luTime11=strtoul(field[11],NULL,10);
		char cBuf11[32];
		if(luTime11)
			ctime_r(&luTime11,cBuf11);
		else
			sprintf(cBuf11,"---");
		printf("<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tDID&uDID=%s>%s</a>"
				"<td>%s"
				"<td>%s"
				"<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tPBX&uPBX=%s>%s</a>"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s"
				"<td>%s</tr>"
			,field[0],field[0]
			,field[1]
			,field[2]
			,field[3],ForeignKey("tPBX","cLabel",strtoul(field[3],NULL,10))
			,ForeignKey("tCarrier","cLabel",strtoul(field[4],NULL,10))
			,ForeignKey("tCluster","cLabel",strtoul(field[5],NULL,10))
			,field[6]
			,ForeignKey("tClient","cLabel",strtoul(field[7],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[8],NULL,10))
			,cBuf9
			,ForeignKey("tClient","cLabel",strtoul(field[10],NULL,10))
			,cBuf11
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tDIDList()


void CreatetDID(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tDID ("
		"uDID INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
		"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
		"cDID VARCHAR(32) NOT NULL DEFAULT '', UNIQUE (cDID),"
		"uPBX INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uPBX),"
		"uCarrier INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uCarrier),"
		"uCluster INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uCluster),"
		"cComment VARCHAR(64) NOT NULL DEFAULT '',"
		"uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetDID(void)


