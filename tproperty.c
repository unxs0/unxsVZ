/*
FILE
	tProperty source code of unxsVZ.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
	$Id$
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tpropertyfunc.h while 
	RAD is still to be used.
AUTHOR/LEGAL
	(C) 2001-2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file.	
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uProperty: Primary Key
static unsigned uProperty=0;
//cName: Property Name
static char cName[33]={""};
//cValue: Value of property
static char *cValue={""};
//uType: Type of property
static unsigned uType=0;
static char cuTypePullDown[256]={""};
//uKey: Abstract FK
static unsigned uKey=0;
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

//flag for marking gMysqlUBC as active
static unsigned guUsingUBC=0;

#define VAR_LIST_tProperty "tProperty.uProperty,tProperty.cName,tProperty.cValue,tProperty.uType,tProperty.uKey,tProperty.uOwner,tProperty.uCreatedBy,tProperty.uCreatedDate,tProperty.uModBy,tProperty.uModDate"

 //Local only
void Insert_tProperty(void);
void Update_tProperty(char *cRowid);
void ProcesstPropertyListVars(pentry entries[], int x);

 //In tPropertyfunc.h file included below
void ExtProcesstPropertyVars(pentry entries[], int x);
void ExttPropertyCommands(pentry entries[], int x);
void ExttPropertyButtons(void);
void ExttPropertyNavBar(void);
void ExttPropertyGetHook(entry gentries[], int x);
void ExttPropertySelect(void);
void ExttPropertySelectRow(void);
void ExttPropertyListSelect(void);
void ExttPropertyListFilter(void);
void ExttPropertyAuxTable(void);

#include "tpropertyfunc.h"

 //Table Variables Assignment Function
void ProcesstPropertyVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uProperty"))
			sscanf(entries[i].val,"%u",&uProperty);
		else if(!strcmp(entries[i].name,"cName"))
			sprintf(cName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cValue"))
			cValue=entries[i].val;
		else if(!strcmp(entries[i].name,"uType"))
			sscanf(entries[i].val,"%u",&uType);
		else if(!strcmp(entries[i].name,"cuTypePullDown"))
		{
			sprintf(cuTypePullDown,"%.255s",entries[i].val);
			uType=ReadPullDown("tType","cLabel",cuTypePullDown);
		}
		else if(!strcmp(entries[i].name,"uKey"))
			sscanf(entries[i].val,"%u",&uKey);
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
	ExtProcesstPropertyVars(entries,x);

}//ProcesstPropertyVars()


void ProcesstPropertyListVars(pentry entries[], int x)
{
	char cLogfile[64]={"/tmp/unxsvzlog"};
	if((gLfp=fopen(cLogfile,"a"))==NULL)
        	tProperty("Could not open logfile");
        register int i;

        for(i=0;i<x;i++)
        {
		//debug
		//logfileLine("ProcesstPropertyListVars",entries[i].name);
		//logfileLine("ProcesstPropertyListVars",entries[i].val);
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uProperty);
                        guMode=2002;
                        tProperty("");
                }
		else if(!strcmp(entries[i].name,"htmlUBCDatacenterSelect") && entries[i].val[0]!='-') 
		{
			sprintf(cDatacenterSelect,"%.63s",entries[i].val);
			uTargetDatacenter=ReadPullDown("tDatacenter","cLabel",entries[i].val);
			sprintf(cuDatacenterSelect,"%u",uTargetDatacenter);
			if(uTargetDatacenter && ConnectToOptionalUBCDb(uTargetDatacenter,0))
				tProperty("ConnectToOptionalUBCDb() error");
			guUsingUBC=1;
		}
        }
}//void ProcesstPropertyListVars(pentry entries[], int x)


int tPropertyCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttPropertyCommands(entries,x);

	if(!strcmp(gcFunction,"tPropertyTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			ProcesstPropertyListVars(entries,x);
			tPropertyList();
		}

		//Default
		ProcesstPropertyVars(entries,x);
		tProperty("");
	}
	else if(!strcmp(gcFunction,"tPropertyList"))
	{
		//ProcessControlVars(entries,x);
		ProcesstPropertyListVars(entries,x);
		tPropertyList();
	}

	return(0);

}//tPropertyCommands()


void tProperty(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;
	MYSQL MysqlSave=gMysql;


	if(guUsingUBC)
		gMysql=gMysqlUBC;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttPropertySelectRow();
		else
			ExttPropertySelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetProperty();
				unxsVZ("New tProperty table created");
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
				//UBC safe
				sprintf(gcQuery,"SELECT _rowid FROM tProperty WHERE uProperty=%u"
						,uProperty);
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			sscanf(field[0],"%u",&uProperty);
			sprintf(cName,"%.32s",field[1]);
			cValue=field[2];
			sscanf(field[3],"%u",&uType);
			sscanf(field[4],"%u",&uKey);
			sscanf(field[5],"%u",&uOwner);
			sscanf(field[6],"%u",&uCreatedBy);
			sscanf(field[7],"%lu",&uCreatedDate);
			sscanf(field[8],"%u",&uModBy);
			sscanf(field[9],"%lu",&uModDate);

		}

	}//Internal Skip
	gMysql=MysqlSave;

	Header_ism3(":: Shared property table",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tPropertyTools>");
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

        ExttPropertyButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tProperty Record Data",100);

	if(guMode==2000 || guMode==2002)
		tPropertyInput(1);
	else
		tPropertyInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttPropertyAuxTable();

	Footer_ism3();

}//end of tProperty();


void tPropertyInput(unsigned uMode)
{

//uProperty
	OpenRow(LANG_FL_tProperty_uProperty,"black");
	printf("<input title='%s' type=text name=uProperty value=%u size=16 maxlength=10 ",
			LANG_FT_tProperty_uProperty,uProperty);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uProperty value=%u >\n",uProperty);
	}
//cName
	OpenRow(LANG_FL_tProperty_cName,"black");
	printf("<input title='%s' type=text name=cName value=\"%s\" size=40 maxlength=32 ",
			LANG_FT_tProperty_cName,EncodeDoubleQuotes(cName));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cName value=\"%s\">\n",EncodeDoubleQuotes(cName));
	}
//cValue
	char cPrivate[32]={"private"};
	char *cpValue;
	if(cName[0] && guPermLevel<10 && (strstr(cName,"passwd")||strstr(cName,"Passwd")))
		cpValue=cPrivate;
	else
		cpValue=cValue;
	OpenRow(LANG_FL_tProperty_cValue,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cValue ",LANG_FT_tProperty_cValue);
	if(guPermLevel>=0 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cpValue);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cpValue);
		printf("<input type=hidden name=cValue value=\"%s\" >\n",EncodeDoubleQuotes(cpValue));
	}
//uType
	OpenRow(LANG_FL_tProperty_uType,"black");
	if(guPermLevel>=12 && uMode)
		tTablePullDown("tType;cuTypePullDown","cLabel","cLabel",uType,1);
	else
		tTablePullDown("tType;cuTypePullDown","cLabel","cLabel",uType,0);
//uKey
	OpenRow(LANG_FL_tProperty_uKey,"black");
	printf("<input title='%s' type=text name=uKey value=%u size=16 maxlength=10 ",LANG_FT_tProperty_uKey,uKey);
	if(guPermLevel>=12 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uKey value=%u >\n",uKey);
	}
//uOwner
	OpenRow(LANG_FL_tProperty_uOwner,"black");
	if(guPermLevel>=20 && uMode)
		printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	else
		printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
//uCreatedBy
	OpenRow(LANG_FL_tProperty_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
		printf("%s<input type=hidden name=uCreatedBy value=%u >\n",
			ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	else
		printf("%s<input type=hidden name=uCreatedBy value=%u >\n",
			ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
//uCreatedDate
	OpenRow(LANG_FL_tProperty_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tProperty_uModBy,"black");
	if(guPermLevel>=20 && uMode)
		printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	else
		printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
//uModDate
	OpenRow(LANG_FL_tProperty_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tPropertyInput(unsigned uMode)


void NewtProperty(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	MYSQL gMysqlSave;
	if(guUsingUBC)
	{
		gMysqlSave=gMysql;
		gMysql=gMysqlUBC;
	}

	//Not UBC safe yet
	sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uProperty=%u",uProperty);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tProperty("<blink>Record already exists");
		tProperty(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tProperty();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uProperty=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tProperty",uProperty);
	if(guUsingUBC)
		gMysql=gMysqlSave;
	unxsVZLog(uProperty,"tProperty","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uProperty);
		tProperty(gcQuery);
	}

}//NewtProperty(unsigned uMode)


void DeletetProperty(void)
{

	MYSQL gMysqlSave;
	if(guUsingUBC)
	{
		gMysqlSave=gMysql;
		gMysql=gMysqlUBC;
	}

	sprintf(gcQuery,"DELETE FROM tProperty WHERE uProperty=%u AND ( uOwner=%u OR %u>9 )"
					,uProperty,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tProperty("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		if(guUsingUBC)
			gMysql=gMysqlSave;
		unxsVZLog(uProperty,"tProperty","Del");
		tProperty(LANG_NBR_RECDELETED);
	}
	else
	{
		if(guUsingUBC)
			gMysql=gMysqlSave;
		unxsVZLog(uProperty,"tProperty","DelError");
		tProperty(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetProperty(void)


void Insert_tProperty(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tProperty SET uProperty=%u,cName='%s',cValue='%s',uType=%u,uKey=%u,uOwner=%u,"
			"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uProperty
			,TextAreaSave(cName)
			,TextAreaSave(cValue)
			,uType
			,uKey
			,uOwner
			,uCreatedBy
			);

	MYSQL_RUN;

}//void Insert_tProperty(void)


void Update_tProperty(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tProperty SET uProperty=%u,cName='%s',cValue='%s',uType=%u,uKey=%u,uModBy=%u,"
			"uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uProperty
			,TextAreaSave(cName)
			,TextAreaSave(cValue)
			,uType
			,uKey
			,uModBy
			,cRowid);

	MYSQL_RUN;

}//void Update_tProperty(void)


void ModtProperty(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	MYSQL gMysqlSave;
	if(guUsingUBC)
	{
		gMysqlSave=gMysql;
		gMysql=gMysqlUBC;
	}

	sprintf(gcQuery,"SELECT uProperty,uModDate FROM tProperty WHERE uProperty=%u",uProperty);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tProperty("<blink>Record does not exist");
	if(i<1) tProperty(LANG_NBR_RECNOTEXIST);
	//if(i>1) tProperty("<blink>Multiple rows!");
	if(i>1) tProperty(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tProperty(LANG_NBR_EXTMOD);

	Update_tProperty(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tProperty",uProperty);
	if(guUsingUBC)
		gMysql=gMysqlSave;
	unxsVZLog(uProperty,"tProperty","Mod");
	tProperty(gcQuery);

}//ModtProperty(void)


void tPropertyList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL gMysqlSave;
	if(guUsingUBC)
	{
		gMysqlSave=gMysql;
		gMysql=gMysqlUBC;
	}

	ExttPropertyListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tPropertyList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttPropertyListFilter();

	printf("<input title='Enter numeric value or MySQL LIKE syntax filter string for selected field'"
		" type=text size=32 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);
	printf("<input title='Enter MySQL where condition. E.g. cValue LIKE `%%Node=rc7;%%`'"
		" type=text size=128 name=gcAuxFilter maxlength=255 value=\"%s\" >",gcAuxFilter);
	if(cDatacenterSelect[0] && cDatacenterSelect[0]!='-')
		printf("<input type=hidden name=htmlUBCDatacenterSelect value='%1$s'> %1$s\n",cDatacenterSelect);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uProperty"
		"<td><font face=arial,helvetica color=white>cName"
		"<td><font face=arial,helvetica color=white>cValue"
		"<td><font face=arial,helvetica color=white>uType"
		"<td><font face=arial,helvetica color=white>uKey"
		"<td><font face=arial,helvetica color=white>uOwner"
		"<td><font face=arial,helvetica color=white>uCreatedBy"
		"<td><font face=arial,helvetica color=white>uCreatedDate"
		"<td><font face=arial,helvetica color=white>uModBy"
		"<td><font face=arial,helvetica color=white>uModDate</tr>");

	mysql_data_seek(res,guStart-1);

	char cPrivate[32]={"private"};
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
		time_t luTime7=strtoul(field[7],NULL,10);
		char cBuf7[32];
		if(luTime7)
			ctime_r(&luTime7,cBuf7);
		else
			sprintf(cBuf7,"---");
		time_t luTime9=strtoul(field[9],NULL,10);
		char cBuf9[32];
		if(luTime9)
			ctime_r(&luTime9,cBuf9);
		else
			sprintf(cBuf9,"---");
		unsigned uKey=strtoul(field[3],NULL,10);
		char *cTable;
		switch(uKey)
		{
			case 3:
				cTable="tContainer&uContainer";
			break;
			case 2:
				cTable="tNode&uNode";
			break;
			default:
				cTable="tDatacenter&uDatacenter";
		}

		char cLinkExtra[32]={""};
		if(guUsingUBC)
		{
			gMysql=gMysqlSave;
			sprintf(cLinkExtra,"&cuDatacenterSelect=%s",cuDatacenterSelect);
		}
		char *cValue;
		if(guPermLevel<10 && (strstr(field[1],"passwd")||strstr(field[1],"Passwd")))
			cValue=cPrivate;
		else if(guPermLevel<12 && (strstr(field[1],"Root")||strstr(field[1],"root")))
			cValue=cPrivate;
		else
			cValue=field[2];
		printf("<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tProperty&uProperty=%s%s>%s</a>"
			"<td>%s"
			"<td><textarea rows=1 cols=80 disabled>%s</textarea>"
			"<td>%s"
			"<td><a class=darkLink href=unxsVZ.cgi?gcFunction=%s=%s>%s</a>"
			"<td>%s"
			"<td>%s"
			"<td>%s"
			"<td>%s"
			"<td>%s</tr>"
			,field[0],cLinkExtra,field[0]
			,field[1]
			,cValue
			,ForeignKey("tType","cLabel",uKey)
			,cTable,field[4],field[4]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,cBuf7
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[8],NULL,10))
			,cBuf9
				);
		if(guUsingUBC)
			gMysql=gMysqlUBC;

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tPropertyList()


void CreatetProperty(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tProperty ( "
			"uProperty INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			"cName VARCHAR(32) NOT NULL DEFAULT '', INDEX (cName),"
			"uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner),"
			"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"cValue TEXT NOT NULL DEFAULT '',"
			"uKey INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uKey),"
			"uType INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uType) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetProperty()

