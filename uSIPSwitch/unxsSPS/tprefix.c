/*
FILE
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in tprefixfunc.h
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
static unsigned uPrefix=0;
static char cLabel[33]={""};
static unsigned uOwner=0;
#define StandardFields
static unsigned uCreatedBy=0;
static time_t uCreatedDate=0;
static unsigned uModBy=0;
static time_t uModDate=0;


#define VAR_LIST_tPrefix "tPrefix.uPrefix,tPrefix.cLabel,tPrefix.uOwner,tPrefix.uCreatedBy,tPrefix.uCreatedDate,tPrefix.uModBy,tPrefix.uModDate"

 //Local only
void Insert_tPrefix(void);
void Update_tPrefix(char *cRowid);
void ProcesstPrefixListVars(pentry entries[], int x);

 //In tPrefixfunc.h file included below
void ExtProcesstPrefixVars(pentry entries[], int x);
void ExttPrefixCommands(pentry entries[], int x);
void ExttPrefixButtons(void);
void ExttPrefixNavBar(void);
void ExttPrefixGetHook(entry gentries[], int x);
void ExttPrefixSelect(void);
void ExttPrefixSelectRow(void);
void ExttPrefixListSelect(void);
void ExttPrefixListFilter(void);
void ExttPrefixAuxTable(void);

#include "tprefixfunc.h"

 //Table Variables Assignment Function
void ProcesstPrefixVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		
		if(!strcmp(entries[i].name,"uPrefix"))
			sscanf(entries[i].val,"%u",&uPrefix);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.40s",entries[i].val);
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
	ExtProcesstPrefixVars(entries,x);

}//ProcesstPrefixVars()


void ProcesstPrefixListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uPrefix);
                        guMode=2002;
                        tPrefix("");
                }
        }
}//void ProcesstPrefixListVars(pentry entries[], int x)


int tPrefixCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttPrefixCommands(entries,x);

	if(!strcmp(gcFunction,"tPrefixTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tPrefixList();
		}

		//Default
		ProcesstPrefixVars(entries,x);
		tPrefix("");
	}
	else if(!strcmp(gcFunction,"tPrefixList"))
	{
		ProcessControlVars(entries,x);
		ProcesstPrefixListVars(entries,x);
		tPrefixList();
	}

	return(0);

}//tPrefixCommands()


void tPrefix(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttPrefixSelectRow();
		else
			ExttPrefixSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetPrefix();
				unxsSPS("New tPrefix table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tPrefix WHERE uPrefix=%u"
						,uPrefix);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			
		sscanf(field[0],"%u",&uPrefix);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: DID prefixes for use in rules a",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tPrefixTools>");
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

        ExttPrefixButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tPrefix Record Data",100);

	if(guMode==2000 || guMode==2002)
		tPrefixInput(1);
	else
		tPrefixInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttPrefixAuxTable();

	Footer_ism3();

}//end of tPrefix();


void tPrefixInput(unsigned uMode)
{

	
	//uPrefix uRADType=1001
	OpenRow(LANG_FL_tPrefix_uPrefix,"black");
	printf("<input title='%s' type=text name=uPrefix id=uPrefix value='%u' size=16 maxlength=10 "
		,LANG_FT_tPrefix_uPrefix,uPrefix);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uPrefix id=uPrefix value='%u' >\n",uPrefix);
	}
	//cLabel uRADType=253
	OpenRow(LANG_FL_tPrefix_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel id=cLabel value='%s' size=40 maxlength=32 "
		,LANG_FT_tPrefix_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel id=cLabel value='%s'>\n",EncodeDoubleQuotes(cLabel));
	}
	//uOwner COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tPrefix_uOwner,"black");
	printf("%s<input type=hidden name=uOwner id=uOwner value='%u' >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tPrefix_uCreatedBy,"black");
	printf("%s<input type=hidden name=uCreatedBy id=uCreatedBy value='%u' >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tPrefix_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate id=uCreatedDate value='%lu' >\n",uCreatedDate);
	//uModBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tPrefix_uModBy,"black");
	printf("%s<input type=hidden name=uModBy id=uModBy value='%u' >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tPrefix_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate id=uModDate value='%lu' >\n",uModDate);
	printf("</tr>\n");

}//void tPrefixInput(unsigned uMode)


void NewtPrefix(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uPrefix FROM tPrefix WHERE uPrefix=%u",uPrefix);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		tPrefix(LANG_NBR_RECEXISTS);

	Insert_tPrefix();
	uPrefix=mysql_insert_id(&gMysql);
#ifdef StandardFields
	uCreatedDate=luGetCreatedDate("tPrefix",uPrefix);
#endif
	unxsSPSLog(uPrefix,"tPrefix","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uPrefix);
		tPrefix(gcQuery);
	}

}//NewtPrefix(unsigned uMode)


void DeletetPrefix(void)
{
#ifdef StandardFields
	sprintf(gcQuery,"DELETE FROM tPrefix WHERE uPrefix=%u AND ( uOwner=%u OR %u>9 )"
					,uPrefix,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tPrefix WHERE uPrefix=%u AND %u>9 )"
					,uPrefix,guPermLevel);
#endif
	macro_mySQLQueryHTMLError;
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsSPSLog(uPrefix,"tPrefix","Del");
		tPrefix(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsSPSLog(uPrefix,"tPrefix","DelError");
		tPrefix(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetPrefix(void)


void Insert_tPrefix(void)
{
	sprintf(gcQuery,"INSERT INTO tPrefix SET "
		"cLabel='%s',"
		"uOwner=%u,"
		"uCreatedBy=%u,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tPrefix(void)


void Update_tPrefix(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tPrefix SET "
		"cLabel='%s',"
		"uOwner=%u,"
		"uModBy=%u,"
		"uModDate=UNIX_TIMESTAMP(NOW())"
		" WHERE _rowid=%s"
			,TextAreaSave(cLabel)
			,uOwner
			,uModBy
			,cRowid
		);

	macro_mySQLQueryHTMLError;

}//void Update_tPrefix(void)


void ModtPrefix(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

#ifdef StandardFields
	unsigned uPreModDate=0;
	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tPrefix.uPrefix,"
				" tPrefix.uModDate"
				" FROM tPrefix,tClient"
				" WHERE tPrefix.uPrefix=%u"
				" AND tPrefix.uOwner=tClient.uClient"
				" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uPrefix,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uPrefix,uModDate FROM tPrefix"
				" WHERE uPrefix=%u"
					,uPrefix);
#else
	sprintf(gcQuery,"SELECT uPrefix FROM tPrefix"
				" WHERE uPrefix=%u"
					,uPrefix);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) tPrefix(LANG_NBR_RECNOTEXIST);
	if(i>1) tPrefix(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef StandardFields
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tPrefix(LANG_NBR_EXTMOD);
#endif

	Update_tPrefix(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef StandardFields
	uModDate=luGetModDate("tPrefix",uPrefix);
#endif
	unxsSPSLog(uPrefix,"tPrefix","Mod");
	tPrefix(gcQuery);

}//ModtPrefix(void)


void tPrefixList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttPrefixListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tPrefixList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttPrefixListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uPrefix"
		"<td><font face=arial,helvetica color=white>cLabel"
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
		printf("<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tPrefix&uPrefix=%s>%s</a><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tClient","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[3],NULL,10))
			,cBuf4
			,ForeignKey("tClient","cLabel",strtoul(field[5],NULL,10))
			,cBuf6
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tPrefixList()


void CreatetPrefix(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tPrefix ("
		"uPrefix INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
		"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
		"uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetPrefix(void)


