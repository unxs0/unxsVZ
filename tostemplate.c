/*
FILE
	tOSTemplate source code of unxsVZ.cgi
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
//uOSTemplate: Primary Key
static unsigned uOSTemplate=0;
//cLabel: Short label
static char cLabel[101]={""};
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


#define VAR_LIST_tOSTemplate "tOSTemplate.uOSTemplate,tOSTemplate.cLabel,tOSTemplate.uOwner,tOSTemplate.uCreatedBy,tOSTemplate.uCreatedDate,tOSTemplate.uModBy,tOSTemplate.uModDate,tOSTemplate.uContainerType"

 //Local only
void Insert_tOSTemplate(void);
void Update_tOSTemplate(char *cRowid);
void ProcesstOSTemplateListVars(pentry entries[], int x);

 //In tOSTemplatefunc.h file included below
void ExtProcesstOSTemplateVars(pentry entries[], int x);
void ExttOSTemplateCommands(pentry entries[], int x);
void ExttOSTemplateButtons(void);
void ExttOSTemplateNavBar(void);
void ExttOSTemplateGetHook(entry gentries[], int x);
void ExttOSTemplateSelect(void);
void ExttOSTemplateSelectRow(void);
void ExttOSTemplateListSelect(void);
void ExttOSTemplateListFilter(void);
void ExttOSTemplateAuxTable(void);

#include "tostemplatefunc.h"

 //Table Variables Assignment Function
void ProcesstOSTemplateVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uOSTemplate"))
			sscanf(entries[i].val,"%u",&uOSTemplate);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.100s",entries[i].val);
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
	ExtProcesstOSTemplateVars(entries,x);

}//ProcesstOSTemplateVars()


void ProcesstOSTemplateListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uOSTemplate);
                        guMode=2002;
                        tOSTemplate("");
                }
        }
}//void ProcesstOSTemplateListVars(pentry entries[], int x)


int tOSTemplateCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttOSTemplateCommands(entries,x);

	if(!strcmp(gcFunction,"tOSTemplateTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tOSTemplateList();
		}

		//Default
		ProcesstOSTemplateVars(entries,x);
		tOSTemplate("");
	}
	else if(!strcmp(gcFunction,"tOSTemplateList"))
	{
		ProcessControlVars(entries,x);
		ProcesstOSTemplateListVars(entries,x);
		tOSTemplateList();
	}

	return(0);

}//tOSTemplateCommands()


void tOSTemplate(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttOSTemplateSelectRow();
		else
			ExttOSTemplateSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetOSTemplate();
				unxsVZ("New tOSTemplate table created");
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
				sprintf(gcQuery,"SELECT _rowid FROM tOSTemplate WHERE uOSTemplate=%u"
						,uOSTemplate);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			sscanf(field[0],"%u",&uOSTemplate);
			sprintf(cLabel,"%.100s",field[1]);
			sscanf(field[2],"%u",&uOwner);
			sscanf(field[3],"%u",&uCreatedBy);
			sscanf(field[4],"%lu",&uCreatedDate);
			sscanf(field[5],"%u",&uModBy);
			sscanf(field[6],"%lu",&uModDate);
			sscanf(field[7],"%u",&uContainerType);
		}

	}//Internal Skip

	Header_ism3(":: Container OS Template",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tOSTemplateTools>");
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

        ExttOSTemplateButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tOSTemplate Record Data",100);

	if(guMode==2000 || guMode==2002)
		tOSTemplateInput(1);
	else
		tOSTemplateInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttOSTemplateAuxTable();

	Footer_ism3();

}//end of tOSTemplate();


void tOSTemplateInput(unsigned uMode)
{

//uOSTemplate
	OpenRow(LANG_FL_tOSTemplate_uOSTemplate,"black");
	printf("<input title='%s' type=text name=uOSTemplate value=%u size=16 maxlength=10 "
,LANG_FT_tOSTemplate_uOSTemplate,uOSTemplate);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uOSTemplate value=%u >\n",uOSTemplate);
	}
//cLabel
	OpenRow(LANG_FL_tOSTemplate_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=100 maxlength=100 "
,LANG_FT_tOSTemplate_cLabel,EncodeDoubleQuotes(cLabel));
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
	OpenRow(LANG_FL_tOSTemplate_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tOSTemplate_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tOSTemplate_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tOSTemplate_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tOSTemplate_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tOSTemplateInput(unsigned uMode)


void NewtOSTemplate(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uOSTemplate FROM tOSTemplate WHERE uOSTemplate=%u",uOSTemplate);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tOSTemplate("<blink>Record already exists");
		tOSTemplate(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tOSTemplate();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uOSTemplate=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tOSTemplate",uOSTemplate);
	unxsVZLog(uOSTemplate,"tOSTemplate","New");
	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uOSTemplate);
		tOSTemplate(gcQuery);
	}

}//NewtOSTemplate(unsigned uMode)


void DeletetOSTemplate(void)
{
	sprintf(gcQuery,"DELETE FROM tOSTemplate WHERE uOSTemplate=%u AND ( uOwner=%u OR %u>9 )"
					,uOSTemplate,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tOSTemplate("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsVZLog(uOSTemplate,"tOSTemplate","Del");
		tOSTemplate(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsVZLog(uOSTemplate,"tOSTemplate","DelError");
		tOSTemplate(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetOSTemplate(void)


void Insert_tOSTemplate(void)
{
	sprintf(gcQuery,"INSERT INTO tOSTemplate SET uOSTemplate=%u,cLabel='%s',uOwner=%u,uContainerType=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uOSTemplate
			,TextAreaSave(cLabel)
			,uOwner
			,uContainerType
			,uCreatedBy);
	MYSQL_RUN;

}//void Insert_tOSTemplate(void)


void Update_tOSTemplate(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tOSTemplate SET uOSTemplate=%u,cLabel='%s',uModBy=%u,uContainerType=%u"
			"uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uOSTemplate
			,TextAreaSave(cLabel)
			,uContainerType
			,uModBy
			,cRowid);
	MYSQL_RUN;

}//void Update_tOSTemplate(void)


void ModtOSTemplate(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tOSTemplate.uOSTemplate,\
				tOSTemplate.uModDate\
				FROM tOSTemplate,tClient\
				WHERE tOSTemplate.uOSTemplate=%u\
				AND tOSTemplate.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uOSTemplate,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uOSTemplate,uModDate FROM tOSTemplate\
				WHERE uOSTemplate=%u"
						,uOSTemplate);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tOSTemplate("<blink>Record does not exist");
	if(i<1) tOSTemplate(LANG_NBR_RECNOTEXIST);
	//if(i>1) tOSTemplate("<blink>Multiple rows!");
	if(i>1) tOSTemplate(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tOSTemplate(LANG_NBR_EXTMOD);

	Update_tOSTemplate(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tOSTemplate",uOSTemplate);
	unxsVZLog(uOSTemplate,"tOSTemplate","Mod");
	tOSTemplate(gcQuery);

}//ModtOSTemplate(void)


void tOSTemplateList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttOSTemplateListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tOSTemplateList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttOSTemplateListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uOSTemplate"
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
		printf("<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tOSTemplate&uOSTemplate=%s>%s</a>"
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

}//tOSTemplateList()


void CreatetOSTemplate(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tOSTemplate ( "
			"uOSTemplate INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
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
}//CreatetOSTemplate()

