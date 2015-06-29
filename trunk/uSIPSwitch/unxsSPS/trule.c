/*
FILE
	$Id: module.c 2115 2012-09-19 14:11:03Z Gary $
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in trulefunc.h
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
static unsigned uRule=0;
static char cLabel[33]={""};
static unsigned uPriority=0;
static char cPrefix[33]={""};
static unsigned uInGroup=0;
static char cuInGroupPullDown[256]={""};
static unsigned uExGroup=0;
static char cuExGroupPullDown[256]={""};
static char cComment[33]={""};
static unsigned uOwner=0;
#define StandardFields
static unsigned uCreatedBy=0;
static time_t uCreatedDate=0;
static unsigned uModBy=0;
static time_t uModDate=0;
static unsigned uCluster=0;
static char cuClusterPullDown[256]={""};


#define VAR_LIST_tRule "tRule.uRule,tRule.cLabel,tRule.uPriority,tRule.cPrefix,tRule.uInGroup,tRule.uExGroup,tRule.cComment,tRule.uOwner,tRule.uCreatedBy,tRule.uCreatedDate,tRule.uModBy,tRule.uModDate,tRule.uCluster"

 //Local only
void Insert_tRule(void);
void Update_tRule(char *cRowid);
void ProcesstRuleListVars(pentry entries[], int x);

 //In tRulefunc.h file included below
void ExtProcesstRuleVars(pentry entries[], int x);
void ExttRuleCommands(pentry entries[], int x);
void ExttRuleButtons(void);
void ExttRuleNavBar(void);
void ExttRuleGetHook(entry gentries[], int x);
void ExttRuleSelect(void);
void ExttRuleSelectRow(void);
void ExttRuleListSelect(void);
void ExttRuleListFilter(void);
void ExttRuleAuxTable(void);

#include "trulefunc.h"

 //Table Variables Assignment Function
void ProcesstRuleVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		
		if(!strcmp(entries[i].name,"uRule"))
			sscanf(entries[i].val,"%u",&uRule);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"uPriority"))
			sscanf(entries[i].val,"%u",&uPriority);
		else if(!strcmp(entries[i].name,"cPrefix"))
			sprintf(cPrefix,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"uInGroup"))
			sscanf(entries[i].val,"%u",&uInGroup);
		else if(!strcmp(entries[i].name,"cuInGroupPullDown"))
		{
			sprintf(cuInGroupPullDown,"%.255s",entries[i].val);
			uInGroup=ReadPullDown("tGroup","cLabel",cuInGroupPullDown);
		}
		else if(!strcmp(entries[i].name,"uExGroup"))
			sscanf(entries[i].val,"%u",&uExGroup);
		else if(!strcmp(entries[i].name,"cuExGroupPullDown"))
		{
			sprintf(cuExGroupPullDown,"%.255s",entries[i].val);
			uExGroup=ReadPullDown("tGroup","cLabel",cuExGroupPullDown);
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
		else if(!strcmp(entries[i].name,"uCluster"))
			sscanf(entries[i].val,"%u",&uCluster);
		else if(!strcmp(entries[i].name,"cuClusterPullDown"))
		{
			sprintf(cuClusterPullDown,"%.255s",entries[i].val);
			uCluster=ReadPullDown("tCluster","cLabel",cuClusterPullDown);
		}

	}

	//After so we can overwrite form data if needed.
	ExtProcesstRuleVars(entries,x);

}//ProcesstRuleVars()


void ProcesstRuleListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uRule);
                        guMode=2002;
                        tRule("");
                }
        }
}//void ProcesstRuleListVars(pentry entries[], int x)


int tRuleCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttRuleCommands(entries,x);

	if(!strcmp(gcFunction,"tRuleTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tRuleList();
		}

		//Default
		ProcesstRuleVars(entries,x);
		tRule("");
	}
	else if(!strcmp(gcFunction,"tRuleList"))
	{
		ProcessControlVars(entries,x);
		ProcesstRuleListVars(entries,x);
		tRuleList();
	}

	return(0);

}//tRuleCommands()


void tRule(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttRuleSelectRow();
		else
			ExttRuleSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetRule();
				unxsSPS("New tRule table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tRule WHERE uRule=%u"
						,uRule);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			
		sscanf(field[0],"%u",&uRule);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uPriority);
		sprintf(cPrefix,"%.32s",field[3]);
		sscanf(field[4],"%u",&uInGroup);
		sscanf(field[5],"%u",&uExGroup);
		sprintf(cComment,"%.32s",field[6]);
		sscanf(field[7],"%u",&uOwner);
		sscanf(field[8],"%u",&uCreatedBy);
		sscanf(field[9],"%lu",&uCreatedDate);
		sscanf(field[10],"%u",&uModBy);
		sscanf(field[11],"%lu",&uModDate);
		sscanf(field[12],"%u",&uCluster);

		}

	}//Internal Skip

	Header_ism3(":: Gateway routing rules",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tRuleTools>");
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

        ExttRuleButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tRule Record Data",100);

	if(guMode==2000 || guMode==2002)
		tRuleInput(1);
	else
		tRuleInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttRuleAuxTable();

	Footer_ism3();

}//end of tRule();


void tRuleInput(unsigned uMode)
{

	
	//uRule uRADType=1001
	OpenRow(LANG_FL_tRule_uRule,"black");
	printf("<input title='%s' type=text name=uRule id=uRule value='%u' size=16 maxlength=10 "
		,LANG_FT_tRule_uRule,uRule);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uRule id=uRule value='%u' >\n",uRule);
	}
	//cLabel uRADType=253
	OpenRow(LANG_FL_tRule_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel id=cLabel value='%s' size=40 maxlength=32 "
		,LANG_FT_tRule_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel id=cLabel value='%s'>\n",EncodeDoubleQuotes(cLabel));
	}
	//uCluster COLTYPE_SELECTTABLE
	OpenRow(LANG_FL_tPBX_uCluster,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tCluster;cuClusterPullDown","cLabel","cLabel",uCluster,1);
	else
		tTablePullDown("tCluster;cuClusterPullDown","cLabel","cLabel",uCluster,0);
	//uPriority uRADType=3
	OpenRow(LANG_FL_tRule_uPriority,"black");
	printf("<input title='%s' type=text name=uPriority id=uPriority value='%u' size=16 maxlength=10 "
		,LANG_FT_tRule_uPriority,uPriority);
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uPriority id=uPriority value='%u' >\n",uPriority);
	}
	//cPrefix uRADType=253
	OpenRow(LANG_FL_tRule_cPrefix,"black");
	printf("<input title='%s' type=text name=cPrefix id=cPrefix value='%s' size=40 maxlength=31 "
		,LANG_FT_tRule_cPrefix,EncodeDoubleQuotes(cPrefix));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cPrefix id=cPrefix value='%s'>\n",EncodeDoubleQuotes(cPrefix));
	}
	//uInGroup COLTYPE_SELECTTABLE
	OpenRow(LANG_FL_tRule_uInGroup,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tGroup;cuInGroupPullDown","cLabel","cLabel",uInGroup,1);
	else
		tTablePullDown("tGroup;cuInGroupPullDown","cLabel","cLabel",uInGroup,0);
	//uExGroup COLTYPE_SELECTTABLE
	OpenRow(LANG_FL_tRule_uExGroup,"black");
	if(guPermLevel>=0 && uMode)
		tTablePullDown("tGroup;cuExGroupPullDown","cLabel","cLabel",uExGroup,1);
	else
		tTablePullDown("tGroup;cuExGroupPullDown","cLabel","cLabel",uExGroup,0);
	//cComment uRADType=253
	OpenRow(LANG_FL_tRule_cComment,"black");
	printf("<input title='%s' type=text name=cComment id=cComment value='%s' size=40 maxlength=31 "
		,LANG_FT_tRule_cComment,EncodeDoubleQuotes(cComment));
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
	OpenRow(LANG_FL_tRule_uOwner,"black");
	printf("%s<input type=hidden name=uOwner id=uOwner value='%u' >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tRule_uCreatedBy,"black");
	printf("%s<input type=hidden name=uCreatedBy id=uCreatedBy value='%u' >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tRule_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate id=uCreatedDate value='%lu' >\n",uCreatedDate);
	//uModBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tRule_uModBy,"black");
	printf("%s<input type=hidden name=uModBy id=uModBy value='%u' >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tRule_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate id=uModDate value='%lu' >\n",uModDate);
	printf("</tr>\n");

}//void tRuleInput(unsigned uMode)


void NewtRule(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uRule FROM tRule WHERE uRule=%u",uRule);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		tRule(LANG_NBR_RECEXISTS);

	Insert_tRule();
	uRule=mysql_insert_id(&gMysql);
#ifdef StandardFields
	uCreatedDate=luGetCreatedDate("tRule",uRule);
#endif
	unxsSPSLog(uRule,"tRule","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uRule);
		tRule(gcQuery);
	}

}//NewtRule(unsigned uMode)


void DeletetRule(void)
{
#ifdef StandardFields
	sprintf(gcQuery,"DELETE FROM tRule WHERE uRule=%u AND ( uOwner=%u OR %u>9 )"
					,uRule,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tRule WHERE uRule=%u AND %u>9 )"
					,uRule,guPermLevel);
#endif
	macro_mySQLQueryHTMLError;
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsSPSLog(uRule,"tRule","Del");
		tRule(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsSPSLog(uRule,"tRule","DelError");
		tRule(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetRule(void)


void Insert_tRule(void)
{
	sprintf(gcQuery,"INSERT INTO tRule SET "
		"cLabel='%s',"
		"uCluster=%u,"
		"uPriority=%u,"
		"cPrefix='%s',"
		"uInGroup=%u,"
		"uExGroup=%u,"
		"cComment='%s',"
		"uOwner=%u,"
		"uCreatedBy=%u,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,TextAreaSave(cLabel)
			,uCluster
			,uPriority
			,TextAreaSave(cPrefix)
			,uInGroup
			,uExGroup
			,TextAreaSave(cComment)
			,uOwner
			,uCreatedBy
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tRule(void)


void Update_tRule(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tRule SET "
		"cLabel='%s',"
		"uCluster=%u,"
		"uPriority=%u,"
		"cPrefix='%s',"
		"uInGroup=%u,"
		"uExGroup=%u,"
		"cComment='%s',"
		"uOwner=%u,"
		"uModBy=%u,"
		"uModDate=UNIX_TIMESTAMP(NOW())"
		" WHERE _rowid=%s"
			,TextAreaSave(cLabel)
			,uCluster
			,uPriority
			,TextAreaSave(cPrefix)
			,uInGroup
			,uExGroup
			,TextAreaSave(cComment)
			,uOwner
			,uModBy
			,cRowid
		);

	macro_mySQLQueryHTMLError;

}//void Update_tRule(void)


void ModtRule(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

#ifdef StandardFields
	unsigned uPreModDate=0;
	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tRule.uRule,"
				" tRule.uModDate"
				" FROM tRule,tClient"
				" WHERE tRule.uRule=%u"
				" AND tRule.uOwner=tClient.uClient"
				" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uRule,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uRule,uModDate FROM tRule"
				" WHERE uRule=%u"
					,uRule);
#else
	sprintf(gcQuery,"SELECT uRule FROM tRule"
				" WHERE uRule=%u"
					,uRule);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) tRule(LANG_NBR_RECNOTEXIST);
	if(i>1) tRule(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef StandardFields
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tRule(LANG_NBR_EXTMOD);
#endif

	Update_tRule(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef StandardFields
	uModDate=luGetModDate("tRule",uRule);
#endif
	unxsSPSLog(uRule,"tRule","Mod");
	tRule(gcQuery);

}//ModtRule(void)


void tRuleList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttRuleListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tRuleList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttRuleListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uRule"
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>uPriority"
		"<td><font face=arial,helvetica color=white>cPrefix"
		"<td><font face=arial,helvetica color=white>uInGroup"
		"<td><font face=arial,helvetica color=white>uExGroup"
		"<td><font face=arial,helvetica color=white>cComment"
		"<td><font face=arial,helvetica color=white>uOwner"
		"<td><font face=arial,helvetica color=white>uCreatedBy"
		"<td><font face=arial,helvetica color=white>uCreatedDate"
		"<td><font face=arial,helvetica color=white>uModBy"
		"<td><font face=arial,helvetica color=white>uModDate"
		"<td><font face=arial,helvetica color=white>uCluster"
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
		printf("<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tRule&uRule=%s>%s</a><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,ForeignKey("tGroup","cLabel",strtoul(field[4],NULL,10))
			,ForeignKey("tGroup","cLabel",strtoul(field[5],NULL,10))
			,field[6]
			,ForeignKey("tClient","cLabel",strtoul(field[7],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[8],NULL,10))
			,cBuf9
			,ForeignKey("tClient","cLabel",strtoul(field[10],NULL,10))
			,cBuf11
			,ForeignKey("tCluster","cLabel",strtoul(field[12],NULL,10))
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tRuleList()


void CreatetRule(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tRule ("
		"uRule INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
		"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
		"uPriority INT UNSIGNED NOT NULL DEFAULT 0,"
		"cPrefix VARCHAR(32) NOT NULL DEFAULT '',"
		"uInGroup INT UNSIGNED NOT NULL DEFAULT 0,"
		"uExGroup INT UNSIGNED NOT NULL DEFAULT 0,"
		"cComment VARCHAR(32) NOT NULL DEFAULT '',"
		"uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCluster INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uCluster))");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetRule(void)


