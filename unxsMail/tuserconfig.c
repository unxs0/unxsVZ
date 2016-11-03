/*
FILE
	tUserConfig source code of unxsMail.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tuserconfigfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uUserConfig: Primary Key
static unsigned uUserConfig=0;
//cLabel: Short label
static char cLabel[33]={""};
//uUser: tUser.uUser
static unsigned uUser=0;
//uConfigSpec: Index into tConfigSpec
static unsigned uConfigSpec=0;
static char cuConfigSpecPullDown[256]={""};
//cConfig: Configuration file or section
static char *cConfig={""};
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



#define VAR_LIST_tUserConfig "tUserConfig.uUserConfig,tUserConfig.cLabel,tUserConfig.uUser,tUserConfig.uConfigSpec,tUserConfig.cConfig,tUserConfig.uOwner,tUserConfig.uCreatedBy,tUserConfig.uCreatedDate,tUserConfig.uModBy,tUserConfig.uModDate"

 //Local only
void Insert_tUserConfig(void);
void Update_tUserConfig(char *cRowid);
void ProcesstUserConfigListVars(pentry entries[], int x);

 //In tUserConfigfunc.h file included below
void ExtProcesstUserConfigVars(pentry entries[], int x);
void ExttUserConfigCommands(pentry entries[], int x);
void ExttUserConfigButtons(void);
void ExttUserConfigNavBar(void);
void ExttUserConfigGetHook(entry gentries[], int x);
void ExttUserConfigSelect(void);
void ExttUserConfigSelectRow(void);
void ExttUserConfigListSelect(void);
void ExttUserConfigListFilter(void);
void ExttUserConfigAuxTable(void);

#include "tuserconfigfunc.h"

 //Table Variables Assignment Function
void ProcesstUserConfigVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uUserConfig"))
			sscanf(entries[i].val,"%u",&uUserConfig);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uUser"))
			sscanf(entries[i].val,"%u",&uUser);
		else if(!strcmp(entries[i].name,"uConfigSpec"))
			sscanf(entries[i].val,"%u",&uConfigSpec);
		else if(!strcmp(entries[i].name,"cuConfigSpecPullDown"))
		{
			sprintf(cuConfigSpecPullDown,"%.255s",entries[i].val);
			uConfigSpec=ReadPullDown("tConfigSpec","cLabel",cuConfigSpecPullDown);
		}
		else if(!strcmp(entries[i].name,"cConfig"))
			cConfig=entries[i].val;
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
	ExtProcesstUserConfigVars(entries,x);

}//ProcesstUserConfigVars()


void ProcesstUserConfigListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uUserConfig);
                        guMode=2002;
                        tUserConfig("");
                }
        }
}//void ProcesstUserConfigListVars(pentry entries[], int x)


int tUserConfigCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttUserConfigCommands(entries,x);

	if(!strcmp(gcFunction,"tUserConfigTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tUserConfigList();
		}

		//Default
		ProcesstUserConfigVars(entries,x);
		tUserConfig("");
	}
	else if(!strcmp(gcFunction,"tUserConfigList"))
	{
		ProcessControlVars(entries,x);
		ProcesstUserConfigListVars(entries,x);
		tUserConfigList();
	}

	return(0);

}//tUserConfigCommands()


void tUserConfig(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttUserConfigSelectRow();
		else
			ExttUserConfigSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetUserConfig();
				unxsMail("New tUserConfig table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tUserConfig WHERE uUserConfig=%u"
						,uUserConfig);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uUserConfig);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uUser);
		sscanf(field[3],"%u",&uConfigSpec);
		cConfig=field[4];
		sscanf(field[5],"%u",&uOwner);
		sscanf(field[6],"%u",&uCreatedBy);
		sscanf(field[7],"%lu",&uCreatedDate);
		sscanf(field[8],"%u",&uModBy);
		sscanf(field[9],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tUserConfig",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tUserConfigTools>");
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

        ExttUserConfigButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tUserConfig Record Data",100);

	if(guMode==2000 || guMode==2002)
		tUserConfigInput(1);
	else
		tUserConfigInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttUserConfigAuxTable();

	Footer_ism3();

}//end of tUserConfig();


void tUserConfigInput(unsigned uMode)
{

//uUserConfig
	OpenRow(LANG_FL_tUserConfig_uUserConfig,"black");
	printf("<input title='%s' type=text name=uUserConfig value=%u size=16 maxlength=10 "
,LANG_FT_tUserConfig_uUserConfig,uUserConfig);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uUserConfig value=%u >\n",uUserConfig);
	}
//cLabel
	OpenRow(LANG_FL_tUserConfig_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tUserConfig_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uUser
	OpenRow(LANG_FL_tUserConfig_uUser,"black");
	if(guPermLevel>=10 && uMode && guMode!=2002)
	{
	printf("<input title='Enter tUser.uUser code only' type=text size=16 maxlength=16 name=uUser value=%u >\n",uUser);
	}
	else
	{
	printf("%s<input type=hidden name=uUser value=%u >\n",ForeignKey("tUser","cLogin",uUser),uUser);
	}
//uConfigSpec
	OpenRow(LANG_FL_tUserConfig_uConfigSpec,"black");
	if(guPermLevel>=10 && uMode && guMode!=2002)
		tTablePullDown("tConfigSpec;cuConfigSpecPullDown","cLabel","cLabel",uConfigSpec,1);
	else
		tTablePullDown("tConfigSpec;cuConfigSpecPullDown","cLabel","cLabel",uConfigSpec,0);
//cConfig
	OpenRow(LANG_FL_tUserConfig_cConfig,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cConfig "
,LANG_FT_tUserConfig_cConfig);
	if( (guPermLevel>=20 && uMode) || (guPermLevel>=10 && guMode==3002))
	{
		printf(">%s</textarea></td></tr>\n",cConfig);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cConfig);
		printf("<input type=hidden name=cConfig value=\"%s\" >\n",EncodeDoubleQuotes(cConfig));
	}
//uOwner
	OpenRow(LANG_FL_tUserConfig_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tUserConfig_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tUserConfig_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tUserConfig_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tUserConfig_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tUserConfigInput(unsigned uMode)


void NewtUserConfig(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uUserConfig FROM tUserConfig\
				WHERE uUserConfig=%u"
							,uUserConfig);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tUserConfig("<blink>Record already exists");
		tUserConfig(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tUserConfig();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uUserConfig=mysql_insert_id(&gMysql);

	uCreatedDate=luGetCreatedDate("tUserConfig",uUserConfig);
	unxsMailLog(uUserConfig,"tUserConfig","New");


	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uUserConfig);
	tUserConfig(gcQuery);
	}

}//NewtUserConfig(unsigned uMode)


void DeletetUserConfig(void)
{

	sprintf(gcQuery,"DELETE FROM tUserConfig WHERE uUserConfig=%u AND ( uOwner=%u OR %u>9 )"
					,uUserConfig,guLoginClient,guPermLevel);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tUserConfig("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{

		unxsMailLog(uUserConfig,"tUserConfig","Del");

		tUserConfig(LANG_NBR_RECDELETED);
	}
	else
	{

		unxsMailLog(uUserConfig,"tUserConfig","DelError");

		tUserConfig(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetUserConfig(void)


void Insert_tUserConfig(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tUserConfig SET uUserConfig=%u,cLabel='%s',uUser=%u,uConfigSpec=%u,cConfig='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uUserConfig
			,TextAreaSave(cLabel)
			,uUser
			,uConfigSpec
			,TextAreaSave(cConfig)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tUserConfig(void)


void Update_tUserConfig(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tUserConfig SET uUserConfig=%u,cLabel='%s',uUser=%u,uConfigSpec=%u,cConfig='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uUserConfig
			,TextAreaSave(cLabel)
			,uUser
			,uConfigSpec
			,TextAreaSave(cConfig)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tUserConfig(void)


void ModtUserConfig(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;

	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uUserConfig,uModDate FROM tUserConfig WHERE uUserConfig=%u"
						,uUserConfig);

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tUserConfig("<blink>Record does not exist");
	if(i<1) tUserConfig(LANG_NBR_RECNOTEXIST);
	//if(i>1) tUserConfig("<blink>Multiple rows!");
	if(i>1) tUserConfig(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);

	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tUserConfig(LANG_NBR_EXTMOD);


	Update_tUserConfig(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);

	uModDate=luGetModDate("tUserConfig",uUserConfig);
	unxsMailLog(uUserConfig,"tUserConfig","Mod");

	tUserConfig(gcQuery);

}//ModtUserConfig(void)


void tUserConfigList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttUserConfigListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tUserConfigList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttUserConfigListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uUserConfig<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uUser<td><font face=arial,helvetica color=white>uConfigSpec<td><font face=arial,helvetica color=white>cConfig<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tUser","cLogin",strtoul(field[2],NULL,10))
			,ForeignKey("tConfigSpec","cLabel",strtoul(field[3],NULL,10))
			,field[4]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[6],NULL,10))
			,cBuf7
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[8],NULL,10))
			,cBuf9
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tUserConfigList()


void CreatetUserConfig(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tUserConfig ( uUserConfig INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cConfig TEXT NOT NULL DEFAULT '', uUser INT UNSIGNED NOT NULL DEFAULT 0,index (uUser), uConfigSpec INT UNSIGNED NOT NULL DEFAULT 0,index (uConfigSpec) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetUserConfig()

