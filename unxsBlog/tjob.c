/*
FILE
	tJob source code of unxsBlog.cgi
	Built by mysqlRAD2.cgi (C) Gary Wallis 2001-2007
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
//cLabel: Label for user feedback
static char cLabel[101]={""};
//cServer: Server name ip or any server
static char cServer[65]={""};
//cJobName: Subsystem.Function style job name
static char cJobName[65]={""};
//uJobTarget: Target ID
static unsigned uJobTarget=0;
//uJobTarget2: Target User ID
static unsigned uJobTargetUser=0;
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



#define VAR_LIST_tJob "tJob.uJob,tJob.cLabel,tJob.cServer,tJob.cJobName,tJob.uJobTarget,tJob.uJobTargetUser,tJob.cJobData,tJob.uJobDate,tJob.uJobStatus,tJob.cRemoteMsg,tJob.uOwner,tJob.uCreatedBy,tJob.uCreatedDate,tJob.uModBy,tJob.uModDate"

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
		else if(!strcmp(entries[i].name,"cLabel"))
			sprintf(cLabel,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cServer"))
			sprintf(cServer,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"cJobName"))
			sprintf(cJobName,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uJobTarget"))
			sscanf(entries[i].val,"%u",&uJobTarget);
		else if(!strcmp(entries[i].name,"uJobTargetUser"))
			sscanf(entries[i].val,"%u",&uJobTargetUser);
		else if(!strcmp(entries[i].name,"cJobData"))
			cJobData=entries[i].val;
		else if(!strcmp(entries[i].name,"uJobDate"))
			sscanf(entries[i].val,"%lu",&uJobDate);
		else if(!strcmp(entries[i].name,"uJobStatus"))
			sscanf(entries[i].val,"%u",&uJobStatus);
		else if(!strcmp(entries[i].name,"cuJobStatusPullDown"))
		{
			sprintf(cuJobStatusPullDown,"%.255s",entries[i].val);
			uJobStatus=ReadPullDown("tJobStatus","cLabel",cuJobStatusPullDown);
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
				unxsBlog("New tJob table created");
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
		sprintf(cLabel,"%.100s",field[1]);
		sprintf(cServer,"%.64s",field[2]);
		sprintf(cJobName,"%.64s",field[3]);
		sscanf(field[4],"%u",&uJobTarget);
		sscanf(field[5],"%u",&uJobTargetUser);
		cJobData=field[6];
		sscanf(field[7],"%lu",&uJobDate);
		sscanf(field[8],"%u",&uJobStatus);
		sprintf(cRemoteMsg,"%.32s",field[9]);
		sscanf(field[10],"%u",&uOwner);
		sscanf(field[11],"%u",&uCreatedBy);
		sscanf(field[12],"%lu",&uCreatedDate);
		sscanf(field[13],"%u",&uModBy);
		sscanf(field[14],"%lu",&uModDate);

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
//cLabel
	OpenRow(LANG_FL_tJob_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=100 "
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
//uJobTarget
	OpenRow(LANG_FL_tJob_uJobTarget,"black");
	if(guPermLevel>=0 && uMode)
	{
	printf("%s<input type=hidden name=uJobTarget value=%u >\n",ForeignKey("tTarget","cDomain",uJobTarget),uJobTarget);
	}
	else
	{
	printf("%s<input type=hidden name=uJobTarget value=%u >\n",ForeignKey("tTarget","cDomain",uJobTarget),uJobTarget);
	}
//uJobTargetUser
	OpenRow(LANG_FL_tJob_uJobTargetUser,"black");
	if(guPermLevel>=0 && uMode)
	{
	printf("%s<input type=hidden name=uJobTargetUser value=%u >\n",ForeignKey("tTargetUser","cLogin",uJobTargetUser),uJobTargetUser);
	}
	else
	{
	printf("%s<input type=hidden name=uJobTargetUser value=%u >\n",ForeignKey("tTargetUser","cLogin",uJobTargetUser),uJobTargetUser);
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
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tJobStatus;cuJobStatusPullDown","cLabel","cLabel",uJobStatus,1);
	else
		tTablePullDown("tJobStatus;cuJobStatusPullDown","cLabel","cLabel",uJobStatus,0);
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
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
	else
	{
	printf("%s<input type=hidden name=uOwner value=%u >\n",ForeignKey("tClient","cLabel",uOwner),uOwner);
	}
//uCreatedBy
	OpenRow(LANG_FL_tJob_uCreatedBy,"black");
	if(guPermLevel>=20 && uMode)
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
	printf("%s<input type=hidden name=uCreatedBy value=%u >\n",ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
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
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
	}
	else
	{
	printf("%s<input type=hidden name=uModBy value=%u >\n",ForeignKey("tClient","cLabel",uModBy),uModBy);
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
	unxsBlogLog(uJob,"tJob","New");
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
		unxsBlogLog(uJob,"tJob","Del");
#endif
		tJob(LANG_NBR_RECDELETED);
	}
	else
	{
#ifdef ISM3FIELDS
		unxsBlogLog(uJob,"tJob","DelError");
#endif
		tJob(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetJob(void)


void Insert_tJob(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tJob SET uJob=%u,cLabel='%s',cServer='%s',cJobName='%s',uJobTarget=%u,uJobTargetUser=%u,cJobData='%s',uJobDate=%lu,uJobStatus=%u,cRemoteMsg='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uJob
			,TextAreaSave(cLabel)
			,TextAreaSave(cServer)
			,TextAreaSave(cJobName)
			,uJobTarget
			,uJobTargetUser
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
	sprintf(gcQuery,"UPDATE tJob SET uJob=%u,cLabel='%s',cServer='%s',cJobName='%s',uJobTarget=%u,uJobTargetUser=%u,cJobData='%s',uJobDate=%lu,uJobStatus=%u,cRemoteMsg='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uJob
			,TextAreaSave(cLabel)
			,TextAreaSave(cServer)
			,TextAreaSave(cJobName)
			,uJobTarget
			,uJobTargetUser
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
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tJob.uJob,\
				tJob.uModDate\
				FROM tJob,tClient\
				WHERE tJob.uJob=%u\
				AND tJob.uOwner=tClient.uClient\
				AND (tClient.uOwner=%u OR tClient.uClient=%u)"
			,uJob,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uJob,uModDate FROM tJob\
				WHERE uJob=%u"
						,uJob);
#else
	sprintf(gcQuery,"SELECT uJob FROM tJob\
				WHERE uJob=%u"
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
	unxsBlogLog(uJob,"tJob","Mod");
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
	printf("<tr bgcolor=black><td><font face=arial,helvetica color=white>uJob<td><font face=arial,helvetica color=white>cLabel<td><font face=arial,helvetica color=white>cServer<td><font face=arial,helvetica color=white>cJobName<td><font face=arial,helvetica color=white>uJobTarget<td><font face=arial,helvetica color=white>uJobTargetUser<td><font face=arial,helvetica color=white>cJobData<td><font face=arial,helvetica color=white>uJobDate<td><font face=arial,helvetica color=white>uJobStatus<td><font face=arial,helvetica color=white>cRemoteMsg<td><font face=arial,helvetica color=white>uOwner<td><font face=arial,helvetica color=white>uCreatedBy<td><font face=arial,helvetica color=white>uCreatedDate<td><font face=arial,helvetica color=white>uModBy<td><font face=arial,helvetica color=white>uModDate</tr>");



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
		printf("<td><input type=submit name=ED%s value=Edit> %s<td>%s<td>%s<td>%s<td>%s<td>%s<td><textarea disabled>%s</textarea><td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s<td>%s</tr>"
			,field[0]
			,field[0]
			,field[1]
			,field[2]
			,field[3]
			,ForeignKey("tTarget","cDomain",strtoul(field[4],NULL,10))
			,ForeignKey("tTargetUser","cLogin",strtoul(field[5],NULL,10))
			,field[6]
			,cBuf7
			,ForeignKey("tJobStatus","cLabel",strtoul(field[8],NULL,10))
			,field[9]
			,ForeignKey("tClient","cLabel",strtoul(field[10],NULL,10))
			,ForeignKey("tClient","cLabel",strtoul(field[11],NULL,10))
			,cBuf12
			,ForeignKey("tClient","cLabel",strtoul(field[13],NULL,10))
			,cBuf14
				);

	}

	printf("</table></form>\n");
	Footer_ism3();

}//tJobList()


void CreatetJob(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tJob ( cJobName VARCHAR(64) NOT NULL DEFAULT '', uModDate INT UNSIGNED NOT NULL DEFAULT 0, cServer VARCHAR(64) NOT NULL DEFAULT '', uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, cJobData TEXT NOT NULL DEFAULT '', uJob INT UNSIGNED PRIMARY KEY AUTO_INCREMENT, cLabel VARCHAR(100) NOT NULL DEFAULT '', uJobStatus INT UNSIGNED NOT NULL DEFAULT 0, uJobDate INT UNSIGNED NOT NULL DEFAULT 0, cRemoteMsg VARCHAR(32) NOT NULL DEFAULT '', uJobTarget INT UNSIGNED NOT NULL DEFAULT 0, uJobTargetUser INT UNSIGNED NOT NULL DEFAULT 0 )");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
}//CreatetJob()

//perlSAR patch1
