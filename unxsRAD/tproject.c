/*
FILE
	tProject source code of unxsRAD.cgi
	Built by mysqlRAD2.cgi 
	(C) Gary Wallis 2001-2017 for Unixservice, LLC.
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tprojectfunc.c while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uProject: Primary Key
static unsigned uProject=0;
//cLabel: Short label
static char cLabel[33]={""};
//uProjectStatus: Status of project
static unsigned uProjectStatus=0;
static char cuProjectStatusPullDown[256]={""};
//uTemplateSet: Template set or group to use
static unsigned uTemplateSet=0;
static char cuTemplateSetPullDown[256]={""};
//cDescription: Project blurb
static char *cDescription={""};
//cDirectory: Project Source Code Build Directory
static char cDirectory[101]={""};
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


#define VAR_LIST_tProject "tProject.uProject,tProject.cLabel,tProject.uProjectStatus,tProject.uTemplateSet,tProject.cDescription,tProject.cDirectory,tProject.uOwner,tProject.uCreatedBy,tProject.uCreatedDate,tProject.uModBy,tProject.uModDate"

 //Local only
void Insert_tProject(void);
void Update_tProject(char *cRowid);
void ProcesstProjectListVars(pentry entries[], int x);

 //In tProjectfunc.c file included below
void ExtProcesstProjectVars(pentry entries[], int x);
void ExttProjectCommands(pentry entries[], int x);
void ExttProjectButtons(void);
void ExttProjectNavBar(void);
void ExttProjectGetHook(entry gentries[], int x);
void ExttProjectSelect(void);
void ExttProjectSelectRow(void);
void ExttProjectListSelect(void);
void ExttProjectListFilter(void);
void ExttProjectAuxTable(void);

#include "tprojectfunc.c"

 //Table Variables Assignment Function
void ProcesstProjectVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uProject"))
			sscanf(entries[i].val,"%u",&uProject);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"uProjectStatus"))
			sscanf(entries[i].val,"%u",&uProjectStatus);
		else if(!strcmp(entries[i].name,"cuProjectStatusPullDown"))
		{
			sprintf(cuProjectStatusPullDown,"%.255s",entries[i].val);
			uProjectStatus=ReadPullDown("tProjectStatus","cLabel",cuProjectStatusPullDown);
		}
		else if(!strcmp(entries[i].name,"uTemplateSet"))
			sscanf(entries[i].val,"%u",&uTemplateSet);
		else if(!strcmp(entries[i].name,"cuTemplateSetPullDown"))
		{
			sprintf(cuTemplateSetPullDown,"%.255s",entries[i].val);
			uTemplateSet=ReadPullDown("tTemplateSet","cLabel",cuTemplateSetPullDown);
		}
		else if(!strcmp(entries[i].name,"cDescription"))
			cDescription=entries[i].val;
		else if(!strcmp(entries[i].name,"cDirectory"))
			sprintf(cDirectory,"%.100s",entries[i].val);
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
	ExtProcesstProjectVars(entries,x);

}//ProcesstProjectVars()


void ProcesstProjectListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uProject);
                        guMode=2002;
                        tProject("");
                }
        }
}//void ProcesstProjectListVars(pentry entries[], int x)


int tProjectCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttProjectCommands(entries,x);

	if(!strcmp(gcFunction,"tProjectTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tProjectList();
		}

		//Default
		ProcesstProjectVars(entries,x);
		tProject("");
	}
	else if(!strcmp(gcFunction,"tProjectList"))
	{
		ProcessControlVars(entries,x);
		ProcesstProjectListVars(entries,x);
		tProjectList();
	}

	return(0);

}//tProjectCommands()


void tProject(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttProjectSelectRow();
		else
			ExttProjectSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetProject();
				unxsRAD("New tProject table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tProject WHERE uProject=%u"
						,uProject);
				macro_mySQLRunAndStore(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uProject);
		sprintf(cLabel,"%.32s",field[1]);
		sscanf(field[2],"%u",&uProjectStatus);
		sscanf(field[3],"%u",&uTemplateSet);
		cDescription=field[4];
		sprintf(cDirectory,"%.100s",field[5]);
		sscanf(field[6],"%u",&uOwner);
		sscanf(field[7],"%u",&uCreatedBy);
		sscanf(field[8],"%lu",&uCreatedDate);
		sscanf(field[9],"%u",&uModBy);
		sscanf(field[10],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tProject",0);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tProjectTools>");
	printf("<input type=hidden name=gluRowid value=%lu>",gluRowid);
	if(guI)
	{
		if(guMode==6)
			//printf(" Found");
			printf(LANG_NBR_FOUND);
		else if(guMode==7)
			printf("Workflow loaded");
		else if(guMode==5)
			//printf(" Modified");
			printf(LANG_NBR_MODIFIED);
		else if(guMode==4)
			//printf(" New");
			printf(LANG_NBR_NEW);
		if(guMode!=7)
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

        ExttProjectButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tProject Record Data",100);

	if(guMode==2000 || guMode==2002)
		tProjectInput(1);
	else
		tProjectInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttProjectAuxTable();

	Footer_ism3();

}//end of tProject();


void tProjectInput(unsigned uMode)
{

//uProject
	OpenRow(LANG_FL_tProject_uProject,"black");
	printf("<input title='%s' type=text name=uProject value=%u size=16 maxlength=10 "
			,LANG_FT_tProject_uProject,uProject);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uProject value=%u >\n",uProject);
	}
//cLabel
	OpenRow(LANG_FL_tProject_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
		,LANG_FT_tProject_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//uProjectStatus
	OpenRow(LANG_FL_tProject_uProjectStatus,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDown("tProjectStatus;cuProjectStatusPullDown","cLabel","cLabel",uProjectStatus,1);
	else
		tTablePullDown("tProjectStatus;cuProjectStatusPullDown","cLabel","cLabel",uProjectStatus,0);
//uTemplateSet
	OpenRow(LANG_FL_tProject_uTemplateSet,"black");
	if(guPermLevel>=7 && uMode)
		tTablePullDown("tTemplateSet;cuTemplateSetPullDown","cLabel","cLabel",uTemplateSet,1);
	else
		tTablePullDown("tTemplateSet;cuTemplateSetPullDown","cLabel","cLabel",uTemplateSet,0);
//cDescription
	OpenRow(LANG_FL_tProject_cDescription,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cDescription "
		,LANG_FT_tProject_cDescription);
	if(guPermLevel>=7 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cDescription);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cDescription);
		printf("<input type=hidden name=cDescription value=\"%s\" >\n",EncodeDoubleQuotes(cDescription));
	}
//cDirectory
	OpenRow(LANG_FL_tProject_cDirectory,"black");
	printf("<input title='%s' type=text name=cDirectory value=\"%s\" size=40 maxlength=100 "
		,LANG_FT_tProject_cDirectory,EncodeDoubleQuotes(cDirectory));
	if(guPermLevel>=7 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cDirectory value=\"%s\">\n",EncodeDoubleQuotes(cDirectory));
	}
//uOwner
	OpenRow(LANG_FL_tProject_uOwner,"black");
	if(guPermLevel>=20 && uMode)
		printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	else
		printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
//uCreatedBy
	OpenRow(LANG_FL_tProject_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
		printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	else
		printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
//uCreatedDate
	OpenRow(LANG_FL_tProject_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tProject_uModBy,"black");
	if(guPermLevel>=20 && uMode)
		printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	else
		printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
//uModDate
	OpenRow(LANG_FL_tProject_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");

}//void tProjectInput(unsigned uMode)


void NewtProject(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;
	char gcQuery[128];

	sprintf(gcQuery,"SELECT uProject FROM tProject WHERE uProject=%u",uProject);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	if(i) 
		//tProject("<blink>Record already exists");
		tProject(LANG_NBR_RECEXISTS);

	Insert_tProject();
	//sprintf(gcQuery,"New record %u added");
	uProject=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tProject",uProject);
	unxsRADLog(uProject,"tProject","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uProject);
		tProject(gcQuery);
	}

}//NewtProject(unsigned uMode)


void DeletetProject(void)
{
	sprintf(gcQuery,"DELETE FROM tProject WHERE uProject=%u AND ( uOwner=%u OR %u>9 )"
					,uProject,guLoginClient,guPermLevel);
	macro_mySQLQueryHTMLError;
	//tProject("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsRADLog(uProject,"tProject","Del");
		tProject(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsRADLog(uProject,"tProject","DelError");
		tProject(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetProject(void)


void Insert_tProject(void)
{
	sprintf(gcQuery,"INSERT INTO tProject SET uProject=%u,cLabel='%s',uProjectStatus=%u,uTemplateSet=%u,"
			"cDescription='%s',cDirectory='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uProject
			,TextAreaSave(cLabel)
			,uProjectStatus
			,uTemplateSet
			,TextAreaSave(cDescription)
			,TextAreaSave(cDirectory)
			,uOwner
			,uCreatedBy
			);

	macro_mySQLQueryHTMLError;

}//void Insert_tProject(void)


void Update_tProject(char *cRowid)
{
	sprintf(gcQuery,"UPDATE tProject SET uProject=%u,cLabel='%s',uProjectStatus=%u,uTemplateSet=%u,"
			"cDescription='%s',cDirectory='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uProject
			,TextAreaSave(cLabel)
			,uProjectStatus
			,uTemplateSet
			,TextAreaSave(cDescription)
			,TextAreaSave(cDirectory)
			,uModBy
			,cRowid);

	macro_mySQLQueryHTMLError;

}//void Update_tProject(void)


void ModtProject(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tProject.uProject,"
			" tProject.uModDate"
			" FROM tProject,tClient"
			" WHERE tProject.uProject=%u"
			" AND tProject.uOwner=tClient.uClient"
			" AND (tClient.uOwner=%u OR tClient.uClient=%u)"
				,uProject,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uProject,uModDate FROM tProject WHERE uProject=%u",
				uProject);
	macro_mySQLRunAndStore(res);
	i=mysql_num_rows(res);

	//if(i<1) tProject("<blink>Record does not exist");
	if(i<1) tProject(LANG_NBR_RECNOTEXIST);
	//if(i>1) tProject("<blink>Multiple rows!");
	if(i>1) tProject(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tProject(LANG_NBR_EXTMOD);

	Update_tProject(field[0]);
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tProject",uProject);
	unxsRADLog(uProject,"tProject","Mod");
	tProject(gcQuery);

}//ModtProject(void)


void tProjectList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttProjectListSelect();

	macro_mySQLRunAndStore(res);
	guI=mysql_num_rows(res);

	PageMachine("tProjectList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttProjectListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uProject"
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>uProjectStatus"
		"<td><font face=arial,helvetica color=white>uTemplateSet"
		"<td><font face=arial,helvetica color=white>cDescription"
		"<td><font face=arial,helvetica color=white>cDirectory"
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
		time_t luTime8=strtoul(field[8],NULL,10);
		char cBuf8[32];
		if(luTime8)
			ctime_r(&luTime8,cBuf8);
		else
			sprintf(cBuf8,"---");
		time_t luTime10=strtoul(field[10],NULL,10);
		char cBuf10[32];
		if(luTime10)
			ctime_r(&luTime10,cBuf10);
		else
			sprintf(cBuf10,"---");
		printf("<td><input type=submit name=ED%s value=Edit>"
			"%s<td>%s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,ForeignKey("tProjectStatus","cLabel",strtoul(field[2],NULL,10))
			,ForeignKey("tTemplateSet","cLabel",strtoul(field[3],NULL,10))
			,field[4]
			,field[5]
			,ForeignKey("tClient","cLabel",strtoul(field[6],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[7],NULL,10))
			,cBuf8
			,ForeignKey("tClient","cLabel",strtoul(field[9],NULL,10))
			,cBuf10
				);
	}

	printf("</table></form>\n");
	Footer_ism3();

}//tProjectList()


void CreatetProject(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tProject ("
			" uProject INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			" cLabel VARCHAR(32) NOT NULL DEFAULT '',"
			" uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner),"
			" uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			" uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			" uProjectStatus INT UNSIGNED NOT NULL DEFAULT 0,"
			" uTemplateSet INT UNSIGNED NOT NULL DEFAULT 0,"
			" cDescription TEXT NOT NULL DEFAULT '',"
			" cDirectory VARCHAR(100) NOT NULL DEFAULT '' )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetProject()

