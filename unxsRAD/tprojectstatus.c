/*
FILE
	tProjectStatus source code of unxsRAD.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2009 for Unixservice
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tprojectstatusfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uStatus: Primary Key
static unsigned uStatus=0;
//cLabel: Short label
static char cLabel[33]={""};
//uLockMain: Do not replace main.c on build
static unsigned uLockMain=0;
static char cYesNouLockMain[32]={""};
//uLockMainFunc: Do not replace mainfunc.h on build
static unsigned uLockMainFunc=0;
static char cYesNouLockMainFunc[32]={""};
//uLockModule: Do not replace table .c file on build
static unsigned uLockModule=0;
static char cYesNouLockModule[32]={""};
//uLockModuleFunc: Do not replace table func.h file on build
static unsigned uLockModuleFunc=0;
static char cYesNouLockModuleFunc[32]={""};
//uLockMakefile: Do not replace makefile on build
static unsigned uLockMakefile=0;
static char cYesNouLockMakefile[32]={""};
//uLockInclude: Do not replace mysqlrad.h on build
static unsigned uLockInclude=0;
static char cYesNouLockInclude[32]={""};
//uLockLanguage: Do not replace languagex.h files on build
static unsigned uLockLanguage=0;
static char cYesNouLockLanguage[32]={""};
//uLockCapFiles: Do not replace INSTALL and similar files on build
static unsigned uLockCapFiles=0;
static char cYesNouLockCapFiles[32]={""};
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



#define VAR_LIST_tProjectStatus "tProjectStatus.uStatus,tProjectStatus.cLabel,tProjectStatus.uLockMain,tProjectStatus.uLockMainFunc,tProjectStatus.uLockModule,tProjectStatus.uLockModuleFunc,tProjectStatus.uLockMakefile,tProjectStatus.uLockInclude,tProjectStatus.uLockLanguage,tProjectStatus.uLockCapFiles,tProjectStatus.uOwner,tProjectStatus.uCreatedBy,tProjectStatus.uCreatedDate,tProjectStatus.uModBy,tProjectStatus.uModDate"

 //Local only
void Insert_tProjectStatus(void);
void Update_tProjectStatus(char *cRowid);
void ProcesstProjectStatusListVars(pentry entries[], int x);

 //In tProjectStatusfunc.h file included below
void ExtProcesstProjectStatusVars(pentry entries[], int x);
void ExttProjectStatusCommands(pentry entries[], int x);
void ExttProjectStatusButtons(void);
void ExttProjectStatusNavBar(void);
void ExttProjectStatusGetHook(entry gentries[], int x);
void ExttProjectStatusSelect(void);
void ExttProjectStatusSelectRow(void);
void ExttProjectStatusListSelect(void);
void ExttProjectStatusListFilter(void);
void ExttProjectStatusAuxTable(void);

#include "tprojectstatusfunc.h"

 //Table Variables Assignment Function
void ProcesstProjectStatusVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uStatus"))
			sscanf(entries[i].val,"%u",&uStatus);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uLockMain"))
			sscanf(entries[i].val,"%u",&uLockMain);
		else if(!strcmp(entries[i].name,"cYesNouLockMain"))
		{
			sprintf(cYesNouLockMain,"%.31s",entries[i].val);
			uLockMain=ReadYesNoPullDown(cYesNouLockMain);
		}
		else if(!strcmp(entries[i].name,"uLockMainFunc"))
			sscanf(entries[i].val,"%u",&uLockMainFunc);
		else if(!strcmp(entries[i].name,"cYesNouLockMainFunc"))
		{
			sprintf(cYesNouLockMainFunc,"%.31s",entries[i].val);
			uLockMainFunc=ReadYesNoPullDown(cYesNouLockMainFunc);
		}
		else if(!strcmp(entries[i].name,"uLockModule"))
			sscanf(entries[i].val,"%u",&uLockModule);
		else if(!strcmp(entries[i].name,"cYesNouLockModule"))
		{
			sprintf(cYesNouLockModule,"%.31s",entries[i].val);
			uLockModule=ReadYesNoPullDown(cYesNouLockModule);
		}
		else if(!strcmp(entries[i].name,"uLockModuleFunc"))
			sscanf(entries[i].val,"%u",&uLockModuleFunc);
		else if(!strcmp(entries[i].name,"cYesNouLockModuleFunc"))
		{
			sprintf(cYesNouLockModuleFunc,"%.31s",entries[i].val);
			uLockModuleFunc=ReadYesNoPullDown(cYesNouLockModuleFunc);
		}
		else if(!strcmp(entries[i].name,"uLockMakefile"))
			sscanf(entries[i].val,"%u",&uLockMakefile);
		else if(!strcmp(entries[i].name,"cYesNouLockMakefile"))
		{
			sprintf(cYesNouLockMakefile,"%.31s",entries[i].val);
			uLockMakefile=ReadYesNoPullDown(cYesNouLockMakefile);
		}
		else if(!strcmp(entries[i].name,"uLockInclude"))
			sscanf(entries[i].val,"%u",&uLockInclude);
		else if(!strcmp(entries[i].name,"cYesNouLockInclude"))
		{
			sprintf(cYesNouLockInclude,"%.31s",entries[i].val);
			uLockInclude=ReadYesNoPullDown(cYesNouLockInclude);
		}
		else if(!strcmp(entries[i].name,"uLockLanguage"))
			sscanf(entries[i].val,"%u",&uLockLanguage);
		else if(!strcmp(entries[i].name,"cYesNouLockLanguage"))
		{
			sprintf(cYesNouLockLanguage,"%.31s",entries[i].val);
			uLockLanguage=ReadYesNoPullDown(cYesNouLockLanguage);
		}
		else if(!strcmp(entries[i].name,"uLockCapFiles"))
			sscanf(entries[i].val,"%u",&uLockCapFiles);
		else if(!strcmp(entries[i].name,"cYesNouLockCapFiles"))
		{
			sprintf(cYesNouLockCapFiles,"%.31s",entries[i].val);
			uLockCapFiles=ReadYesNoPullDown(cYesNouLockCapFiles);
		}
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
	ExtProcesstProjectStatusVars(entries,x);

}//ProcesstProjectStatusVars()


void ProcesstProjectStatusListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uStatus);
                        guMode=2002;
                        tProjectStatus("");
                }
        }
}//void ProcesstProjectStatusListVars(pentry entries[], int x)


int tProjectStatusCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttProjectStatusCommands(entries,x);

	if(!strcmp(gcFunction,"tProjectStatusTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tProjectStatusList();
		}

		//Default
		ProcesstProjectStatusVars(entries,x);
		tProjectStatus("");
	}
	else if(!strcmp(gcFunction,"tProjectStatusList"))
	{
		ProcessControlVars(entries,x);
		ProcesstProjectStatusListVars(entries,x);
		tProjectStatusList();
	}

	return(0);

}//tProjectStatusCommands()


void tProjectStatus(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttProjectStatusSelectRow();
		else
			ExttProjectStatusSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetProjectStatus();
				unxsRAD("New tProjectStatus table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tProjectStatus WHERE uStatus=%u"
						,uStatus);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uStatus);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uLockMain);
		sscanf(field[3],"%u",&uLockMainFunc);
		sscanf(field[4],"%u",&uLockModule);
		sscanf(field[5],"%u",&uLockModuleFunc);
		sscanf(field[6],"%u",&uLockMakefile);
		sscanf(field[7],"%u",&uLockInclude);
		sscanf(field[8],"%u",&uLockLanguage);
		sscanf(field[9],"%u",&uLockCapFiles);
		sscanf(field[10],"%u",&uOwner);
		sscanf(field[11],"%u",&uCreatedBy);
		sscanf(field[12],"%lu",&uCreatedDate);
		sscanf(field[13],"%u",&uModBy);
		sscanf(field[14],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tProjectStatus",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tProjectStatusTools>");
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

        ExttProjectStatusButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tProjectStatus Record Data",100);

	if(guMode==2000 || guMode==2002)
		tProjectStatusInput(1);
	else
		tProjectStatusInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttProjectStatusAuxTable();

	Footer_ism3();

}//end of tProjectStatus();


void tProjectStatusInput(unsigned uMode)
{

//uStatus
	OpenRow(LANG_FL_tProjectStatus_uStatus,"black");
	printf("<input title='%s' type=text name=uStatus value=%u size=16 maxlength=10 "
,LANG_FT_tProjectStatus_uStatus,uStatus);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uStatus value=%u >\n",uStatus);
	}
//cLabel
	OpenRow(LANG_FL_tProjectStatus_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tProjectStatus_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uLockMain
	OpenRow(LANG_FL_tProjectStatus_uLockMain,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uLockMain",uLockMain,1);
	else
		YesNoPullDown("uLockMain",uLockMain,0);
//uLockMainFunc
	OpenRow(LANG_FL_tProjectStatus_uLockMainFunc,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uLockMainFunc",uLockMainFunc,1);
	else
		YesNoPullDown("uLockMainFunc",uLockMainFunc,0);
//uLockModule
	OpenRow(LANG_FL_tProjectStatus_uLockModule,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uLockModule",uLockModule,1);
	else
		YesNoPullDown("uLockModule",uLockModule,0);
//uLockModuleFunc
	OpenRow(LANG_FL_tProjectStatus_uLockModuleFunc,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uLockModuleFunc",uLockModuleFunc,1);
	else
		YesNoPullDown("uLockModuleFunc",uLockModuleFunc,0);
//uLockMakefile
	OpenRow(LANG_FL_tProjectStatus_uLockMakefile,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uLockMakefile",uLockMakefile,1);
	else
		YesNoPullDown("uLockMakefile",uLockMakefile,0);
//uLockInclude
	OpenRow(LANG_FL_tProjectStatus_uLockInclude,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uLockInclude",uLockInclude,1);
	else
		YesNoPullDown("uLockInclude",uLockInclude,0);
//uLockLanguage
	OpenRow(LANG_FL_tProjectStatus_uLockLanguage,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uLockLanguage",uLockLanguage,1);
	else
		YesNoPullDown("uLockLanguage",uLockLanguage,0);
//uLockCapFiles
	OpenRow(LANG_FL_tProjectStatus_uLockCapFiles,"black");
	if(guPermLevel>=7 && uMode)
		YesNoPullDown("uLockCapFiles",uLockCapFiles,1);
	else
		YesNoPullDown("uLockCapFiles",uLockCapFiles,0);
//uOwner
	OpenRow(LANG_FL_tProjectStatus_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tProjectStatus_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tProjectStatus_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tProjectStatus_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tProjectStatus_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tProjectStatusInput(unsigned uMode)


void NewtProjectStatus(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uStatus FROM tProjectStatus\
				WHERE uStatus=%u"
							,uStatus);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tProjectStatus("<blink>Record already exists");
		tProjectStatus(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tProjectStatus();
	//sprintf(gcQuery,"New record %u added");
	uStatus=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tProjectStatus",uStatus);
	unxsRADLog(uStatus,"tProjectStatus","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uStatus);
	tProjectStatus(gcQuery);
	}

}//NewtProjectStatus(unsigned uMode)


void DeletetProjectStatus(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tProjectStatus WHERE uStatus=%u AND ( uOwner=%u OR %u>9 )"
					,uStatus,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tProjectStatus WHERE uStatus=%u"
					,uStatus);
#endif
	macro_mySQLQueryHTMLError;
	//tProjectStatus("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsRADLog(uStatus,"tProjectStatus","Del");
#endif
		tProjectStatus(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsRADLog(uStatus,"tProjectStatus","DelError");
#endif
		tProjectStatus(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetProjectStatus(void)


void Insert_tProjectStatus(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tProjectStatus SET uStatus=%u,cLabel='%s',uLockMain=%u,uLockMainFunc=%u,uLockModule=%u,uLockModuleFunc=%u,uLockMakefile=%u,uLockInclude=%u,uLockLanguage=%u,uLockCapFiles=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uStatus
			,TextAreaSave(cLabel)
			,uLockMain
			,uLockMainFunc
			,uLockModule
			,uLockModuleFunc
			,uLockMakefile
			,uLockInclude
			,uLockLanguage
			,uLockCapFiles
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tProjectStatus(void)


void Update_tProjectStatus(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tProjectStatus SET uStatus=%u,cLabel='%s',uLockMain=%u,uLockMainFunc=%u,uLockModule=%u,uLockModuleFunc=%u,uLockMakefile=%u,uLockInclude=%u,uLockLanguage=%u,uLockCapFiles=%u,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uStatus
			,TextAreaSave(cLabel)
			,uLockMain
			,uLockMainFunc
			,uLockModule
			,uLockModuleFunc
			,uLockMakefile
			,uLockInclude
			,uLockLanguage
			,uLockCapFiles
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tProjectStatus(void)


void ModtProjectStatus(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tProjectStatus.uStatus,\
				tProjectStatus.uModDate\
				FROM tProjectStatus,tClient\
				WHERE tProjectStatus.uStatus=%u\
				AND tProjectStatus.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uStatus,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uStatus,uModDate FROM tProjectStatus\
				WHERE uStatus=%u"
						,uStatus);
#else
	sprintf(gcQuery,"SELECT uStatus FROM tProjectStatus\
				WHERE uStatus=%u"
						,uStatus);
#endif

	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tProjectStatus("<blink>Record does not exist");
	if(i<1) tProjectStatus(LANG_NBR_RECNOTEXIST);
	//if(i>1) tProjectStatus("<blink>Multiple rows!");
	if(i>1) tProjectStatus(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tProjectStatus(LANG_NBR_EXTMOD);
#endif

	Update_tProjectStatus(field[0]);
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tProjectStatus",uStatus);
	unxsRADLog(uStatus,"tProjectStatus","Mod");
#endif
	tProjectStatus(gcQuery);

}//ModtProjectStatus(void)


void tProjectStatusList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttProjectStatusListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tProjectStatusList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttProjectStatusListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uStatus<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>uLockMain<td><font face=arial,helvetica color=white>uLockMainFunc<td><font face=arial,helvetica color=white>uLockModule<td><font face=arial,helvetica color=white>uLockModuleFunc<td><font face=arial,helvetica color=white>uLockMakefile<td><font face=arial,helvetica color=white>uLockInclude<td><font face=arial,helvetica color=white>uLockLanguage<td><font face=arial,helvetica color=white>uLockCapFiles<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		long unsigned luYesNo2=strtoul(field[2],NULL,10);
		char cBuf2[4];
		if(luYesNo2)
			sprintf(cBuf2,"Yes");
		else
			sprintf(cBuf2,"No");
		long unsigned luYesNo3=strtoul(field[3],NULL,10);
		char cBuf3[4];
		if(luYesNo3)
			sprintf(cBuf3,"Yes");
		else
			sprintf(cBuf3,"No");
		long unsigned luYesNo4=strtoul(field[4],NULL,10);
		char cBuf4[4];
		if(luYesNo4)
			sprintf(cBuf4,"Yes");
		else
			sprintf(cBuf4,"No");
		long unsigned luYesNo5=strtoul(field[5],NULL,10);
		char cBuf5[4];
		if(luYesNo5)
			sprintf(cBuf5,"Yes");
		else
			sprintf(cBuf5,"No");
		long unsigned luYesNo6=strtoul(field[6],NULL,10);
		char cBuf6[4];
		if(luYesNo6)
			sprintf(cBuf6,"Yes");
		else
			sprintf(cBuf6,"No");
		long unsigned luYesNo7=strtoul(field[7],NULL,10);
		char cBuf7[4];
		if(luYesNo7)
			sprintf(cBuf7,"Yes");
		else
			sprintf(cBuf7,"No");
		long unsigned luYesNo8=strtoul(field[8],NULL,10);
		char cBuf8[4];
		if(luYesNo8)
			sprintf(cBuf8,"Yes");
		else
			sprintf(cBuf8,"No");
		long unsigned luYesNo9=strtoul(field[9],NULL,10);
		char cBuf9[4];
		if(luYesNo9)
			sprintf(cBuf9,"Yes");
		else
			sprintf(cBuf9,"No");
		time_t luTime12=strtoul(field[12],NULL,10);
		char cBuf12[32];
		if(luTime12)
			ctime_r(&luTime12,cBuf12);
		else
			sprintf(cBuf12,"---");
		time_t luTime14=strtoul(field[14],NULL,10);
		char cBuf14[32];
		if(luTime14)
			ctime_r(&luTime14,cBuf14);
		else
			sprintf(cBuf14,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,cBuf2
			,cBuf3
			,cBuf4
			,cBuf5
			,cBuf6
			,cBuf7
			,cBuf8
			,cBuf9
			,ForeignKey("tClient","cLabel",strtoul(field[10],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[11],NULL,10))
			,cBuf12
			,ForeignKey("tClient","cLabel",strtoul(field[13],NULL,10))
			,cBuf14
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tProjectStatusList()


void CreatetProjectStatus(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tProjectStatus ("
			" uStatus INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			" uOwner INT UNSIGNED NOT NULL DEFAULT 0,"
			" uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			" cLabel VARCHAR(32) NOT NULL DEFAULT '',"
			" uLockMain INT UNSIGNED NOT NULL DEFAULT 0,"
			" uLockMainFunc INT UNSIGNED NOT NULL DEFAULT 0,"
			" uLockModule INT UNSIGNED NOT NULL DEFAULT 0,"
			" uLockModuleFunc INT UNSIGNED NOT NULL DEFAULT 0,"
			" uLockMakefile INT UNSIGNED NOT NULL DEFAULT 0,"
			" uLockInclude INT UNSIGNED NOT NULL DEFAULT 0,"
			" uLockLanguage INT UNSIGNED NOT NULL DEFAULT 0,"
			" uLockCapFiles INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetProjectStatus()

