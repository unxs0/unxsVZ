/*
FILE
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in tgroupfunc.h
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
static unsigned uGroup=0;
static char cLabel[33]={""};
static unsigned uGroupType=0;
static char cuGroupTypePullDown[256]={""};
static unsigned uOwner=0;
#define StandardFields
static unsigned uCreatedBy=0;
static time_t uCreatedDate=0;
static unsigned uModBy=0;
static time_t uModDate=0;


#define VAR_LIST_tGroup "tGroup.uGroup,tGroup.cLabel,tGroup.uGroupType,tGroup.uOwner,tGroup.uCreatedBy,tGroup.uCreatedDate,tGroup.uModBy,tGroup.uModDate"

 //Local only
void Insert_tGroup(void);
void Update_tGroup(char *cRowid);
void ProcesstGroupListVars(pentry entries[], int x);

 //In tGroupfunc.h file included below
void ExtProcesstGroupVars(pentry entries[], int x);
void ExttGroupCommands(pentry entries[], int x);
void ExttGroupButtons(void);
void ExttGroupNavBar(void);
void ExttGroupGetHook(entry gentries[], int x);
void ExttGroupSelect(void);
void ExttGroupSelectRow(void);
void ExttGroupListSelect(void);
void ExttGroupListFilter(void);
void ExttGroupAuxTable(void);

#include "tgroupfunc.h"

 //Table Variables Assignment Function
void ProcesstGroupVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		
		if(!strcmp(entries[i].name,"uGroup"))
			sscanf(entries[i].val,"%u",&uGroup);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.40s",entries[i].val);
		else if(!strcmp(entries[i].name,"uGroupType"))
			sscanf(entries[i].val,"%u",&uGroupType);
		else if(!strcmp(entries[i].name,"cuGroupTypePullDown"))
		{
			sprintf(cuGroupTypePullDown,"%.255s",entries[i].val);
			uGroupType=ReadPullDown("tGroupType","cLabel",cuGroupTypePullDown);
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
	ExtProcesstGroupVars(entries,x);

}//ProcesstGroupVars()


void ProcesstGroupListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uGroup);
                        guMode=2002;
                        tGroup("");
                }
        }
}//void ProcesstGroupListVars(pentry entries[], int x)


int tGroupCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttGroupCommands(entries,x);

	if(!strcmp(gcFunction,"tGroupTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tGroupList();
		}

		//Default
		ProcesstGroupVars(entries,x);
		tGroup("");
	}
	else if(!strcmp(gcFunction,"tGroupList"))
	{
		ProcessControlVars(entries,x);
		ProcesstGroupListVars(entries,x);
		tGroupList();
	}

	return(0);

}//tGroupCommands()


void tGroup(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttGroupSelectRow();
		else
			ExttGroupSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetGroup();
				unxsSPS("New tGroup table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tGroup WHERE uGroup=%u"
						,uGroup);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			
		sscanf(field[0],"%u",&uGroup);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uGroupType);
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uCreatedBy);
		sscanf(field[5],"%lu",&uCreatedDate);
		sscanf(field[6],"%u",&uModBy);
		sscanf(field[7],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: Group names",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tGroupTools>");
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

        ExttGroupButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tGroup Record Data",100);

	if(guMode==2000 || guMode==2002)
		tGroupInput(1);
	else
		tGroupInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttGroupAuxTable();

	Footer_ism3();

}//end of tGroup();


void tGroupInput(unsigned uMode)
{

	
	//uGroup uRADType=1001
	OpenRow(LANG_FL_tGroup_uGroup,"black");
	printf("<input title='%s' type=text name=uGroup id=uGroup value='%u' size=16 maxlength=10 "
		,LANG_FT_tGroup_uGroup,uGroup);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uGroup id=uGroup value='%u' >\n",uGroup);
	}
	//cLabel uRADType=253
	OpenRow(LANG_FL_tGroup_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel id=cLabel value='%s' size=40 maxlength=32 "
		,LANG_FT_tGroup_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel id=cLabel value='%s'>\n",EncodeDoubleQuotes(cLabel));
	}
	//uGroupType COLTYPE_SELECTTABLE
	OpenRow(LANG_FL_tGroup_uGroupType,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tGroupType;cuGroupTypePullDown","cLabel","cLabel",uGroupType,1);
	else
		tTablePullDown("tGroupType;cuGroupTypePullDown","cLabel","cLabel",uGroupType,0);
	//uOwner COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tGroup_uOwner,"black");
	printf("%s<input type=hidden name=uOwner id=uOwner value='%u' >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tGroup_uCreatedBy,"black");
	printf("%s<input type=hidden name=uCreatedBy id=uCreatedBy value='%u' >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tGroup_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate id=uCreatedDate value='%lu' >\n",uCreatedDate);
	//uModBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tGroup_uModBy,"black");
	printf("%s<input type=hidden name=uModBy id=uModBy value='%u' >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tGroup_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate id=uModDate value='%lu' >\n",uModDate);
	printf("</tr>\n");

}//void tGroupInput(unsigned uMode)


void NewtGroup(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uGroup FROM tGroup WHERE uGroup=%u",uGroup);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		tGroup(LANG_NBR_RECEXISTS);

	Insert_tGroup();
	uGroup=mysql_insert_id(&gMysql);
#ifdef StandardFields
	uCreatedDate=luGetCreatedDate("tGroup",uGroup);
#endif
	unxsSPSLog(uGroup,"tGroup","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uGroup);
		tGroup(gcQuery);
	}

}//NewtGroup(unsigned uMode)


void DeletetGroup(void)
{
#ifdef StandardFields
	sprintf(gcQuery,"DELETE FROM tGroup WHERE uGroup=%u AND ( uOwner=%u OR %u>9 )"
					,uGroup,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tGroup WHERE uGroup=%u AND %u>9 )"
					,uGroup,guPermLevel);
#endif
	macro_mySQLQueryHTMLError;
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsSPSLog(uGroup,"tGroup","Del");
		tGroup(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsSPSLog(uGroup,"tGroup","DelError");
		tGroup(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetGroup(void)


void Insert_tGroup(void)
{
	sprintf(gcQuery,"INSERT INTO tGroup SET "
		"cLabel='%s',"
		"uGroupType=%u,"
		"uOwner=%u,"
		"uCreatedBy=%u,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,TextAreaSave(cLabel)
			,uGroupType
			,uOwner
			,uCreatedBy
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tGroup(void)


void Update_tGroup(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tGroup SET "
		"cLabel='%s',"
		"uGroupType=%u,"
		"uOwner=%u,"
		"uModBy=%u,"
		"uModDate=UNIX_TIMESTAMP(NOW())"
		" WHERE _rowid=%s"
			,TextAreaSave(cLabel)
			,uGroupType
			,uOwner
			,uModBy
			,cRowid
		);

	macro_mySQLQueryHTMLError;

}//void Update_tGroup(void)


void ModtGroup(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

#ifdef StandardFields
	unsigned uPreModDate=0;
	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tGroup.uGroup,"
				" tGroup.uModDate"
				" FROM tGroup,tClient"
				" WHERE tGroup.uGroup=%u"
				" AND tGroup.uOwner=tClient.uClient"
				" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uGroup,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uGroup,uModDate FROM tGroup"
				" WHERE uGroup=%u"
					,uGroup);
#else
	sprintf(gcQuery,"SELECT uGroup FROM tGroup"
				" WHERE uGroup=%u"
					,uGroup);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) tGroup(LANG_NBR_RECNOTEXIST);
	if(i>1) tGroup(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef StandardFields
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tGroup(LANG_NBR_EXTMOD);
#endif

	Update_tGroup(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef StandardFields
	uModDate=luGetModDate("tGroup",uGroup);
#endif
	unxsSPSLog(uGroup,"tGroup","Mod");
	tGroup(gcQuery);

}//ModtGroup(void)


void tGroupList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttGroupListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tGroupList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttGroupListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uGroup"
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>uGroupType"
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
		printf("<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tGroup&uGroup=%s>%s</a><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tGroupType","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[3],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[4],NULL,10))
			,cBuf5
			,ForeignKey("tClient","cLabel",strtoul(field[6],NULL,10))
			,cBuf7
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tGroupList()


void CreatetGroup(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tGroup ("
		"uGroup INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
		"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
		"uGroupType INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uGroupType),"
		"uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetGroup(void)


