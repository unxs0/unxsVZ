/*
FILE
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in tgatewayfunc.h
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
static unsigned uGateway=0;
static char cLabel[33]={""};
static char cHostname[33]={""};
static unsigned uGatewayType=0;
static char cuGatewayTypePullDown[256]={""};
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


#define VAR_LIST_tGateway "tGateway.uGateway,tGateway.cLabel,tGateway.cHostname,tGateway.uGatewayType,tGateway.uCarrier,tGateway.uCluster,tGateway.cComment,tGateway.uOwner,tGateway.uCreatedBy,tGateway.uCreatedDate,tGateway.uModBy,tGateway.uModDate"

 //Local only
void Insert_tGateway(void);
void Update_tGateway(char *cRowid);
void ProcesstGatewayListVars(pentry entries[], int x);

 //In tGatewayfunc.h file included below
void ExtProcesstGatewayVars(pentry entries[], int x);
void ExttGatewayCommands(pentry entries[], int x);
void ExttGatewayButtons(void);
void ExttGatewayNavBar(void);
void ExttGatewayGetHook(entry gentries[], int x);
void ExttGatewaySelect(void);
void ExttGatewaySelectRow(void);
void ExttGatewayListSelect(void);
void ExttGatewayListFilter(void);
void ExttGatewayAuxTable(void);

#include "tgatewayfunc.h"

 //Table Variables Assignment Function
void ProcesstGatewayVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		
		if(!strcmp(entries[i].name,"uGateway"))
			sscanf(entries[i].val,"%u",&uGateway);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"cHostname"))
			sprintf(cHostname,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"uGatewayType"))
			sscanf(entries[i].val,"%u",&uGatewayType);
		else if(!strcmp(entries[i].name,"cuGatewayTypePullDown"))
		{
			sprintf(cuGatewayTypePullDown,"%.255s",entries[i].val);
			uGatewayType=ReadPullDown("tGatewayType","cLabel",cuGatewayTypePullDown);
		}
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
	ExtProcesstGatewayVars(entries,x);

}//ProcesstGatewayVars()


void ProcesstGatewayListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uGateway);
                        guMode=2002;
                        tGateway("");
                }
        }
}//void ProcesstGatewayListVars(pentry entries[], int x)


int tGatewayCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttGatewayCommands(entries,x);

	if(!strcmp(gcFunction,"tGatewayTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tGatewayList();
		}

		//Default
		ProcesstGatewayVars(entries,x);
		tGateway("");
	}
	else if(!strcmp(gcFunction,"tGatewayList"))
	{
		ProcessControlVars(entries,x);
		ProcesstGatewayListVars(entries,x);
		tGatewayList();
	}

	return(0);

}//tGatewayCommands()


void tGateway(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttGatewaySelectRow();
		else
			ExttGatewaySelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetGateway();
				unxsSPS("New tGateway table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tGateway WHERE uGateway=%u"
						,uGateway);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			
		sscanf(field[0],"%u",&uGateway);
		sprintf(cLabel,"%.32s",field[1]);
		sprintf(cHostname,"%.32s",field[2]);
		sscanf(field[3],"%u",&uGatewayType);
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

	Header_ism3(":: Outgoing termination provider g",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tGatewayTools>");
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

        ExttGatewayButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tGateway Record Data",100);

	if(guMode==2000 || guMode==2002)
		tGatewayInput(1);
	else
		tGatewayInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttGatewayAuxTable();

	Footer_ism3();

}//end of tGateway();


void tGatewayInput(unsigned uMode)
{

	
	//uGateway uRADType=1001
	OpenRow(LANG_FL_tGateway_uGateway,"black");
	printf("<input title='%s' type=text name=uGateway id=uGateway value='%u' size=16 maxlength=10 "
		,LANG_FT_tGateway_uGateway,uGateway);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uGateway id=uGateway value='%u' >\n",uGateway);
	}
	//cLabel uRADType=253
	OpenRow(LANG_FL_tGateway_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel id=cLabel value='%s' size=40 maxlength=32 "
		,LANG_FT_tGateway_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel id=cLabel value='%s'>\n",EncodeDoubleQuotes(cLabel));
	}
	//cHostname uRADType=253
	OpenRow(LANG_FL_tGateway_cHostname,"black");
	printf("<input title='%s' type=text name=cHostname id=cHostname value='%s' size=40 maxlength=32 "
		,LANG_FT_tGateway_cHostname,EncodeDoubleQuotes(cHostname));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cHostname id=cHostname value='%s'>\n",EncodeDoubleQuotes(cHostname));
	}
	//uGatewayType COLTYPE_SELECTTABLE
	OpenRow(LANG_FL_tGateway_uGatewayType,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tGatewayType;cuGatewayTypePullDown","cLabel","cLabel",uGatewayType,1);
	else
		tTablePullDown("tGatewayType;cuGatewayTypePullDown","cLabel","cLabel",uGatewayType,0);
	//uCarrier COLTYPE_SELECTTABLE
	OpenRow(LANG_FL_tGateway_uCarrier,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tCarrier;cuCarrierPullDown","cLabel","cLabel",uCarrier,1);
	else
		tTablePullDown("tCarrier;cuCarrierPullDown","cLabel","cLabel",uCarrier,0);
	//uCluster COLTYPE_SELECTTABLE
	OpenRow(LANG_FL_tGateway_uCluster,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tCluster;cuClusterPullDown","cLabel","cLabel",uCluster,1);
	else
		tTablePullDown("tCluster;cuClusterPullDown","cLabel","cLabel",uCluster,0);
	//cComment uRADType=253
	OpenRow(LANG_FL_tGateway_cComment,"black");
	printf("<input title='%s' type=text name=cComment id=cComment value='%s' size=40 maxlength=31 "
		,LANG_FT_tGateway_cComment,EncodeDoubleQuotes(cComment));
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
	OpenRow(LANG_FL_tGateway_uOwner,"black");
	printf("%s<input type=hidden name=uOwner id=uOwner value='%u' >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tGateway_uCreatedBy,"black");
	printf("%s<input type=hidden name=uCreatedBy id=uCreatedBy value='%u' >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tGateway_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate id=uCreatedDate value='%lu' >\n",uCreatedDate);
	//uModBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tGateway_uModBy,"black");
	printf("%s<input type=hidden name=uModBy id=uModBy value='%u' >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tGateway_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate id=uModDate value='%lu' >\n",uModDate);
	printf("</tr>\n");

}//void tGatewayInput(unsigned uMode)


void NewtGateway(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uGateway FROM tGateway WHERE uGateway=%u",uGateway);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		tGateway(LANG_NBR_RECEXISTS);

	Insert_tGateway();
	uGateway=mysql_insert_id(&gMysql);
#ifdef StandardFields
	uCreatedDate=luGetCreatedDate("tGateway",uGateway);
#endif
	unxsSPSLog(uGateway,"tGateway","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uGateway);
		tGateway(gcQuery);
	}

}//NewtGateway(unsigned uMode)


void DeletetGateway(void)
{
#ifdef StandardFields
	sprintf(gcQuery,"DELETE FROM tGateway WHERE uGateway=%u AND ( uOwner=%u OR %u>9 )"
					,uGateway,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tGateway WHERE uGateway=%u AND %u>9 )"
					,uGateway,guPermLevel);
#endif
	macro_mySQLQueryHTMLError;
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsSPSLog(uGateway,"tGateway","Del");
		tGateway(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsSPSLog(uGateway,"tGateway","DelError");
		tGateway(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetGateway(void)


void Insert_tGateway(void)
{
	sprintf(gcQuery,"INSERT INTO tGateway SET "
		"cLabel='%s',"
		"cHostname='%s',"
		"uGatewayType=%u,"
		"uCarrier=%u,"
		"uCluster=%u,"
		"cComment='%s',"
		"uOwner=%u,"
		"uCreatedBy=%u,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,TextAreaSave(cLabel)
			,TextAreaSave(cHostname)
			,uGatewayType
			,uCarrier
			,uCluster
			,TextAreaSave(cComment)
			,uOwner
			,uCreatedBy
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tGateway(void)


void Update_tGateway(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tGateway SET "
		"cLabel='%s',"
		"cHostname='%s',"
		"uGatewayType=%u,"
		"uCarrier=%u,"
		"uCluster=%u,"
		"cComment='%s',"
		"uOwner=%u,"
		"uModBy=%u,"
		"uModDate=UNIX_TIMESTAMP(NOW())"
		" WHERE _rowid=%s"
			,TextAreaSave(cLabel)
			,TextAreaSave(cHostname)
			,uGatewayType
			,uCarrier
			,uCluster
			,TextAreaSave(cComment)
			,uOwner
			,uModBy
			,cRowid
		);

	macro_mySQLQueryHTMLError;

}//void Update_tGateway(void)


void ModtGateway(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

#ifdef StandardFields
	unsigned uPreModDate=0;
	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tGateway.uGateway,"
				" tGateway.uModDate"
				" FROM tGateway,tClient"
				" WHERE tGateway.uGateway=%u"
				" AND tGateway.uOwner=tClient.uClient"
				" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uGateway,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uGateway,uModDate FROM tGateway"
				" WHERE uGateway=%u"
					,uGateway);
#else
	sprintf(gcQuery,"SELECT uGateway FROM tGateway"
				" WHERE uGateway=%u"
					,uGateway);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) tGateway(LANG_NBR_RECNOTEXIST);
	if(i>1) tGateway(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef StandardFields
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tGateway(LANG_NBR_EXTMOD);
#endif

	Update_tGateway(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef StandardFields
	uModDate=luGetModDate("tGateway",uGateway);
#endif
	unxsSPSLog(uGateway,"tGateway","Mod");
	tGateway(gcQuery);

}//ModtGateway(void)


void tGatewayList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttGatewayListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tGatewayList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttGatewayListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uGateway"
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>cHostname"
		"<td><font face=arial,helvetica color=white>uGatewayType"
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
		printf("<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tGateway&uGateway=%s>%s</a><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,ForeignKey("tGatewayType","cLabel",strtoul(field[3],NULL,10))
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

}//tGatewayList()


void CreatetGateway(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tGateway ("
		"uGateway INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
		"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
		"cHostname VARCHAR(32) NOT NULL DEFAULT '', INDEX (cHostname),"
		"uGatewayType INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uGatewayType),"
		"uCarrier INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uCarrier),"
		"uCluster INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uCluster),"
		"cComment VARCHAR(32) NOT NULL DEFAULT '',"
		"uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetGateway(void)


