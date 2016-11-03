/*
FILE
	tDatacenter source code of unxsVZ.cgi
	Built by mysqlRAD2.cgi 
	(C) 2001-2016 Gary Wallis for Unixservice, LLC.
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tdatacenterfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uDatacenter: Primary Key
static unsigned uDatacenter=0;
//cLabel: Short label
static char cLabel[33]={""};
//uStatus: Status of Datacenter
static unsigned uStatus=0;
static char cuStatusPullDown[256]={""};
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
//git describe version info
static char *cGitVersion="GitVersion:"GitVersion;



#define VAR_LIST_tDatacenter "tDatacenter.uDatacenter,tDatacenter.cLabel,tDatacenter.uStatus,tDatacenter.uOwner,tDatacenter.uCreatedBy,tDatacenter.uCreatedDate,tDatacenter.uModBy,tDatacenter.uModDate"

 //Local only
void Insert_tDatacenter(void);
void Update_tDatacenter(char *cRowid);
void ProcesstDatacenterListVars(pentry entries[], int x);

 //In tDatacenterfunc.h file included below
void ExtProcesstDatacenterVars(pentry entries[], int x);
void ExttDatacenterCommands(pentry entries[], int x);
void ExttDatacenterButtons(void);
void ExttDatacenterNavBar(void);
void ExttDatacenterGetHook(entry gentries[], int x);
void ExttDatacenterSelect(void);
void ExttDatacenterSelectRow(void);
void ExttDatacenterListSelect(void);
void ExttDatacenterListFilter(void);
void ExttDatacenterAuxTable(void);

#include "tdatacenterfunc.h"

 //Table Variables Assignment Function
void ProcesstDatacenterVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uDatacenter"))
			sscanf(entries[i].val,"%u",&uDatacenter);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uStatus"))
			sscanf(entries[i].val,"%u",&uStatus);
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
		else if(!strcmp(entries[i].name,"cuStatusPullDown"))
		{
			sprintf(cuStatusPullDown,"%.255s",entries[i].val);
			uStatus=ReadPullDown("tStatus","cLabel",cuStatusPullDown);
		}

	}

	//After so we can overwrite form data if needed.
	ExtProcesstDatacenterVars(entries,x);

}//ProcesstDatacenterVars()


void ProcesstDatacenterListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uDatacenter);
                        guMode=2002;
                        tDatacenter("");
                }
        }
}//void ProcesstDatacenterListVars(pentry entries[], int x)


int tDatacenterCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttDatacenterCommands(entries,x);

	if(!strcmp(gcFunction,"tDatacenterTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tDatacenterList();
		}

		//Default
		ProcesstDatacenterVars(entries,x);
		tDatacenter("");
	}
	else if(!strcmp(gcFunction,"tDatacenterList"))
	{
		ProcessControlVars(entries,x);
		ProcesstDatacenterListVars(entries,x);
		tDatacenterList();
	}

	return(0);

}//tDatacenterCommands()


void tDatacenter(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttDatacenterSelectRow();
		else
			ExttDatacenterSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetDatacenter();
				unxsVZ("New tDatacenter table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tDatacenter WHERE uDatacenter=%u"
						,uDatacenter);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uDatacenter);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uStatus);
		sscanf(field[3],"%u",&uOwner);
		sscanf(field[4],"%u",&uCreatedBy);
		sscanf(field[5],"%lu",&uCreatedDate);
		sscanf(field[6],"%u",&uModBy);
		sscanf(field[7],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tDatacenter",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tDatacenterTools>");
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

        ExttDatacenterButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tDatacenter Record Data",100);

	if(guMode==2000 || guMode==2002)
		tDatacenterInput(1);
	else
		tDatacenterInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttDatacenterAuxTable();

	Footer_ism3();

}//end of tDatacenter();


void tDatacenterInput(unsigned uMode)
{
	if(uDatacenter && uStatus!=uOFFLINE)
	{
		char cConfigBuffer[256]={""};
		char cConfigBuffer2[256]={"/traffic/datacenter.html"};
		char cGetWhat[64];

		sprintf(cGetWhat,"%.32s-cDatacenterTrafficPNG",cLabel);
		GetConfiguration(cGetWhat,cConfigBuffer,0,0,0,0);
		if(!cConfigBuffer[0])
			GetConfiguration("cDatacenterTrafficDirURL",cConfigBuffer,0,0,0,0);
		if(cConfigBuffer[0])
		{
			sprintf(cGetWhat,"%.32s-cDatacenterHtmlURL",cLabel);
			GetConfiguration(cGetWhat,cConfigBuffer2,0,0,0,0);
	
			OpenRow("Graph","black");
			printf("<a href=%s><img src=%s border=0 onerror=\"this.src='/images/noimage.jpg'\"></a>\n",cConfigBuffer2,cConfigBuffer);
			printf("</td></tr>\n");
		}
	}

//uContainer

//uDatacenter
	OpenRow(LANG_FL_tDatacenter_uDatacenter,"black");
	printf("<input title='%s' type=text name=uDatacenter value=%u size=16 maxlength=10 "
,LANG_FT_tDatacenter_uDatacenter,uDatacenter);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uDatacenter value=%u >\n",uDatacenter);
	}
//cLabel
	OpenRow(LANG_FL_tDatacenter_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tDatacenter_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uStatus
	OpenRow(LANG_FL_tDatacenter_uStatus,"black");
	if(guPermLevel>=12 && uMode)
		tTablePullDown("tStatus;cuStatusPullDown","cLabel","cLabel",uStatus,1);
	else
		tTablePullDown("tStatus;cuStatusPullDown","cLabel","cLabel",uStatus,0);
//uOwner
	OpenRow(LANG_FL_tDatacenter_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tDatacenter_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tDatacenter_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tDatacenter_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tDatacenter_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tDatacenterInput(unsigned uMode)


void NewtDatacenter(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uDatacenter FROM tDatacenter WHERE uDatacenter=%u"
							,uDatacenter);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tDatacenter("<blink>Record already exists");
		tDatacenter(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tDatacenter();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uDatacenter=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tDatacenter",uDatacenter);
	unxsVZLog(uDatacenter,"tDatacenter","New");

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uDatacenter);
	tDatacenter(gcQuery);
	}

}//NewtDatacenter(unsigned uMode)


void DeletetDatacenter(void)
{
	sprintf(gcQuery,"DELETE FROM tDatacenter WHERE uDatacenter=%u AND ( uOwner=%u OR %u>9 )"
					,uDatacenter,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tDatacenter("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsVZLog(uDatacenter,"tDatacenter","Del");
		tDatacenter(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsVZLog(uDatacenter,"tDatacenter","DelError");
		tDatacenter(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetDatacenter(void)


void Insert_tDatacenter(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tDatacenter SET uDatacenter=%u,cLabel='%s',uStatus=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uDatacenter
			,TextAreaSave(cLabel)
			,uStatus
			,uOwner
			,uCreatedBy
			);

	MYSQL_RUN;

}//void Insert_tDatacenter(void)


void Update_tDatacenter(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tDatacenter SET uDatacenter=%u,cLabel='%s',uStatus=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uDatacenter
			,TextAreaSave(cLabel)
			,uStatus
			,uModBy
			,cRowid);

	MYSQL_RUN;

}//void Update_tDatacenter(void)


void ModtDatacenter(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	sprintf(gcQuery,"SELECT uDatacenter,uModDate FROM tDatacenter WHERE uDatacenter=%u"
						,uDatacenter);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tDatacenter("<blink>Record does not exist");
	if(i<1) tDatacenter(LANG_NBR_RECNOTEXIST);
	//if(i>1) tDatacenter("<blink>Multiple rows!");
	if(i>1) tDatacenter(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tDatacenter(LANG_NBR_EXTMOD);

	Update_tDatacenter(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tDatacenter",uDatacenter);
	unxsVZLog(uDatacenter,"tDatacenter","Mod");
	tDatacenter(gcQuery);

}//ModtDatacenter(void)


void tDatacenterList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttDatacenterListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tDatacenterList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttDatacenterListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uDatacenter<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uStatus<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		//printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
		printf("<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tDatacenter&uDatacenter=%s>%s",field[0],field[1]);
		printf("<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[1]
			,ForeignKey("tStatus","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[3],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[4],NULL,10))
			,cBuf5
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,cBuf7
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tDatacenterList()


void CreatetDatacenter(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tDatacenter ("
			" uDatacenter INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			" cLabel VARCHAR(32) NOT NULL DEFAULT '',unique (cLabel),"
			" uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner),"
			" uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			" uStatus INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetDatacenter()

