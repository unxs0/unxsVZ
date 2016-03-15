/*
FILE
	tConfig source code of unxsVZ.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id$
PURPOSE
AUTHOR/LEGAL
	(C) 2001-2016 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uConfig: Primary Key
static unsigned uConfig=0;
//cLabel: Short label
static char cLabel[100]={""};
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
//uContainerType: VZ or Google Compute Engine VM
static unsigned uContainerType=0;
static char cuContainerTypePullDown[256]={""};



#define VAR_LIST_tConfig "tConfig.uConfig,tConfig.cLabel,tConfig.uOwner,tConfig.uCreatedBy,tConfig.uCreatedDate,tConfig.uModBy,tConfig.uModDate,tConfig.uContainerType"

 //Local only
void Insert_tConfig(void);
void Update_tConfig(char *cRowid);
void ProcesstConfigListVars(pentry entries[], int x);

 //In tConfigfunc.h file included below
void ExtProcesstConfigVars(pentry entries[], int x);
void ExttConfigCommands(pentry entries[], int x);
void ExttConfigButtons(void);
void ExttConfigNavBar(void);
void ExttConfigGetHook(entry gentries[], int x);
void ExttConfigSelect(void);
void ExttConfigSelectRow(void);
void ExttConfigListSelect(void);
void ExttConfigListFilter(void);
void ExttConfigAuxTable(void);

#include "tconfigfunc.h"

 //Table Variables Assignment Function
void ProcesstConfigVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uConfig"))
			sscanf(entries[i].val,"%u",&uConfig);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.99s",entries[i].val);
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
		else if(!strcmp(entries[i].name,"uContainerType"))
			sscanf(entries[i].val,"%u",&uContainerType);
		else if(!strcmp(entries[i].name,"cuContainerTypePullDown"))
		{
			sprintf(cuContainerTypePullDown,"%.255s",entries[i].val);
			uContainerType=ReadPullDown("tContainerType","cLabel",cuContainerTypePullDown);
		}

	}

	//After so we can overwrite form data if needed.
	ExtProcesstConfigVars(entries,x);

}//ProcesstConfigVars()


void ProcesstConfigListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uConfig);
                        guMode=2002;
                        tConfig("");
                }
        }
}//void ProcesstConfigListVars(pentry entries[], int x)


int tConfigCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttConfigCommands(entries,x);

	if(!strcmp(gcFunction,"tConfigTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tConfigList();
		}

		//Default
		ProcesstConfigVars(entries,x);
		tConfig("");
	}
	else if(!strcmp(gcFunction,"tConfigList"))
	{
		ProcessControlVars(entries,x);
		ProcesstConfigListVars(entries,x);
		tConfigList();
	}

	return(0);

}//tConfigCommands()


void tConfig(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttConfigSelectRow();
		else
			ExttConfigSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetConfig();
				unxsVZ("New tConfig table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tConfig WHERE uConfig=%u"
						,uConfig);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uConfig);
		sprintf(cLabel,"%.99s",field[1]);
		sscanf(field[2],"%u",&uOwner);
		sscanf(field[3],"%u",&uCreatedBy);
		sscanf(field[4],"%lu",&uCreatedDate);
		sscanf(field[5],"%u",&uModBy);
		sscanf(field[6],"%lu",&uModDate);
		sscanf(field[7],"%u",&uContainerType);

		}

	}//Internal Skip

	Header_ism3(":: Config File Used on Creation",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tConfigTools>");
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

        ExttConfigButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tConfig Record Data",100);

	if(guMode==2000 || guMode==2002)
		tConfigInput(1);
	else
		tConfigInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttConfigAuxTable();

	Footer_ism3();

}//end of tConfig();


void tConfigInput(unsigned uMode)
{

//uConfig
	OpenRow(LANG_FL_tConfig_uConfig,"black");
	printf("<input title='%s' type=text name=uConfig value=%u size=16 maxlength=10 "
,LANG_FT_tConfig_uConfig,uConfig);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uConfig value=%u >\n",uConfig);
	}
//cLabel
	OpenRow(LANG_FL_tConfig_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=99 "
,LANG_FT_tConfig_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uContainerType
	OpenRow(LANG_FL_tContainer_uContainerType,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tContainerType;cuContainerTypePullDown","cLabel","cLabel",uContainerType,1);
	else
		tTablePullDown("tContainerType;cuContainerTypePullDown","cLabel","cLabel",uContainerType,0);
//uOwner
	OpenRow(LANG_FL_tConfig_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tConfig_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tConfig_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tConfig_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tConfig_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tConfigInput(unsigned uMode)


void NewtConfig(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uConfig FROM tConfig WHERE uConfig=%u",uConfig);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tConfig("<blink>Record already exists");
		tConfig(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tConfig();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uConfig=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tConfig",uConfig);
	unxsVZLog(uConfig,"tConfig","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uConfig);
		tConfig(gcQuery);
	}

}//NewtConfig(unsigned uMode)


void DeletetConfig(void)
{
	sprintf(gcQuery,"DELETE FROM tConfig WHERE uConfig=%u AND ( uOwner=%u OR %u>9 )"
					,uConfig,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tConfig("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsVZLog(uConfig,"tConfig","Del");
		tConfig(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsVZLog(uConfig,"tConfig","DelError");
		tConfig(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetConfig(void)


void Insert_tConfig(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tConfig SET uConfig=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,uContainerType=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uConfig
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy
			,uContainerType
		);
	MYSQL_RUN;

}//void Insert_tConfig(void)


void Update_tConfig(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tConfig SET uConfig=%u,cLabel='%s',uModBy=%u,uContainerType=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uConfig
			,TextAreaSave(cLabel)
			,uModBy
			,uContainerType
			,cRowid);

	MYSQL_RUN;

}//void Update_tConfig(void)


void ModtConfig(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tConfig.uConfig,"
				"tConfig.uModDate"
				"FROM tConfig,tClient"
				"WHERE tConfig.uConfig=%u"
				"AND tConfig.uOwner=tClient.uClient"
				"AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uConfig,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uConfig,uModDate FROM tConfig\
				WHERE uConfig=%u"
						,uConfig);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tConfig("<blink>Record does not exist");
	if(i<1) tConfig(LANG_NBR_RECNOTEXIST);
	//if(i>1) tConfig("<blink>Multiple rows!");
	if(i>1) tConfig(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tConfig(LANG_NBR_EXTMOD);

	Update_tConfig(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tConfig",uConfig);
	unxsVZLog(uConfig,"tConfig","Mod");
	tConfig(gcQuery);

}//ModtConfig(void)


void tConfigList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttConfigListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tConfigList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttConfigListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uConfig"
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>uContainerType"
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
		printf("<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tConfig&uConfig=%s>%s</a>"
			,field[0]
			,field[0]);
		printf("<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[1]
			,ForeignKey("tContainerType","cLabel",strtoul(field[7],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[3],NULL,10))
			,cBuf4
			,ForeignKey("tClient","cLabel",strtoul(field[5],NULL,10))
			,cBuf6);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tConfigList()


void CreatetConfig(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tConfig ( "
			"uConfig INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			"cLabel VARCHAR(100) NOT NULL DEFAULT '',"
			"uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner),"
			"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uContainerType INT UNSIGNED NOT NULL DEFAULT 0,"
			"uDatacenter INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetConfig()

