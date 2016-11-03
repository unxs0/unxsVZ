/*
FILE
	tConfig source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tconfigfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uConfig: Primary Key
static unsigned uConfig=0;
//uGroup: Service configuration instance group index
static unsigned uGroup=0;
//uParameter: Parameter name pointer
static unsigned uParameter=0;
//cValue: Parameter value
static char cValue[101]={""};
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



#define VAR_LIST_tConfig "tConfig.uConfig,tConfig.uGroup,tConfig.uParameter,tConfig.cValue,tConfig.uOwner,tConfig.uCreatedBy,tConfig.uCreatedDate,tConfig.uModBy,tConfig.uModDate"

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
		else if(!strcmp(entries[i].name,"uGroup"))
			sscanf(entries[i].val,"%u",&uGroup);
		else if(!strcmp(entries[i].name,"uParameter"))
			sscanf(entries[i].val,"%u",&uParameter);
		else if(!strcmp(entries[i].name,"cValue"))
			sprintf(cValue,"%.100s",entries[i].val);
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
				unxsISP("New tConfig table created");
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
		sscanf(field[2],"%u",&uParameter);
		sprintf(cValue,"%.100s",field[3]);
		sscanf(field[4],"%u",&uOwner);
		sscanf(field[5],"%u",&uCreatedBy);
		sscanf(field[6],"%lu",&uCreatedDate);
		sscanf(field[7],"%u",&uModBy);
		sscanf(field[8],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tConfig",1);
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
//uGroup
	OpenRow(LANG_FL_tConfig_uGroup,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uGroup value=%u >\n",ForeignKey("tService","cLabel",uGroup),uGroup);
	}
	else
	{
	printf("%s<input type=hidden name=uGroup value=%u >\n",ForeignKey("tService","cLabel",uGroup),uGroup);
	}
//uParameter
	OpenRow(LANG_FL_tConfig_uParameter,"black");
	if(guPermLevel>=0 && uMode)
	{
	printf("%s<input type=hidden name=uParameter value=%u >\n",ForeignKey("tParameter","cParameter",uParameter),uParameter);
	}
	else
	{
	printf("%s<input type=hidden name=uParameter value=%u >\n",ForeignKey("tParameter","cParameter",uParameter),uParameter);
	}
//cValue
	OpenRow(LANG_FL_tConfig_cValue,"black");
	printf("<input title='%s' type=text name=cValue value=\"%s\" size=40 maxlength=100 "
,LANG_FT_tConfig_cValue,EncodeDoubleQuotes(cValue));
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
	OpenRow(LANG_FL_tConfig_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tConfig_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
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
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
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

	sprintf(gcQuery,"SELECT uConfig FROM tConfig\
				WHERE uConfig=%u"
							,uConfig);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tConfig("<blink>Record already exists");
		tConfig(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tConfig();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uConfig=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tConfig",uConfig);
	unxsISPLog(uConfig,"tConfig","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uConfig);
	tConfig(gcQuery);
	}

}//NewtConfig(unsigned uMode)


void DeletetConfig(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tConfig WHERE uConfig=%u AND ( uOwner=%u OR %u>9 )"
					,uConfig,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tConfig WHERE uConfig=%u"
					,uConfig);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tConfig("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uConfig,"tConfig","Del");
#endif
		tConfig(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uConfig,"tConfig","DelError");
#endif
		tConfig(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetConfig(void)


void Insert_tConfig(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tConfig SET uConfig=%u,uGroup=%u,uParameter=%u,cValue='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uConfig
			,uGroup
			,uParameter
			,TextAreaSave(cValue)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tConfig(void)


void Update_tConfig(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tConfig SET uConfig=%u,uGroup=%u,uParameter=%u,cValue='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uConfig
			,uGroup
			,uParameter
			,TextAreaSave(cValue)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tConfig(void)


void ModtConfig(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uConfig,uModDate FROM tConfig WHERE uConfig=%u"
						,uConfig);
#else
	sprintf(gcQuery,"SELECT uConfig FROM tConfig WHERE uConfig=%u"
						,uConfig);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tConfig("<blink>Record does not exist");
	if(i<1) tConfig(LANG_NBR_RECNOTEXIST);
	//if(i>1) tConfig("<blink>Multiple rows!");
	if(i>1) tConfig(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tConfig(LANG_NBR_EXTMOD);
#endif

	Update_tConfig(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tConfig",uConfig);
	unxsISPLog(uConfig,"tConfig","Mod");
#endif
	tConfig(gcQuery);

}//ModtConfig(void)


void tConfigList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttConfigListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tConfigList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttConfigListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uConfig<td><font face=arial,helvetica color=white>uGroup<td><font face=arial,helvetica color=white>uParameter<td><font face=arial,helvetica color=white>cValue<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime6=strtoul(field[6],NULL,10);
		char cBuf6[32];
		if(luTime6)
			ctime_r(&luTime6,cBuf6);
		else
			sprintf(cBuf6,"---");
		time_t luTime8=strtoul(field[8],NULL,10);
		char cBuf8[32];
		if(luTime8)
			ctime_r(&luTime8,cBuf8);
		else
			sprintf(cBuf8,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,ForeignKey("tService","cLabel",strtoul(field[1],NULL,10))
			,ForeignKey("tParameter","cParameter",strtoul(field[2],NULL,10))
			,field[3]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[4],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,cBuf6
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[7],NULL,10))
			,cBuf8
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tConfigList()


void CreatetConfig(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tConfig ( uGroup INT UNSIGNED NOT NULL DEFAULT 0,index (uGroup), uConfig INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, uParameter INT UNSIGNED NOT NULL DEFAULT 0, uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cValue VARCHAR(100) NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetConfig()

