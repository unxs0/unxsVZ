/*
FILE
	$Id: module.c 2115 2012-09-19 14:11:03Z Gary $
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in tpbxfunc.h
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
static unsigned uPBX=0;
static char cLabel[33]={""};
static char cAddress[65]={""};
static char cHostname[65]={""};
static unsigned uPort=0;
static unsigned uPriority=0;
static unsigned uWeight=0;
static char cAttributes[65]={""};
static char cDescription[65]={""};
static unsigned uStatus=0;
static unsigned uLines=0;
static unsigned uStrip=0;
static char cPrefix[33]={""};
static unsigned uCluster=0;
static char cuClusterPullDown[256]={""};
static char cComment[65]={""};
static unsigned uOwner=0;
#define StandardFields
static unsigned uCreatedBy=0;
static time_t uCreatedDate=0;
static unsigned uModBy=0;
static time_t uModDate=0;


#define VAR_LIST_tPBX "tPBX.uPBX,tPBX.cLabel,tPBX.cAddress,tPBX.cHostname,tPBX.uPort,tPBX.uPriority,tPBX.uWeight,tPBX.cAttributes,tPBX.cDescription,tPBX.uStatus,tPBX.uLines,tPBX.uStrip,tPBX.cPrefix,tPBX.uCluster,tPBX.cComment,tPBX.uOwner,tPBX.uCreatedBy,tPBX.uCreatedDate,tPBX.uModBy,tPBX.uModDate"

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
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"cAddress"))
			sprintf(cAddress,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"cHostname"))
			sprintf(cHostname,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"uPort"))
			sscanf(entries[i].val,"%u",&uPort);
		else if(!strcmp(entries[i].name,"uPriority"))
			sscanf(entries[i].val,"%u",&uPriority);
		else if(!strcmp(entries[i].name,"uWeight"))
			sscanf(entries[i].val,"%u",&uWeight);
		else if(!strcmp(entries[i].name,"cAttributes"))
			sprintf(cAttributes,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"cDescription"))
			sprintf(cDescription,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"uStatus"))
			sscanf(entries[i].val,"%u",&uStatus);
		else if(!strcmp(entries[i].name,"uLines"))
			sscanf(entries[i].val,"%u",&uLines);
		else if(!strcmp(entries[i].name,"uStrip"))
			sscanf(entries[i].val,"%u",&uStrip);
		else if(!strcmp(entries[i].name,"cPrefix"))
			sprintf(cPrefix,"%.40s",entries[i].val);
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
		sprintf(cLabel,"%.32s",field[1]);
		sprintf(cAddress,"%.64s",field[2]);
		sprintf(cHostname,"%.64s",field[3]);
		sscanf(field[4],"%u",&uPort);
		sscanf(field[5],"%u",&uPriority);
		sscanf(field[6],"%u",&uWeight);
		sprintf(cAttributes,"%.64s",field[7]);
		sprintf(cDescription,"%.64s",field[8]);
		sscanf(field[9],"%u",&uStatus);
		sscanf(field[10],"%u",&uLines);
		sscanf(field[11],"%u",&uStrip);
		sprintf(cPrefix,"%.32s",field[12]);
		sscanf(field[13],"%u",&uCluster);
		sprintf(cComment,"%.64s",field[14]);
		sscanf(field[15],"%u",&uOwner);
		sscanf(field[16],"%u",&uCreatedBy);
		sscanf(field[17],"%lu",&uCreatedDate);
		sscanf(field[18],"%u",&uModBy);
		sscanf(field[19],"%lu",&uModDate);

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
	printf("<input title='%s' type=text name=uPBX value='%u' size=16 maxlength=10 "
		,LANG_FT_tPBX_uPBX,uPBX);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uPBX value='%u' >\n",uPBX);
	}
	//cLabel uRADType=253
	OpenRow(LANG_FL_tPBX_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value='%s' size=40 maxlength=32 "
		,LANG_FT_tPBX_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value='%s'>\n",EncodeDoubleQuotes(cLabel));
	}
	//cAddress uRADType=253
	OpenRow(LANG_FL_tPBX_cAddress,"black");
	printf("<input title='%s' type=text name=cAddress value='%s' size=40 maxlength=63 "
		,LANG_FT_tPBX_cAddress,EncodeDoubleQuotes(cAddress));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cAddress value='%s'>\n",EncodeDoubleQuotes(cAddress));
	}
	//cHostname uRADType=253
	OpenRow(LANG_FL_tPBX_cHostname,"black");
	printf("<input title='%s' type=text name=cHostname value='%s' size=40 maxlength=63 "
		,LANG_FT_tPBX_cHostname,EncodeDoubleQuotes(cHostname));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cHostname value='%s'>\n",EncodeDoubleQuotes(cHostname));
	}
	//uPort uRADType=3
	OpenRow(LANG_FL_tPBX_uPort,"black");
	printf("<input title='%s' type=text name=uPort value='%u' size=16 maxlength=10 "
		,LANG_FT_tPBX_uPort,uPort);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uPort value='%u' >\n",uPort);
	}
	//uPriority uRADType=3
	OpenRow(LANG_FL_tPBX_uPriority,"black");
	printf("<input title='%s' type=text name=uPriority value='%u' size=16 maxlength=10 "
		,LANG_FT_tPBX_uPriority,uPriority);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uPriority value='%u' >\n",uPriority);
	}
	//uWeight uRADType=3
	OpenRow(LANG_FL_tPBX_uWeight,"black");
	printf("<input title='%s' type=text name=uWeight value='%u' size=16 maxlength=10 "
		,LANG_FT_tPBX_uWeight,uWeight);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uWeight value='%u' >\n",uWeight);
	}
	//cAttributes uRADType=253
	OpenRow(LANG_FL_tPBX_cAttributes,"black");
	printf("<input title='%s' type=text name=cAttributes value='%s' size=40 maxlength=63 "
		,LANG_FT_tPBX_cAttributes,EncodeDoubleQuotes(cAttributes));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cAttributes value='%s'>\n",EncodeDoubleQuotes(cAttributes));
	}
	//cDescription uRADType=253
	OpenRow(LANG_FL_tPBX_cDescription,"black");
	printf("<input title='%s' type=text name=cDescription value='%s' size=40 maxlength=63 "
		,LANG_FT_tPBX_cDescription,EncodeDoubleQuotes(cDescription));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cDescription value='%s'>\n",EncodeDoubleQuotes(cDescription));
	}
	//uStatus uRADType=3
	OpenRow(LANG_FL_tPBX_uStatus,"black");
	printf("<input title='%s' type=text name=uStatus value='%u' size=16 maxlength=10 "
		,LANG_FT_tPBX_uStatus,uStatus);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uStatus value='%u' >\n",uStatus);
	}
	//uLines uRADType=3
	OpenRow(LANG_FL_tPBX_uLines,"black");
	printf("<input title='%s' type=text name=uLines value='%u' size=16 maxlength=10 "
		,LANG_FT_tPBX_uLines,uLines);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uLines value='%u' >\n",uLines);
	}
	//uStrip uRADType=3
	OpenRow(LANG_FL_tPBX_uStrip,"black");
	printf("<input title='%s' type=text name=uStrip value='%u' size=16 maxlength=10 "
		,LANG_FT_tPBX_uStrip,uStrip);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uStrip value='%u' >\n",uStrip);
	}
	//cPrefix uRADType=253
	OpenRow(LANG_FL_tPBX_cPrefix,"black");
	printf("<input title='%s' type=text name=cPrefix value='%s' size=40 maxlength=31 "
		,LANG_FT_tPBX_cPrefix,EncodeDoubleQuotes(cPrefix));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cPrefix value='%s'>\n",EncodeDoubleQuotes(cPrefix));
	}
	//uCluster COLTYPE_SELECTTABLE
	OpenRow(LANG_FL_tPBX_uCluster,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tCluster;cuClusterPullDown","cLabel","cLabel",uCluster,1);
	else
		tTablePullDown("tCluster;cuClusterPullDown","cLabel","cLabel",uCluster,0);
	//cComment uRADType=253
	OpenRow(LANG_FL_tPBX_cComment,"black");
	printf("<input title='%s' type=text name=cComment value='%s' size=40 maxlength=63 "
		,LANG_FT_tPBX_cComment,EncodeDoubleQuotes(cComment));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cComment value='%s'>\n",EncodeDoubleQuotes(cComment));
	}
	//uOwner COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tPBX_uOwner,"black");
	printf("%s<input type=hidden name=uOwner value='%u' >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tPBX_uCreatedBy,"black");
	printf("%s<input type=hidden name=uCreatedBy value='%u' >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tPBX_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value='%lu' >\n",uCreatedDate);
	//uModBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tPBX_uModBy,"black");
	printf("%s<input type=hidden name=uModBy value='%u' >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tPBX_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value='%lu' >\n",uModDate);
	printf("</tr>\n");

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
#ifdef StandardFields
	uCreatedDate=luGetCreatedDate("tPBX",uPBX);
#endif
	unxsSPSLog(uPBX,"tPBX","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uPBX);
		tPBX(gcQuery);
	}

}//NewtPBX(unsigned uMode)


void DeletetPBX(void)
{
#ifdef StandardFields
	sprintf(gcQuery,"DELETE FROM tPBX WHERE uPBX=%u AND ( uOwner=%u OR %u>9 )"
					,uPBX,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tPBX WHERE uPBX=%u AND %u>9 )"
					,uPBX,guPermLevel);
#endif
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
	sprintf(gcQuery,"INSERT INTO tPBX SET "
		"cLabel='%s',"
		"cAddress='%s',"
		"cHostname='%s',"
		"uPort=%u,"
		"uPriority=%u,"
		"uWeight=%u,"
		"cAttributes='%s',"
		"cDescription='%s',"
		"uStatus=%u,"
		"uLines=%u,"
		"uStrip=%u,"
		"cPrefix='%s',"
		"uCluster=%u,"
		"cComment='%s',"
		"uOwner=%u,"
		"uCreatedBy=%u,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,TextAreaSave(cLabel)
			,TextAreaSave(cAddress)
			,TextAreaSave(cHostname)
			,uPort
			,uPriority
			,uWeight
			,TextAreaSave(cAttributes)
			,TextAreaSave(cDescription)
			,uStatus
			,uLines
			,uStrip
			,TextAreaSave(cPrefix)
			,uCluster
			,TextAreaSave(cComment)
			,uOwner
			,uCreatedBy
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tPBX(void)


void Update_tPBX(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tPBX SET "
		"cLabel='%s',"
		"cAddress='%s',"
		"cHostname='%s',"
		"uPort=%u,"
		"uPriority=%u,"
		"uWeight=%u,"
		"cAttributes='%s',"
		"cDescription='%s',"
		"uStatus=%u,"
		"uLines=%u,"
		"uStrip=%u,"
		"cPrefix='%s',"
		"uCluster=%u,"
		"cComment='%s',"
		"uOwner=%u,"
		"uModBy=%u,"
		"uModDate=UNIX_TIMESTAMP(NOW())"
		" WHERE _rowid=%s"
			,TextAreaSave(cLabel)
			,TextAreaSave(cAddress)
			,TextAreaSave(cHostname)
			,uPort
			,uPriority
			,uWeight
			,TextAreaSave(cAttributes)
			,TextAreaSave(cDescription)
			,uStatus
			,uLines
			,uStrip
			,TextAreaSave(cPrefix)
			,uCluster
			,TextAreaSave(cComment)
			,uOwner
			,uModBy
			,cRowid
		);

	macro_mySQLQueryHTMLError;

}//void Update_tPBX(void)


void ModtPBX(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

#ifdef StandardFields
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
#else
	sprintf(gcQuery,"SELECT uPBX FROM tPBX"
				" WHERE uPBX=%u"
					,uPBX);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) tPBX(LANG_NBR_RECNOTEXIST);
	if(i>1) tPBX(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef StandardFields
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tPBX(LANG_NBR_EXTMOD);
#endif

	Update_tPBX(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef StandardFields
	uModDate=luGetModDate("tPBX",uPBX);
#endif
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
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>cAddress"
		"<td><font face=arial,helvetica color=white>cHostname"
		"<td><font face=arial,helvetica color=white>uPort"
		"<td><font face=arial,helvetica color=white>uPriority"
		"<td><font face=arial,helvetica color=white>uWeight"
		"<td><font face=arial,helvetica color=white>cAttributes"
		"<td><font face=arial,helvetica color=white>cDescription"
		"<td><font face=arial,helvetica color=white>uStatus"
		"<td><font face=arial,helvetica color=white>uLines"
		"<td><font face=arial,helvetica color=white>uStrip"
		"<td><font face=arial,helvetica color=white>cPrefix"
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
				time_t luTime17=strtoul(field[17],NULL,10);
		char cBuf17[32];
		if(luTime17)
			ctime_r(&luTime17,cBuf17);
		else
			sprintf(cBuf17,"---");
		time_t luTime19=strtoul(field[19],NULL,10);
		char cBuf19[32];
		if(luTime19)
			ctime_r(&luTime19,cBuf19);
		else
			sprintf(cBuf19,"---");
		printf("<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tPBX&uPBX=%s>%s</a><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
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
			,ForeignKey("tCluster","cLabel",strtoul(field[13],NULL,10))
			,field[14]
			,ForeignKey("tClient","cLabel",strtoul(field[15],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[16],NULL,10))
			,cBuf17
			,ForeignKey("tClient","cLabel",strtoul(field[18],NULL,10))
			,cBuf19
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tPBXList()


void CreatetPBX(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tPBX ("
		"uPBX INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
		"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
		"cAddress VARCHAR(64) NOT NULL DEFAULT '', INDEX (cAddress),"
		"cHostname VARCHAR(64) NOT NULL DEFAULT '', INDEX (cHostname),"
		"uPort INT UNSIGNED NOT NULL DEFAULT 0,"
		"uPriority INT UNSIGNED NOT NULL DEFAULT 0,"
		"uWeight INT UNSIGNED NOT NULL DEFAULT 0,"
		"cAttributes VARCHAR(64) NOT NULL DEFAULT '',"
		"cDescription VARCHAR(64) NOT NULL DEFAULT '',"
		"uStatus INT UNSIGNED NOT NULL DEFAULT 0,"
		"uLines INT UNSIGNED NOT NULL DEFAULT 0,"
		"uStrip INT UNSIGNED NOT NULL DEFAULT 0,"
		"cPrefix VARCHAR(32) NOT NULL DEFAULT '',"
		"uCluster INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uCluster),"
		"cComment VARCHAR(64) NOT NULL DEFAULT '',"
		"uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner),"
		"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetPBX(void)


