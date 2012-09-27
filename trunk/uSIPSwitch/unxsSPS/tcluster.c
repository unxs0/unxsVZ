/*
FILE
	$Id: module.c 2115 2012-09-19 14:11:03Z Gary $
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality can be developed in tclusterfunc.h
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
static unsigned uCluster=0;
static char cLabel[33]={""};
static char cComment[33]={""};
static unsigned uOwner=0;
#define StandardFields
static unsigned uCreatedBy=0;
static time_t uCreatedDate=0;
static unsigned uModBy=0;
static time_t uModDate=0;


#define VAR_LIST_tCluster "tCluster.uCluster,tCluster.cLabel,tCluster.cComment,tCluster.uOwner,tCluster.uCreatedBy,tCluster.uCreatedDate,tCluster.uModBy,tCluster.uModDate"

 //Local only
void Insert_tCluster(void);
void Update_tCluster(char *cRowid);
void ProcesstClusterListVars(pentry entries[], int x);

 //In tClusterfunc.h file included below
void ExtProcesstClusterVars(pentry entries[], int x);
void ExttClusterCommands(pentry entries[], int x);
void ExttClusterButtons(void);
void ExttClusterNavBar(void);
void ExttClusterGetHook(entry gentries[], int x);
void ExttClusterSelect(void);
void ExttClusterSelectRow(void);
void ExttClusterListSelect(void);
void ExttClusterListFilter(void);
void ExttClusterAuxTable(void);

#include "tclusterfunc.h"

 //Table Variables Assignment Function
void ProcesstClusterVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		
		if(!strcmp(entries[i].name,"uCluster"))
			sscanf(entries[i].val,"%u",&uCluster);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.40s",entries[i].val);
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
	ExtProcesstClusterVars(entries,x);

}//ProcesstClusterVars()


void ProcesstClusterListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uCluster);
                        guMode=2002;
                        tCluster("");
                }
        }
}//void ProcesstClusterListVars(pentry entries[], int x)


int tClusterCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttClusterCommands(entries,x);

	if(!strcmp(gcFunction,"tClusterTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tClusterList();
		}

		//Default
		ProcesstClusterVars(entries,x);
		tCluster("");
	}
	else if(!strcmp(gcFunction,"tClusterList"))
	{
		ProcessControlVars(entries,x);
		ProcesstClusterListVars(entries,x);
		tClusterList();
	}

	return(0);

}//tClusterCommands()


void tCluster(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttClusterSelectRow();
		else
			ExttClusterSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetCluster();
				unxsSPS("New tCluster table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tCluster WHERE uCluster=%u"
						,uCluster);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			
		sscanf(field[0],"%u",&uCluster);
		sprintf(cLabel,"%.32s",field[1]);
		sprintf(cComment,"%.32s",field[2]);
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uCreatedBy);
		sscanf(field[5],"%lu",&uCreatedDate);
		sscanf(field[6],"%u",&uModBy);
		sscanf(field[7],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: Group of SIP servers",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tClusterTools>");
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

        ExttClusterButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tCluster Record Data",100);

	if(guMode==2000 || guMode==2002)
		tClusterInput(1);
	else
		tClusterInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttClusterAuxTable();

	Footer_ism3();

}//end of tCluster();


void tClusterInput(unsigned uMode)
{

	
	//uCluster uRADType=1001
	OpenRow(LANG_FL_tCluster_uCluster,"black");
	printf("<input title='%s' type=text name=uCluster value='%u' size=16 maxlength=10 "
		,LANG_FT_tCluster_uCluster,uCluster);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uCluster value='%u' >\n",uCluster);
	}
	//cLabel uRADType=253
	OpenRow(LANG_FL_tCluster_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value='%s' size=40 maxlength=32 "
		,LANG_FT_tCluster_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value='%s'>\n",EncodeDoubleQuotes(cLabel));
	}
	//cComment uRADType=253
	OpenRow(LANG_FL_tCluster_cComment,"black");
	printf("<input title='%s' type=text name=cComment value='%s' size=40 maxlength=31 "
		,LANG_FT_tCluster_cComment,EncodeDoubleQuotes(cComment));
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
	OpenRow(LANG_FL_tCluster_uOwner,"black");
	printf("%s<input type=hidden name=uOwner value='%u' >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	//uCreatedBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tCluster_uCreatedBy,"black");
	printf("%s<input type=hidden name=uCreatedBy value='%u' >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	//uCreatedDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tCluster_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value='%lu' >\n",uCreatedDate);
	//uModBy COLTYPE_FOREIGNKEY
	OpenRow(LANG_FL_tCluster_uModBy,"black");
	printf("%s<input type=hidden name=uModBy value='%u' >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	//uModDate COLTYPE_UNIXTIMECREATE COLTYPE_UNIXTIMEUPDATE
	OpenRow(LANG_FL_tCluster_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value='%lu' >\n",uModDate);
	printf("</tr>\n");

}//void tClusterInput(unsigned uMode)


void NewtCluster(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uCluster FROM tCluster WHERE uCluster=%u",uCluster);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		tCluster(LANG_NBR_RECEXISTS);

	Insert_tCluster();
	uCluster=mysql_insert_id(&gMysql);
#ifdef StandardFields
	uCreatedDate=luGetCreatedDate("tCluster",uCluster);
#endif
	unxsSPSLog(uCluster,"tCluster","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uCluster);
		tCluster(gcQuery);
	}

}//NewtCluster(unsigned uMode)


void DeletetCluster(void)
{
#ifdef StandardFields
	sprintf(gcQuery,"DELETE FROM tCluster WHERE uCluster=%u AND ( uOwner=%u OR %u>9 )"
					,uCluster,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tCluster WHERE uCluster=%u AND %u>9 )"
					,uCluster,guPermLevel);
#endif
	macro_mySQLQueryHTMLError;
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsSPSLog(uCluster,"tCluster","Del");
		tCluster(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsSPSLog(uCluster,"tCluster","DelError");
		tCluster(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetCluster(void)


void Insert_tCluster(void)
{
	sprintf(gcQuery,"INSERT INTO tCluster SET "
		"cLabel='%s',"
		"cComment='%s',"
		"uOwner=%u,"
		"uCreatedBy=%u,"
		"uCreatedDate=UNIX_TIMESTAMP(NOW())"
			,TextAreaSave(cLabel)
			,TextAreaSave(cComment)
			,uOwner
			,uCreatedBy
		);

	macro_mySQLQueryHTMLError;

}//void Insert_tCluster(void)


void Update_tCluster(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tCluster SET "
		"cLabel='%s',"
		"cComment='%s',"
		"uOwner=%u,"
		"uModBy=%u,"
		"uModDate=UNIX_TIMESTAMP(NOW())"
		" WHERE _rowid=%s"
			,TextAreaSave(cLabel)
			,TextAreaSave(cComment)
			,uOwner
			,uModBy
			,cRowid
		);

	macro_mySQLQueryHTMLError;

}//void Update_tCluster(void)


void ModtCluster(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

#ifdef StandardFields
	unsigned uPreModDate=0;
	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tCluster.uCluster,"
				" tCluster.uModDate"
				" FROM tCluster,tClient"
				" WHERE tCluster.uCluster=%u"
				" AND tCluster.uOwner=tClient.uClient"
				" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uCluster,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uCluster,uModDate FROM tCluster"
				" WHERE uCluster=%u"
					,uCluster);
#else
	sprintf(gcQuery,"SELECT uCluster FROM tCluster"
				" WHERE uCluster=%u"
					,uCluster);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i<1) tCluster(LANG_NBR_RECNOTEXIST);
	if(i>1) tCluster(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef StandardFields
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tCluster(LANG_NBR_EXTMOD);
#endif

	Update_tCluster(field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef StandardFields
	uModDate=luGetModDate("tCluster",uCluster);
#endif
	unxsSPSLog(uCluster,"tCluster","Mod");
	tCluster(gcQuery);

}//ModtCluster(void)


void tClusterList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttClusterListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tClusterList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttClusterListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uCluster"
		"<td><font face=arial,helvetica color=white>cLabel"
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
		printf("<td><a class=darkLink href=unxsSPS.cgi?gcFunction=tCluster&uCluster=%s>%s</a><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,ForeignKey("tClient","cLabel",strtoul(field[3],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[4],NULL,10))
			,cBuf5
			,ForeignKey("tClient","cLabel",strtoul(field[6],NULL,10))
			,cBuf7
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tClusterList()


void CreatetCluster(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tCluster ("
		"uCluster INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
		"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
		"cComment VARCHAR(32) NOT NULL DEFAULT '',"
		"uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
		"uModDate INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//void CreatetCluster(void)


