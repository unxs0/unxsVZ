/*
FILE
	tNAS source code of unxsRadius.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tnasfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uNAS: Primary Key
static unsigned uNAS=0;
//cLabel: Short label
static char cLabel[33]={""};
//cKey: Shared secret key
static char cKey[33]={""};
//cType: NAS/RAS Type
static char cType[33]={""};
//cIP: IP Number for remote management of NAS
static char cIP[16]={""};
//cLogin: Login for remote management of NAS
static char cLogin[33]={""};
//cPasswd: Passwd for remote management of NAS
static char cPasswd[33]={""};
//cInfo: Location and hardware info of NAS
static char *cInfo={""};
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



#define VAR_LIST_tNAS "tNAS.uNAS,tNAS.cLabel,tNAS.cKey,tNAS.cType,tNAS.cIP,tNAS.cLogin,tNAS.cPasswd,tNAS.cInfo,tNAS.uOwner,tNAS.uCreatedBy,tNAS.uCreatedDate,tNAS.uModBy,tNAS.uModDate"

 //Local only
void Insert_tNAS(void);
void Update_tNAS(char *cRowid);
void ProcesstNASListVars(pentry entries[], int x);

 //In tNASfunc.h file included below
void ExtProcesstNASVars(pentry entries[], int x);
void ExttNASCommands(pentry entries[], int x);
void ExttNASButtons(void);
void ExttNASNavBar(void);
void ExttNASGetHook(entry gentries[], int x);
void ExttNASSelect(void);
void ExttNASSelectRow(void);
void ExttNASListSelect(void);
void ExttNASListFilter(void);
void ExttNASAuxTable(void);

#include "tnasfunc.h"

 //Table Variables Assignment Function
void ProcesstNASVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uNAS"))
			sscanf(entries[i].val,"%u",&uNAS);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cKey"))
			sprintf(cKey,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cType"))
			sprintf(cType,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cIP"))
			sprintf(cIP,"%.15s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLogin"))
			sprintf(cLogin,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cInfo"))
			cInfo=entries[i].val;
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
	ExtProcesstNASVars(entries,x);

}//ProcesstNASVars()


void ProcesstNASListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uNAS);
                        guMode=2002;
                        tNAS("");
                }
        }
}//void ProcesstNASListVars(pentry entries[], int x)


int tNASCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttNASCommands(entries,x);

	if(!strcmp(gcFunction,"tNASTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tNASList();
		}

		//Default
		ProcesstNASVars(entries,x);
		tNAS("");
	}
	else if(!strcmp(gcFunction,"tNASList"))
	{
		ProcessControlVars(entries,x);
		ProcesstNASListVars(entries,x);
		tNASList();
	}

	return(0);

}//tNASCommands()


void tNAS(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttNASSelectRow();
		else
			ExttNASSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetNAS();
				unxsRadius("New tNAS table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tNAS WHERE uNAS=%u"
						,uNAS);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uNAS);
		sprintf(cLabel,"%.32s",field[1]);
		sprintf(cKey,"%.32s",field[2]);
		sprintf(cType,"%.32s",field[3]);
		sprintf(cIP,"%.15s",field[4]);
		sprintf(cLogin,"%.32s",field[5]);
		sprintf(cPasswd,"%.32s",field[6]);
		cInfo=field[7];
		sscanf(field[8],"%u",&uOwner);
		sscanf(field[9],"%u",&uCreatedBy);
		sscanf(field[10],"%lu",&uCreatedDate);
		sscanf(field[11],"%u",&uModBy);
		sscanf(field[12],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tNAS",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tNASTools>");
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

        ExttNASButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tNAS Record Data",100);

	if(guMode==2000 || guMode==2002)
		tNASInput(1);
	else
		tNASInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttNASAuxTable();

	Footer_ism3();

}//end of tNAS();


void tNASInput(unsigned uMode)
{

//uNAS
	OpenRow(LANG_FL_tNAS_uNAS,"black");
	printf("<input title='%s' type=text name=uNAS value=%u size=16 maxlength=10 "
,LANG_FT_tNAS_uNAS,uNAS);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uNAS value=%u >\n",uNAS);
	}
//cLabel
	OpenRow(LANG_FL_tNAS_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tNAS_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//cKey
	OpenRow(LANG_FL_tNAS_cKey,"black");
	printf("<input title='%s' type=text name=cKey value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tNAS_cKey,EncodeDoubleQuotes(cKey));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cKey value=\"%s\">\n",EncodeDoubleQuotes(cKey));
	}
//cType
	OpenRow(LANG_FL_tNAS_cType,"black");
	printf("<input title='%s' type=text name=cType value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tNAS_cType,EncodeDoubleQuotes(cType));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cType value=\"%s\">\n",EncodeDoubleQuotes(cType));
	}
//cIP
	OpenRow(LANG_FL_tNAS_cIP,"black");
	printf("<input title='%s' type=text name=cIP value=\"%s\" size=40 maxlength=15 "
,LANG_FT_tNAS_cIP,EncodeDoubleQuotes(cIP));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cIP value=\"%s\">\n",EncodeDoubleQuotes(cIP));
	}
//cLogin
	OpenRow(LANG_FL_tNAS_cLogin,"black");
	printf("<input title='%s' type=text name=cLogin value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tNAS_cLogin,EncodeDoubleQuotes(cLogin));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLogin value=\"%s\">\n",EncodeDoubleQuotes(cLogin));
	}
//cPasswd
	OpenRow(LANG_FL_tNAS_cPasswd,"black");
	printf("<input title='%s' type=text name=cPasswd value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tNAS_cPasswd,EncodeDoubleQuotes(cPasswd));
	if(guPermLevel>=10 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cPasswd value=\"%s\">\n",EncodeDoubleQuotes(cPasswd));
	}
//cInfo
	OpenRow(LANG_FL_tNAS_cInfo,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cInfo "
,LANG_FT_tNAS_cInfo);
	if(guPermLevel>=10 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cInfo);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cInfo);
		printf("<input type=hidden name=cInfo value=\"%s\" >\n",EncodeDoubleQuotes(cInfo));
	}
//uOwner
	OpenRow(LANG_FL_tNAS_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tNAS_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tNAS_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tNAS_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tNAS_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tNASInput(unsigned uMode)


void NewtNAS(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uNAS FROM tNAS WHERE uNAS=%u",uNAS);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tNAS("<blink>Record already exists");
		tNAS(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tNAS();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uNAS=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tNAS",uNAS);
	unxsRadiusLog(uNAS,"tNAS","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uNAS);
		tNAS(gcQuery);
	}

}//NewtNAS(unsigned uMode)


void DeletetNAS(void)
{
	sprintf(gcQuery,"DELETE FROM tNAS WHERE uNAS=%u",uNAS);
	macro_mySQLQueryHTMLError;

	//tNAS("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsRadiusLog(uNAS,"tNAS","Del");
		tNAS(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsRadiusLog(uNAS,"tNAS","DelError");
		tNAS(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetNAS(void)


void Insert_tNAS(void)
{
	sprintf(gcQuery,"INSERT INTO tNAS SET uNAS=%u,cLabel='%s',cKey='%s',cType='%s',cIP='%s',"
			"cLogin='%s',cPasswd='%s',cInfo='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uNAS
				,TextAreaSave(cLabel)
				,TextAreaSave(cKey)
				,TextAreaSave(cType)
				,TextAreaSave(cIP)
				,TextAreaSave(cLogin)
				,TextAreaSave(cPasswd)
				,TextAreaSave(cInfo)
				,uOwner
				,uCreatedBy);
	macro_mySQLQueryHTMLError;
}//void Insert_tNAS(void)


void Update_tNAS(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tNAS SET uNAS=%u,cLabel='%s',cKey='%s',cType='%s',cIP='%s',"
			"cLogin='%s',cPasswd='%s',cInfo='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
				uNAS
				,TextAreaSave(cLabel)
				,TextAreaSave(cKey)
				,TextAreaSave(cType)
				,TextAreaSave(cIP)
				,TextAreaSave(cLogin)
				,TextAreaSave(cPasswd)
				,TextAreaSave(cInfo)
				,uModBy
				,cRowid);
	macro_mySQLQueryHTMLError;
}//void Update_tNAS(void)


void ModtNAS(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uNAS,uModDate FROM tNAS WHERE uNAS=%u",uNAS);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tNAS("<blink>Record does not exist");
	if(i<1) tNAS(LANG_NBR_RECNOTEXIST);
	//if(i>1) tNAS("<blink>Multiple rows!");
	if(i>1) tNAS(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tNAS(LANG_NBR_EXTMOD);

	Update_tNAS(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tNAS",uNAS);
	unxsRadiusLog(uNAS,"tNAS","Mod");
	tNAS(gcQuery);

}//ModtNAS(void)


void tNASList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttNASListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tNASList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttNASListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uNAS<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>cKey<td><font face=arial,helvetica color=white>cType<td><font face=arial,helvetica color=white>cIP<td><font face=arial,helvetica color=white>cLogin<td><font face=arial,helvetica color=white>cPasswd<td><font face=arial,helvetica color=white>cInfo<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime10=strtoul(field[10],NULL,10);
		char cBuf10[32];
		if(luTime10)
			ctime_r(&luTime10,cBuf10);
		else
			sprintf(cBuf10,"---");
		time_t luTime12=strtoul(field[12],NULL,10);
		char cBuf12[32];
		if(luTime12)
			ctime_r(&luTime12,cBuf12);
		else
			sprintf(cBuf12,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,field[4]
			,field[5]
			,field[6]
			,field[7]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[8],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[9],NULL,10))
			,cBuf10
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[11],NULL,10))
			,cBuf12
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tNASList()


void CreatetNAS(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tNAS ( uNAS INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cKey VARCHAR(32) NOT NULL DEFAULT '', cType VARCHAR(32) NOT NULL DEFAULT '', cIP VARCHAR(15) NOT NULL DEFAULT '', cLogin VARCHAR(32) NOT NULL DEFAULT '', cPasswd VARCHAR(32) NOT NULL DEFAULT '', cInfo TEXT NOT NULL DEFAULT '' )");
	macro_mySQLQueryHTMLError;
}//CreatetNAS()

