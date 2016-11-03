/*
FILE
	tClientConfig source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tclientconfigfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uConfig: Primary Key
static unsigned uConfig=0;
//uGroup: Product configuration instance group index
static unsigned uGroup=0;
//uService: Service this parameter belongs to.
static unsigned uService=0;
//uParameter: Parameter name pointer
static unsigned uParameter=0;
//cValue: Parameter value
static char cValue[256]={""};
//uOwner: Record owner
static unsigned uOwner=0;
//uCreatedBy: uClient for last insert
static unsigned uCreatedBy=0;
#define ISM3FIELDS
//uCreatedDate: Unix seconds date last insert
static time_t uCreatedDate=0;
//uModBy: uClient for last update
static unsigned uModBy=0;
//uModDate: Unix seconds date last update
static time_t uModDate=0;



#define VAR_LIST_tClientConfig "tClientConfig.uConfig,tClientConfig.uGroup,tClientConfig.uService,tClientConfig.uParameter,tClientConfig.cValue,tClientConfig.uOwner,tClientConfig.uCreatedBy,tClientConfig.uCreatedDate,tClientConfig.uModBy,tClientConfig.uModDate"

 //Local only
void Insert_tClientConfig(void);
void Update_tClientConfig(char *cRowid);
void ProcesstClientConfigListVars(pentry entries[], int x);

 //In tClientConfigfunc.h file included below
void ExtProcesstClientConfigVars(pentry entries[], int x);
void ExttClientConfigCommands(pentry entries[], int x);
void ExttClientConfigButtons(void);
void ExttClientConfigNavBar(void);
void ExttClientConfigGetHook(entry gentries[], int x);
void ExttClientConfigSelect(void);
void ExttClientConfigSelectRow(void);
void ExttClientConfigListSelect(void);
void ExttClientConfigListFilter(void);
void ExttClientConfigAuxTable(void);

#include "tclientconfigfunc.h"

 //Table Variables Assignment Function
void ProcesstClientConfigVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uConfig"))
			sscanf(entries[i].val,"%u",&uConfig);
		else if(!strcmp(entries[i].name,"uGroup"))
			sscanf(entries[i].val,"%u",&uGroup);
		else if(!strcmp(entries[i].name,"uService"))
			sscanf(entries[i].val,"%u",&uService);
		else if(!strcmp(entries[i].name,"uParameter"))
			sscanf(entries[i].val,"%u",&uParameter);
		else if(!strcmp(entries[i].name,"cValue"))
			sprintf(cValue,"%.255s",entries[i].val);
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
	ExtProcesstClientConfigVars(entries,x);

}//ProcesstClientConfigVars()


void ProcesstClientConfigListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uConfig);
                        guMode=2002;
                        tClientConfig("");
                }
        }
}//void ProcesstClientConfigListVars(pentry entries[], int x)


int tClientConfigCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttClientConfigCommands(entries,x);

	if(!strcmp(gcFunction,"tClientConfigTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tClientConfigList();
		}

		//Default
		ProcesstClientConfigVars(entries,x);
		tClientConfig("");
	}
	else if(!strcmp(gcFunction,"tClientConfigList"))
	{
		ProcessControlVars(entries,x);
		ProcesstClientConfigListVars(entries,x);
		tClientConfigList();
	}

	return(0);

}//tClientConfigCommands()


void tClientConfig(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttClientConfigSelectRow();
		else
			ExttClientConfigSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetClientConfig();
				unxsISP("New tClientConfig table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tClientConfig WHERE uConfig=%u"
						,uConfig);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uConfig);
		sscanf(field[1],"%u",&uGroup);
		sscanf(field[2],"%u",&uService);
		sscanf(field[3],"%u",&uParameter);
		sprintf(cValue,"%.255s",field[4]);
		sscanf(field[5],"%u",&uOwner);
		sscanf(field[6],"%u",&uCreatedBy);
		sscanf(field[7],"%lu",&uCreatedDate);
		sscanf(field[8],"%u",&uModBy);
		sscanf(field[9],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tClientConfig",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tClientConfigTools>");
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

        ExttClientConfigButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tClientConfig Record Data",100);

	if(guMode==2000 || guMode==2002)
		tClientConfigInput(1);
	else
		tClientConfigInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttClientConfigAuxTable();

	Footer_ism3();

}//end of tClientConfig();


void tClientConfigInput(unsigned uMode)
{

//uConfig
	OpenRow(LANG_FL_tClientConfig_uConfig,"black");
	printf("<input title='%s' type=text name=uConfig value=%u size=16 maxlength=10 "
,LANG_FT_tClientConfig_uConfig,uConfig);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uConfig value=%u >\n",uConfig);
	}
//uGroup
	OpenRow(LANG_FL_tClientConfig_uGroup,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uGroup value=%u >\n",ForeignKey("tInstance","cLabel",uGroup),uGroup);
	}
	else
	{
	printf("%s<input type=hidden name=uGroup value=%u >\n",ForeignKey("tInstance","cLabel",uGroup),uGroup);
	}
//uService
	OpenRow(LANG_FL_tClientConfig_uService,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uService value=%u >\n",ForeignKey("tService","cLabel",uService),uService);
	}
	else
	{
	printf("%s<input type=hidden name=uService value=%u >\n",ForeignKey("tService","cLabel",uService),uService);
	}
//uParameter
	OpenRow(LANG_FL_tClientConfig_uParameter,"black");
	if(guPermLevel>=0 && uMode)
	{
	printf("%s<input type=hidden name=uParameter value=%u >\n",ForeignKey("tParameter","cParameter",uParameter),uParameter);
	}
	else
	{
	printf("%s<input type=hidden name=uParameter value=%u >\n",ForeignKey("tParameter","cParameter",uParameter),uParameter);
	}
//cValue
	OpenRow(LANG_FL_tClientConfig_cValue,"black");
	printf("<input title='%s' type=text name=cValue value=\"%s\" size=40 maxlength=255 "
,LANG_FT_tClientConfig_cValue,EncodeDoubleQuotes(cValue));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cValue value=\"%s\">\n",EncodeDoubleQuotes(cValue));
	}
//uOwner
	OpenRow(LANG_FL_tClientConfig_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tClientConfig_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tClientConfig_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tClientConfig_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tClientConfig_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tClientConfigInput(unsigned uMode)


void NewtClientConfig(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uConfig FROM tClientConfig\
				WHERE uConfig=%u"
							,uConfig);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tClientConfig("<blink>Record already exists");
		tClientConfig(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tClientConfig();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uConfig=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tClientConfig",uConfig);
	unxsISPLog(uConfig,"tClientConfig","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uConfig);
	tClientConfig(gcQuery);
	}

}//NewtClientConfig(unsigned uMode)


void DeletetClientConfig(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tClientConfig WHERE uConfig=%u AND ( uOwner=%u OR %u>9 )"
					,uConfig,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tClientConfig WHERE uConfig=%u"
					,uConfig);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tClientConfig("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uConfig,"tClientConfig","Del");
#endif
		tClientConfig(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uConfig,"tClientConfig","DelError");
#endif
		tClientConfig(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetClientConfig(void)


void Insert_tClientConfig(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tClientConfig SET uConfig=%u,uGroup=%u,uService=%u,uParameter=%u,cValue='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uConfig
			,uGroup
			,uService
			,uParameter
			,TextAreaSave(cValue)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tClientConfig(void)


void Update_tClientConfig(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tClientConfig SET uConfig=%u,uGroup=%u,uService=%u,uParameter=%u,cValue='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uConfig
			,uGroup
			,uService
			,uParameter
			,TextAreaSave(cValue)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tClientConfig(void)


void ModtClientConfig(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uConfig,uModDate FROM tClientConfig WHERE uConfig=%u"
						,uConfig);
#else
	sprintf(gcQuery,"SELECT uConfig FROM tClientConfig WHERE uConfig=%u"
						,uConfig);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tClientConfig("<blink>Record does not exist");
	if(i<1) tClientConfig(LANG_NBR_RECNOTEXIST);
	//if(i>1) tClientConfig("<blink>Multiple rows!");
	if(i>1) tClientConfig(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tClientConfig(LANG_NBR_EXTMOD);
#endif

	Update_tClientConfig(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tClientConfig",uConfig);
	unxsISPLog(uConfig,"tClientConfig","Mod");
#endif
	tClientConfig(gcQuery);

}//ModtClientConfig(void)


void tClientConfigList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttClientConfigListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tClientConfigList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttClientConfigListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uConfig<td><font face=arial,helvetica color=white>uGroup<td><font face=arial,helvetica color=white>uService<td><font face=arial,helvetica color=white>uParameter<td><font face=arial,helvetica color=white>cValue<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,ForeignKey("tInstance","cLabel",strtoul(field[1],NULL,10))
			,ForeignKey("tService","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tParameter","cParameter",strtoul(field[3],NULL,10))
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

}//tClientConfigList()


void CreatetClientConfig(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tClientConfig ( uService INT UNSIGNED NOT NULL DEFAULT 0,index (uService), cValue VARCHAR(255) NOT NULL DEFAULT '', uModDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uParameter INT UNSIGNED NOT NULL DEFAULT 0, uConfig INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uGroup INT UNSIGNED NOT NULL DEFAULT 0,index (uGroup) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetClientConfig()

