/*
FILE
	tJob source code of unxsISP.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis and Hugo Urquiza 2001-2009
	svn ID removed
PURPOSE
	Schema dependent RAD generated file.
	Program app functionality in tjobfunc.h while 
	RAD is still to be used.
*/


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uJob: Primary Key
static unsigned uJob=0;
//uJobGroup: Same as first uJob for related instance service jobs
static unsigned uJobGroup=0;
//cLabel: Short label
static char cLabel[33]={""};
//cServer: Server name ip or any server
static char cServer[65]={""};
//cJobName: Subsystem.Function style job name
static char cJobName[65]={""};
//uJobClient: Owner of resources created/mod
static unsigned uJobClient=0;
//uInstance: Job belongs to this product instance
static unsigned uInstance=0;
//cJobData: Remote subsystem server function arguments
static char *cJobData={""};
//uJobDate: Unix seconds for job to start to be considered
static time_t uJobDate=0;
//uJobStatus: Waiting, being processed, done, error
static unsigned uJobStatus=0;
static char cuJobStatusPullDown[256]={""};
//cRemoteMsg: Remote subsytem error message
static char cRemoteMsg[33]={""};
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



#define VAR_LIST_tJob "tJob.uJob,tJob.uJobGroup,tJob.cLabel,tJob.cServer,tJob.cJobName,tJob.uJobClient,tJob.uInstance,tJob.cJobData,tJob.uJobDate,tJob.uJobStatus,tJob.cRemoteMsg,tJob.uOwner,tJob.uCreatedBy,tJob.uCreatedDate,tJob.uModBy,tJob.uModDate"

 //Local only
void Insert_tJob(void);
void Update_tJob(char *cRowid);
void ProcesstJobListVars(pentry entries[], int x);

 //In tJobfunc.h file included below
void ExtProcesstJobVars(pentry entries[], int x);
void ExttJobCommands(pentry entries[], int x);
void ExttJobButtons(void);
void ExttJobNavBar(void);
void ExttJobGetHook(entry gentries[], int x);
void ExttJobSelect(void);
void ExttJobSelectRow(void);
void ExttJobListSelect(void);
void ExttJobListFilter(void);
void ExttJobAuxTable(void);

#include "tjobfunc.h"

 //Table Variables Assignment Function
void ProcesstJobVars(pentry entries[], int x)
{
	register int i;


	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uJob"))
			sscanf(entries[i].val,"%u",&uJob);
		else if(!strcmp(entries[i].name,"uJobGroup"))
			sscanf(entries[i].val,"%u",&uJobGroup);
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cServer"))
			sprintf(cServer,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cJobName"))
			sprintf(cJobName,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uJobClient"))
			sscanf(entries[i].val,"%u",&uJobClient);
		else if(!strcmp(entries[i].name,"uInstance"))
			sscanf(entries[i].val,"%u",&uInstance);
		else if(!strcmp(entries[i].name,"cJobData"))
			cJobData=entries[i].val;
		else if(!strcmp(entries[i].name,"uJobDate"))
			sscanf(entries[i].val,"%lu",&uJobDate);
		else if(!strcmp(entries[i].name,"uJobStatus"))
			sscanf(entries[i].val,"%u",&uJobStatus);
		else if(!strcmp(entries[i].name,"cuJobStatusPullDown"))
		{
			sprintf(cuJobStatusPullDown,"%.255s",entries[i].val);
			uJobStatus=ReadPullDown("tStatus","cLabel",cuJobStatusPullDown);
		}
		else if(!strcmp(entries[i].name,"cRemoteMsg"))
			sprintf(cRemoteMsg,"%.32s",entries[i].val);
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
	ExtProcesstJobVars(entries,x);

}//ProcesstJobVars()


void ProcesstJobListVars(pentry entries[], int x)
{
        register int i;

        for(i=0;i<x;i++)
        {
                if(!strncmp(entries[i].name,"ED",2))
                {
                        sscanf(entries[i].name+2,"%u",&uJob);
                        guMode=2002;
                        tJob("");
                }
        }
}//void ProcesstJobListVars(pentry entries[], int x)


int tJobCommands(pentry entries[], int x)
{
	ProcessControlVars(entries,x);

	ExttJobCommands(entries,x);

	if(!strcmp(gcFunction,"tJobTools"))
	{
		if(!strcmp(gcFind,LANG_NB_LIST))
		{
			tJobList();
		}

		//Default
		ProcesstJobVars(entries,x);
		tJob("");
	}
	else if(!strcmp(gcFunction,"tJobList"))
	{
		ProcessControlVars(entries,x);
		ProcesstJobListVars(entries,x);
		tJobList();
	}

	return(0);

}//tJobCommands()


void tJob(const char *cResult)
{
	MYSQL_RES *res;
	MYSQL_RES *res2;
	MYSQL_ROW field;

	//Internal skip reloading
	if(!cResult[0])
	{
		if(guMode)
			ExttJobSelectRow();
		else
			ExttJobSelect();

		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
        	{
			if(strstr(mysql_error(&gMysql)," doesn't exist"))
                	{
				CreatetJob();
				unxsISP("New tJob table created");
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
			sprintf(gcQuery,"SELECT _rowid FROM tJob WHERE uJob=%u"
						,uJob);
				mysql_query(&gMysql,gcQuery);
				res2=mysql_store_result(&gMysql);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
		sscanf(field[0],"%u",&uJob);
		sscanf(field[1],"%u",&uJobGroup);
		sprintf(cLabel,"%.32s",field[2]);
		sprintf(cServer,"%.64s",field[3]);
		sprintf(cJobName,"%.64s",field[4]);
		sscanf(field[5],"%u",&uJobClient);
		sscanf(field[6],"%u",&uInstance);
		cJobData=field[7];
		sscanf(field[8],"%lu",&uJobDate);
		sscanf(field[9],"%u",&uJobStatus);
		sprintf(cRemoteMsg,"%.32s",field[10]);
		sscanf(field[11],"%u",&uOwner);
		sscanf(field[12],"%u",&uCreatedBy);
		sscanf(field[13],"%lu",&uCreatedDate);
		sscanf(field[14],"%u",&uModBy);
		sscanf(field[15],"%lu",&uModDate);

		}

	}//Internal Skip

	Header_ism3(":: tJob",1);
	printf("<table width=100%% cellspacing=0 cellpadding=0>\n");
	printf("<tr><td colspan=2 align=right valign=center>");


	printf("<input type=hidden name=gcFunction value=tJobTools>");
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

        ExttJobButtons();

        printf("</td><td valign=top>");
	//
	OpenFieldSet("tJob Record Data",100);

	if(guMode==2000 || guMode==2002)
		tJobInput(1);
	else
		tJobInput(0);

	//
	CloseFieldSet();

	//Bottom table
	printf("<tr><td colspan=2>");
        ExttJobAuxTable();

	Footer_ism3();

}//end of tJob();


void tJobInput(unsigned uMode)
{

//uJob
	OpenRow(LANG_FL_tJob_uJob,"black");
	printf("<input title='%s' type=text name=uJob value=%u size=16 maxlength=10 "
,LANG_FT_tJob_uJob,uJob);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uJob value=%u >\n",uJob);
	}
//uJobGroup
	OpenRow(LANG_FL_tJob_uJobGroup,"black");
	printf("<input title='%s' type=text name=uJobGroup value=%u size=16 maxlength=10 "
,LANG_FT_tJob_uJobGroup,uJobGroup);
	if(guPermLevel>=20 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=uJobGroup value=%u >\n",uJobGroup);
	}
//cLabel
	OpenRow(LANG_FL_tJob_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tJob_cLabel,EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//cServer
	OpenRow(LANG_FL_tJob_cServer,"black");
	printf("<input title='%s' type=text name=cServer value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tJob_cServer,EncodeDoubleQuotes(cServer));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cServer value=\"%s\">\n",EncodeDoubleQuotes(cServer));
	}
//cJobName
	OpenRow(LANG_FL_tJob_cJobName,"black");
	printf("<input title='%s' type=text name=cJobName value=\"%s\" size=40 maxlength=64 "
,LANG_FT_tJob_cJobName,EncodeDoubleQuotes(cJobName));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cJobName value=\"%s\">\n",EncodeDoubleQuotes(cJobName));
	}
//uJobClient
	OpenRow(LANG_FL_tJob_uJobClient,"black");
	if(guPermLevel>=0 && uMode)
	{
	printf("%s<input type=hidden name=uJobClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uJobClient),uJobClient);
	}
	else
	{
	printf("%s<input type=hidden name=uJobClient value=%u >\n",ForeignKey(TCLIENT,"cLabel",uJobClient),uJobClient);
	}
//uInstance
	OpenRow(LANG_FL_tJob_uInstance,"black");
	if(guPermLevel>=0 && uMode)
	{
	printf("%s<input type=hidden name=uInstance value=%u >\n",ForeignKey("tInstance","cLabel",uInstance),uInstance);
	}
	else
	{
	printf("%s<input type=hidden name=uInstance value=%u >\n",ForeignKey("tInstance","cLabel",uInstance),uInstance);
	}
//cJobData
	OpenRow(LANG_FL_tJob_cJobData,"black");
	printf("<textarea title='%s' cols=80 wrap=hard rows=16 name=cJobData "
,LANG_FT_tJob_cJobData);
	if(guPermLevel>=0 && uMode)
	{
		printf(">%s</textarea></td></tr>\n",cJobData);
	}
	else
	{
		printf("disabled>%s</textarea></td></tr>\n",cJobData);
		printf("<input type=hidden name=cJobData value=\"%s\" >\n",EncodeDoubleQuotes(cJobData));
	}
//uJobDate
	OpenRow(LANG_FL_tJob_uJobDate,"black");
	if(uJobDate)
		printf("<input type=text name=cuJobDate value='%s' disabled>\n",ctime(&uJobDate));
	else
		printf("<input type=text name=cuJobDate value='---' disabled>\n");
	printf("<input type=hidden name=uJobDate value=%lu>\n",uJobDate);
//uJobStatus
	OpenRow(LANG_FL_tJob_uJobStatus,"black");
	if(guPermLevel>=12 && uMode)
		tTablePullDown("tStatus;cuJobStatusPullDown","cLabel","cLabel",uJobStatus,1);
	else
		tTablePullDown("tStatus;cuJobStatusPullDown","cLabel","cLabel",uJobStatus,0);
//cRemoteMsg
	OpenRow(LANG_FL_tJob_cRemoteMsg,"black");
	printf("<input title='%s' type=text name=cRemoteMsg value=\"%s\" size=40 maxlength=32 "
,LANG_FT_tJob_cRemoteMsg,EncodeDoubleQuotes(cRemoteMsg));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cRemoteMsg value=\"%s\">\n",EncodeDoubleQuotes(cRemoteMsg));
	}
//uOwner
	OpenRow(LANG_FL_tJob_uOwner,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey(TCLIENT,"cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tJob_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uCreatedBy),uCreatedBy);
	}
//uCreatedDate
	OpenRow(LANG_FL_tJob_uCreatedDate,"black");
	if(uCreatedDate)
		printf("%s\n\n",ctime(&uCreatedDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uCreatedDate value=%lu >\n",uCreatedDate);
//uModBy
	OpenRow(LANG_FL_tJob_uModBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey(TCLIENT,"cLabel",uModBy),uModBy);
	}
//uModDate
	OpenRow(LANG_FL_tJob_uModDate,"black");
	if(uModDate)
		printf("%s\n\n",ctime(&uModDate));
	else
		printf("---\n\n");
	printf("<input type=hidden name=uModDate value=%lu >\n",uModDate);
	printf("</tr>\n");



}//void tJobInput(unsigned uMode)


void NewtJob(unsigned uMode)
{
	register int i=0;
	MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uJob FROM tJob\
				WHERE uJob=%u"
							,uJob);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i) 
		//tJob("<blink>Record already exists");
		tJob(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tJob();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uJob=mysql_insert_id(&gMysql);
#ifdef ISM3FIELDS
	uCreatedDate=luGetCreatedDate("tJob",uJob);
	unxsISPLog(uJob,"tJob","New");
#endif

	if(!uMode)
	{
	sprintf(gcQuery,LANG_NBR_NEWRECADDED,uJob);
	tJob(gcQuery);
	}

}//NewtJob(unsigned uMode)


void DeletetJob(void)
{
#ifdef ISM3FIELDS
	sprintf(gcQuery,"DELETE FROM tJob WHERE uJob=%u AND ( uOwner=%u OR %u>9 )"
					,uJob,guLoginClient,guPermLevel);
#else
	sprintf(gcQuery,"DELETE FROM tJob WHERE uJob=%u"
					,uJob);
#endif
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));

	//tJob("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
#ifdef ISM3FIELDS
		unxsISPLog(uJob,"tJob","Del");
#endif
		tJob(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsISPLog(uJob,"tJob","DelError");
#endif
		tJob(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetJob(void)


void Insert_tJob(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tJob SET uJob=%u,uJobGroup=%u,cLabel='%s',cServer='%s',cJobName='%s',uJobClient=%u,uInstance=%u,cJobData='%s',uJobDate=%lu,uJobStatus=%u,cRemoteMsg='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uJob
			,uJobGroup
			,TextAreaSave(cLabel)
			,TextAreaSave(cServer)
			,TextAreaSave(cJobName)
			,uJobClient
			,uInstance
			,TextAreaSave(cJobData)
			,uJobDate
			,uJobStatus
			,TextAreaSave(cRemoteMsg)
			,uOwner
			,uCreatedBy
			);

	mysql_query(&gMysql,gcQuery);

}//void Insert_tJob(void)


void Update_tJob(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tJob SET uJob=%u,uJobGroup=%u,cLabel='%s',cServer='%s',cJobName='%s',uJobClient=%u,uInstance=%u,cJobData='%s',uJobDate=%lu,uJobStatus=%u,cRemoteMsg='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uJob
			,uJobGroup
			,TextAreaSave(cLabel)
			,TextAreaSave(cServer)
			,TextAreaSave(cJobName)
			,uJobClient
			,uInstance
			,TextAreaSave(cJobData)
			,uJobDate
			,uJobStatus
			,TextAreaSave(cRemoteMsg)
			,uModBy
			,cRowid);

	mysql_query(&gMysql,gcQuery);

}//void Update_tJob(void)


void ModtJob(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
#ifdef ISM3FIELDS
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uJob,uModDate FROM tJob WHERE uJob=%u"
						,uJob);
#else
	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uJob=%u"
						,uJob);
#endif

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	//if(i<1) tJob("<blink>Record does not exist");
	if(i<1) tJob(LANG_NBR_RECNOTEXIST);
	//if(i>1) tJob("<blink>Multiple rows!");
	if(i>1) tJob(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
#ifdef ISM3FIELDS
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tJob(LANG_NBR_EXTMOD);
#endif

	Update_tJob(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
#ifdef ISM3FIELDS
	uModDate=luGetModDate("tJob",uJob);
	unxsISPLog(uJob,"tJob","Mod");
#endif
	tJob(gcQuery);

}//ModtJob(void)


void tJobList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttJobListSelect();

	mysql_query(&gMysql,gcQuery);
	if(mysql_error(&gMysql)[0]) htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	guI=mysql_num_rows(res);

	PageMachine("tJobList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttJobListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uJob<td><font face=arial,helvetica color=white>uJobGroup<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>cServer<td><font face=arial,helvetica color=white>cJobName<td><font face=arial,helvetica color=white>uJobClient<td><font face=arial,helvetica color=white>uInstance<td><font face=arial,helvetica color=white>cJobData<td><font face=arial,helvetica color=white>uJobDate<td><font face=arial,helvetica color=white>uJobStatus<td><font face=arial,helvetica color=white>cRemoteMsg<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		time_t luTime13=strtoul(field[13],NULL,10);
		char cBuf13[32];
		if(luTime13)
			ctime_r(&luTime13,cBuf13);
		else
			sprintf(cBuf13,"---");
		time_t luTime15=strtoul(field[15],NULL,10);
		char cBuf15[32];
		if(luTime15)
			ctime_r(&luTime15,cBuf15);
		else
			sprintf(cBuf15,"---");
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,field[4]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[5],NULL,10))
			,ForeignKey("tInstance","cLabel",strtoul(field[6],NULL,10))
			,field[7]
			,cBuf8
			,ForeignKey("tStatus","cLabel",strtoul(field[9],NULL,10))
			,field[10]
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[11],NULL,10))
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[12],NULL,10))
			,cBuf13
			,ForeignKey(TCLIENT,"cLabel",strtoul(field[14],NULL,10))
			,cBuf15
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tJobList()


void CreatetJob(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tJob ( cRemoteMsg VARCHAR(32) NOT NULL DEFAULT '', uJobStatus INT UNSIGNED NOT NULL DEFAULT 0, uJobDate INT UNSIGNED NOT NULL DEFAULT 0, cJobData TEXT NOT NULL DEFAULT '', uJob INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(32) NOT NULL DEFAULT '', uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT UNSIGNED NOT NULL DEFAULT 0, cServer VARCHAR(64) NOT NULL DEFAULT '', cJobName VARCHAR(64) NOT NULL DEFAULT '', uJobClient INT UNSIGNED NOT NULL DEFAULT 0, uInstance INT UNSIGNED NOT NULL DEFAULT 0, uJobGroup INT UNSIGNED NOT NULL DEFAULT 0,index (uJobGroup) )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetJob()

