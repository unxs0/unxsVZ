/*
FILE
	tSearchdomain source code of unxsVZ.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id$
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tsearchdomainfunc.h while 
	RAD is still to be used.
AUTHOR/LEGAL
	(C) 2001-2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file included.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uSearchdomain: Primary Key
static unsigned uSearchdomain=0;
//cLabel: Short label
static char cLabel[33]={""};
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

#define VAR_LIST_tSearchdomain "tSearchdomain.uSearchdomain,tSearchdomain.cLabel,tSearchdomain.uOwner,tSearchdomain.uCreatedBy,tSearchdomain.uCreatedDate,tSearchdomain.uModBy,tSearchdomain.uModDate"

 //Local only
void Insert_tSearchdomain(void);
void Update_tSearchdomain(char *cRowid);
void ProcesstSearchdomainListVars(pentry entries[], int x);

 //In tSearchdomainfunc.h file included below
void ExtProcesstSearchdomainVars(pentry entries[], int x);
void ExttSearchdomainCommands(pentry entries[], int x);
void ExttSearchdomainButtons(void);
void ExttSearchdomainNavBar(void);
void ExttSearchdomainGetHook(entry gentries[], int x);
void ExttSearchdomainSelect(void);
void ExttSearchdomainSelectRow(void);
void ExttSearchdomainListSelect(void);
void ExttSearchdomainListFilter(void);
void ExttSearchdomainAuxTable(void);

#include "tsearchdomainfunc.h"

 //Table Variables Assignment Function
void ProcesstSearchdomainVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uSearchdomain"))
			sscanf(entries[i].val,"%u",&uSearchdomain);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
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
	ExtProcesstSearchdomainVars(entries,x);

}//ProcesstSearchdomainVars()


void ProcesstSearchdomainListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uSearchdomain);
                        guMode=2002;
                        tSearchdomain("");
                }
        }
}//void ProcesstSearchdomainListVars(pentry entries[], int x)


int tSearchdomainCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttSearchdomainCommands(entries,x);

	if(!strcmp(gcFunction,"tSearchdomainTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tSearchdomainList();
		}

		//Default
		ProcesstSearchdomainVars(entries,x);
		tSearchdomain("");
	}
	else if(!strcmp(gcFunction,"tSearchdomainList"))
	{
		ProcessControlVars(entries,x);
		ProcesstSearchdomainListVars(entries,x);
		tSearchdomainList();
	}

	return(0);

}//tSearchdomainCommands()


void tSearchdomain(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttSearchdomainSelectRow();
		else
			ExttSearchdomainSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetSearchdomain();
				unxsVZ("New tSearchdomain table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tSearchdomain WHERE uSearchdomain=%u"
						,uSearchdomain);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			sscanf(field[0],"%u",&uSearchdomain);
			sprintf(cLabel,"%.32s",field[1]);
			sscanf(field[2],"%u",&uOwner);
			sscanf(field[3],"%u",&uCreatedBy);
			sscanf(field[4],"%lu",&uCreatedDate);
			sscanf(field[5],"%u",&uModBy);
			sscanf(field[6],"%lu",&uModDate);
		}

	}//Internal Skip

	Header_ism3(":: Container Resolver Searchdomain",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tSearchdomainTools>");
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

        ExttSearchdomainButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tSearchdomain Record Data",100);

	if(guMode==2000 || guMode==2002)
		tSearchdomainInput(1);
	else
		tSearchdomainInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttSearchdomainAuxTable();

	Footer_ism3();

}//end of tSearchdomain();


void tSearchdomainInput(unsigned uMode)
{

//uSearchdomain
	OpenRow(LANG_FL_tSearchdomain_uSearchdomain,"black");
	printf("<input title='%s' type=text name=uSearchdomain value=%u size=16 maxlength=10 "
,LANG_FT_tSearchdomain_uSearchdomain,uSearchdomain);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uSearchdomain value=%u >\n",uSearchdomain);
	}
//cLabel
	OpenRow(LANG_FL_tSearchdomain_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tSearchdomain_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uOwner
	OpenRow(LANG_FL_tSearchdomain_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tSearchdomain_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tSearchdomain_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tSearchdomain_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tSearchdomain_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tSearchdomainInput(unsigned uMode)


void NewtSearchdomain(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uSearchdomain FROM tSearchdomain\
				WHERE uSearchdomain=%u"
							,uSearchdomain);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tSearchdomain("<blink>Record already exists");
		tSearchdomain(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tSearchdomain();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uSearchdomain=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tSearchdomain",uSearchdomain);
	unxsVZLog(uSearchdomain,"tSearchdomain","New");

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uSearchdomain);
	tSearchdomain(gcQuery);
	}

}//NewtSearchdomain(unsigned uMode)


void DeletetSearchdomain(void)
{
	sprintf(gcQuery,"DELETE FROM tSearchdomain WHERE uSearchdomain=%u AND ( uOwner=%u OR %u>9 )"
					,uSearchdomain,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tSearchdomain("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsVZLog(uSearchdomain,"tSearchdomain","Del");
		tSearchdomain(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsVZLog(uSearchdomain,"tSearchdomain","DelError");
		tSearchdomain(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetSearchdomain(void)


void Insert_tSearchdomain(void)
{
	sprintf(gcQuery,"INSERT INTO tSearchdomain SET uSearchdomain=%u,cLabel='%s',uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uSearchdomain
			,TextAreaSave(cLabel)
			,uOwner
			,uCreatedBy);
	MYSQL_RUN;

}//void Insert_tSearchdomain(void)


void Update_tSearchdomain(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tSearchdomain SET uSearchdomain=%u,cLabel='%s',uModBy=%u,"
			"uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uSearchdomain
			,TextAreaSave(cLabel)
			,uModBy
			,cRowid);
	MYSQL_RUN;

}//void Update_tSearchdomain(void)


void ModtSearchdomain(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tSearchdomain.uSearchdomain,"
				"tSearchdomain.uModDate"
				"FROM tSearchdomain,tClient"
				"WHERE tSearchdomain.uSearchdomain=%u"
				"AND tSearchdomain.uOwner=tClient.uClient"
				"AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uSearchdomain,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uSearchdomain,uModDate FROM tSearchdomain WHERE uSearchdomain=%u"
						,uSearchdomain);

	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tSearchdomain("<blink>Record does not exist");
	if(i<1) tSearchdomain(LANG_NBR_RECNOTEXIST);
	//if(i>1) tSearchdomain("<blink>Multiple rows!");
	if(i>1) tSearchdomain(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tSearchdomain(LANG_NBR_EXTMOD);

	Update_tSearchdomain(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tSearchdomain",uSearchdomain);
	unxsVZLog(uSearchdomain,"tSearchdomain","Mod");
	tSearchdomain(gcQuery);

}//ModtSearchdomain(void)


void tSearchdomainList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttSearchdomainListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tSearchdomainList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttSearchdomainListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uSearchdomain"
		"<td><font face=arial,helvetica color=white>cLabel"
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
		printf("<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tSearchdomain&uSearchdomain=%s>%s</a>"
			,field[0]
			,field[0]);
		printf("<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[1]
			,ForeignKey("tClient","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[3],NULL,10))
			,cBuf4
			,ForeignKey("tClient","cLabel",strtoul(field[5],NULL,10))
			,cBuf6);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tSearchdomainList()


void CreatetSearchdomain(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tSearchdomain ( "
			"uSearchdomain INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			"cLabel VARCHAR(32) NOT NULL DEFAULT '',"
			"uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner),"
			"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uDatacenter INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetSearchdomain()

