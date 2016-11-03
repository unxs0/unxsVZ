/*
FILE
	tJob source code of unxsVZ.cgi
PURPOSE
	Job queue table
AUTHOR/LEGAL
	(C) 2001-2016 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file.	
*/
//git describe version info
static char *cGitVersion="GitVersion:"GitVersion;


#include "mysqlrad.h"

//Table Variables
//Table Variables
//uJob: Primary Key
static unsigned uJob=0;
//cLabel: Label for user feedback
static char cLabel[101]={""};
//cJobName: Subsystem.Function style job name
static char cJobName[65]={""};
//uDatacenter: Collection of uNodes
static unsigned uDatacenter=0;
//uNode: Hardware node
static unsigned uNode=0;
//uContainer: VZ VE container running on uNode
static unsigned uContainer=0;
//cJobData: Remote subsystem server function arguments
static char *cJobData={""};
//uJobDate: Unix seconds for job to start to be considered
static time_t uJobDate=0;
//uJobStatus: Waiting, being processed, done, error
static unsigned uJobStatus=0;
static char cuJobStatusPullDown[256]={""};
//cRemoteMsg: Remote subsytem error message
static char cRemoteMsg[65]={""};
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
//uMasterJob: New multi related job control.
static unsigned uMasterJob=0;


#define VAR_LIST_tJob "tJob.uJob,tJob.cLabel,tJob.cJobName,tJob.uDatacenter,tJob.uNode,tJob.uContainer,tJob.cJobData,tJob.uJobDate,tJob.uJobStatus,tJob.cRemoteMsg,tJob.uOwner,tJob.uCreatedBy,tJob.uCreatedDate,tJob.uModBy,tJob.uModDate,tJob.uMasterJob"

 //Local only
void Insert_tJob(void);
void Update_tJob(char *cRowid);
void ProcesstJobListVars(pentry entries[], int x);
void tJobNewStep(unsigned uStep);
void RecurringJobDropDown(unsigned uSelector, unsigned uMode);
unsigned ReadRecurringDropDown(char *cRecurringJobDropDown);
static unsigned uRecurringJob=0;
static char cRecurringJobDropDown[100]={""};

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
void SelectedUBCJobs(unsigned uCreateJobs);

void tTablePullDownActiveColorCoded(char *cuDatacenterPullDown,char *cLabel,char *cLabel2,unsigned uDatacenter,unsigned uMode);

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
		else if(!strcmp(entries[i].name,"cJobName"))
			sprintf(cJobName,"%.64s",entries[i].val);
		else if(!strcmp(entries[i].name,"uDatacenter"))
			sscanf(entries[i].val,"%u",&uDatacenter);
		else if(!strcmp(entries[i].name,"uNode"))
			sscanf(entries[i].val,"%u",&uNode);
		else if(!strcmp(entries[i].name,"uContainer"))
			sscanf(entries[i].val,"%u",&uContainer);
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
		else if(!strcmp(entries[i].name,"uRecurringJob"))
			sscanf(entries[i].val,"%u",&uRecurringJob);
		else if(!strcmp(entries[i].name,"cRecurringJobDropDown"))
		{
			sprintf(cRecurringJobDropDown,"%.255s",entries[i].val);
			uRecurringJob=ReadRecurringDropDown(cRecurringJobDropDown);
		}
		else if(!strcmp(entries[i].name,"cRemoteMsg"))
			sprintf(cRemoteMsg,"%.64s",entries[i].val);
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
		else if(!strcmp(entries[i].name,"uMasterJob"))
			sscanf(entries[i].val,"%u",&uMasterJob);

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
				unxsVZ("New tJob table created");
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
				MYSQL_RUN_STORE(res2);
				field=mysql_fetch_row(res2);
				sscanf(field[0],"%lu",&gluRowid);
				gluRowid++;
			}
			PageMachine("",0,"");
			if(!guMode) mysql_data_seek(res,gluRowid-1);
			field=mysql_fetch_row(res);
			sscanf(field[0],"%u",&uJob);
			sprintf(cLabel,"%.100s",field[1]);
			sprintf(cJobName,"%.64s",field[2]);
			sscanf(field[3],"%u",&uDatacenter);
			sscanf(field[4],"%u",&uNode);
			sscanf(field[5],"%u",&uContainer);
			cJobData=field[6];
			sscanf(field[7],"%lu",&uJobDate);
			sscanf(field[8],"%u",&uJobStatus);
			sprintf(cRemoteMsg,"%.64s",field[9]);
			sscanf(field[10],"%u",&uOwner);
			sscanf(field[11],"%u",&uCreatedBy);
			sscanf(field[12],"%lu",&uCreatedDate);
			sscanf(field[13],"%u",&uModBy);
			sscanf(field[14],"%lu",&uModDate);
			sscanf(field[15],"%u",&uMasterJob);

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

	//Custom right panel
	//repeating jobs wizard
	if(guMode==9001)
		tJobNewStep(1);
	else if(guMode==9002)
		tJobNewStep(2);
	else if(guMode==9003)
		tJobNewStep(3);
	else if(guMode==9004)
		tJobNewStep(4);
	//UBC wizard
	else if(guMode==8001)
		tJobNewStep(10);
	else if(guMode==8002)
		tJobNewStep(11);
	else if(guMode==8003)
		tJobNewStep(12);
	//Normal
	else if(guMode==2000 || guMode==2002)
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


void SelectedUBCJobs(unsigned uCreateJobs)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	if(uNode && uContainer)
	{
		sprintf(gcQuery,"SELECT tJob.cJobName,tJob.uJob,tContainer.cLabel,tNode.cLabel,"
				"FROM_UNIXTIME(tJob.uCreatedDate),"
				"tJob.cJobData,tJob.uDatacenter,tJob.uNode,tJob.uContainer"
			" FROM tJob,tNode,tContainer"
			" WHERE tJob.uDatacenter=%u"
			" AND tJob.cJobName='UpdateContainerUBCJob'"
			" AND tJob.cRemoteMsg NOT LIKE 'Rollback job%%'"
			" AND tJob.uNode=tNode.uNode"
			" AND tJob.uContainer=tContainer.uContainer"
			" AND tJob.uNode=%u"
			" AND tJob.uContainer=%u",uDatacenter,uNode,uContainer);
	}
	else if(uNode)
	{
		sprintf(gcQuery,"SELECT tJob.cJobName,tJob.uJob,tContainer.cLabel,tNode.cLabel,"
				"FROM_UNIXTIME(tJob.uCreatedDate),"
				"tJob.cJobData,tJob.uDatacenter,tJob.uNode,tJob.uContainer"
			" FROM tJob,tNode,tContainer"
			" WHERE tJob.uDatacenter=%u"
			" AND tJob.cJobName='UpdateContainerUBCJob'"
			" AND tJob.cRemoteMsg NOT LIKE 'Rollback job%%'"
			" AND tJob.uNode=tNode.uNode"
			" AND tJob.uContainer=tContainer.uContainer"
			" AND tJob.uNode=%u",uDatacenter,uNode);
	}
	else if(1)
	{
		sprintf(gcQuery,"SELECT tJob.cJobName,tJob.uJob,tContainer.cLabel,tNode.cLabel,"
				"FROM_UNIXTIME(tJob.uCreatedDate),"
				"tJob.cJobData,tJob.uDatacenter,tJob.uNode,tJob.uContainer"
			" FROM tJob,tNode,tContainer"
			" WHERE tJob.uDatacenter=%u"
			" AND tJob.cJobName='UpdateContainerUBCJob'"
			" AND tJob.cRemoteMsg NOT LIKE 'Rollback job%%'"
			" AND tJob.uNode=tNode.uNode"
			" AND tJob.uContainer=tContainer.uContainer",uDatacenter);
	}
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	char *cJobMsg;
	unsigned uNewJob=0;
	unsigned uJob=0;
	while((field=mysql_fetch_row(res)))
	{
		cJobMsg="";
		uNewJob=0;
		sscanf(field[1],"%u",&uJob);
		if(uCreateJobs)
		{
			sprintf(gcQuery,"INSERT INTO tJob SET"
				" cLabel='UpdateContainerUBCDownJob',"
				" cJobName='UpdateContainerUBCDownJob',"
				" cJobData='%s',"
				" uDatacenter=%s,uNode=%s,uContainer=%s,"
				" uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW()),uJobDate=UNIX_TIMESTAMP(NOW()),uOwner=%u,uJobStatus=%u"
						,field[5],
						field[6],field[7],field[8],
						guLoginClient,guCompany,uWAITING);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
			{
				htmlPlainTextError(mysql_error(&gMysql));
			}
			else
			{
				cJobMsg="job created";
				uNewJob=mysql_insert_id(&gMysql);
			}
		}
		printf("%s %u %s %s %s %s %s<br>\n",field[0],uJob,field[2],field[3],field[5],field[4],cJobMsg);

		//only allow one rollback for now
		if(uNewJob)
		{
			sprintf(gcQuery,"UPDATE tJob SET"
				" cRemoteMsg='Rollback job(%u)',"
				" uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uJob=%u",
						uNewJob,
						guLoginClient,uJob);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
		}
	}
	mysql_free_result(res);

}//void SelectedUBCJobs()


void tJobNewStep(unsigned uStep)
{

	//rollback UBC wizard
	if(uStep==10)
	{
		OpenRow("Select an available datacenter","black");
		tTablePullDownActiveColorCoded("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,1);

		OpenRow("Select an organization","black");
		tTablePullDownResellers(uForClient,0);
	}
	else if(uStep==11)
	{
		OpenRow("Selected datacenter","black");
		tTablePullDown("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,0);

		OpenRow("Selected organization","black");
		tTablePullDown("tClient;cuClientPullDown","cLabel","cLabel",uForClient,0);

		if(!uNode)
		{
			OpenRow("Optionally restrict to node","black");
			tTablePullDownDatacenter("tNode;cuNodePullDown","cLabel","cLabel",uNode,1,
				cuDatacenterPullDown,0,uDatacenter,0);//0 does not use tProperty, uses uDatacenter
		}
		else
		{
			OpenRow("Selected node","black");
			tTablePullDown("tNode;cuNodePullDown","cLabel","cLabel",uNode,0);
		}

		if(uNode && uContainer)
		{
			OpenRow("Selected container","black");
			tTablePullDown("tContainer;cuContainerPullDown","cLabel","cLabel",uContainer,0);
		}
		else if(uNode)
		{
			OpenRow("Optionally restrict to node container","black");
			tTablePullDownDatacenter("tContainer;cuContainerPullDown","cLabel","cLabel",uContainer,1,
				cuContainerPullDown,0,uDatacenter,0);//0 does not use tProperty, uses uDatacenter
		}
		else if(uContainer==0)
		{
			OpenRow("Optionally restrict to container","black");
			tTablePullDownDatacenter("tContainer;cuContainerPullDown","cLabel","cLabel",uContainer,1,
				cuContainerPullDown,0,uDatacenter,0);//0 does not use tProperty, uses uDatacenter
		}

		OpenRow("Selected jobs","black");
		SelectedUBCJobs(0);

	}
	else if(uStep==12)
	{
		OpenRow("Selected datacenter","black");
		tTablePullDown("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,0);

		OpenRow("Selected organization","black");
		tTablePullDown("tClient;cuClientPullDown","cLabel","cLabel",uForClient,0);

		if(uNode)
		{
			OpenRow("Selected node","black");
			tTablePullDown("tNode;cuNodePullDown","cLabel","cLabel",uNode,0);
		}

		if(uNode && uContainer)
		{
			OpenRow("Selected container","black");
			tTablePullDown("tContainer;cuContainerPullDown","cLabel","cLabel",uContainer,0);
		}

		OpenRow("Selected jobs","black");
		SelectedUBCJobs(1);
	}

	//recurring job wizard
	else if(uStep==1)
	{
		OpenRow("Select an available datacenter","black");
		tTablePullDown("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,1);

		OpenRow("Select an organization","black");
		tTablePullDownResellers(uForClient,0);
	}
	else if(uStep==2)
	{
		OpenRow("Selected datacenter","black");
		tTablePullDown("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,0);

		OpenRow("Selected organization","black");
		tTablePullDown("tClient;cuClientPullDown","cLabel","cLabel",uForClient,0);

		OpenRow("Optionally assign to node","black");
		tTablePullDownDatacenter("tNode;cuNodePullDown","cLabel","cLabel",uNode,1,
			cuDatacenterPullDown,0,uDatacenter,0);//0 does not use tProperty, uses uDatacenter

		OpenRow("Optionally assign to container","black");
		tTablePullDownDatacenter("tContainer;cuContainerPullDown","cLabel","cLabel",uContainer,1,
			cuContainerPullDown,0,uDatacenter,0);//0 does not use tProperty, uses uDatacenter

		OpenRow("Select Recurring Job","black");
		RecurringJobDropDown(uRecurringJob,1);

		OpenRow("Assign a label","black");
		printf("<input title='A useful identification label' type=text name=cLabel"
			" size=40 maxlength=100 value=\"%s\">\n",cLabel);

		OpenRow("Select recurring minute","black");
		printf("<input title='Minute on the hour 0-59' type=text name=uMin"
			" value=0 size=40 maxlength=2 >\n");

		OpenRow("Select recurring hour","black");
		printf("<input title='Hour of the day 0-24. 0 for all hours' type=text name=uHour"
			" value=0 size=40 maxlength=2 >\n");

		OpenRow("Select recurring day of week number","black");
		printf("<input title='Day of the week, range 0-7. Sunday is 1. 0 for all days' type=text name=uDayOfWeek"
			" value=0 size=40 maxlength=1 >\n");

		OpenRow("Select recurring day of month number","black");
		printf("<input title='Day of the month 1-31. 0 for all days. Trumps day of week' type=text name=uDayOfMonth"
			" value=0 size=40 maxlength=1 >\n");

		OpenRow("Select recurring month number","black");
		printf("<input title='Month of the year. 1-12. 0 for all months' type=text name=uMonth"
			" value=0 size=40 maxlength=2 >\n");

		OpenRow("Select starting date","black");
		char cTime[32]={""};
		if(!cStartDate[0])
		{
			time_t luClock;
			const struct tm *tmTime;

			time(&luClock);
			tmTime=localtime(&luClock);
			strftime(cTime,31,"%m/%d/%Y",tmTime);
		}
		else
		{
			sprintf(cTime,"%.31s",cStartDate);
		}
		jsCalendarInput("cStartDate",cTime,1);
	}

}//void tJobNewStep(unsigned uStep)



void tJobInput(unsigned uMode)
{

//uJob
	OpenRow(LANG_FL_tJob_uJob,"black");
	printf("<input title='%s' type=text name=uJob value=%u size=16 maxlength=10 ",LANG_FT_tJob_uJob,uJob);
	printf("disabled></td></tr>\n");
	printf("<input type=hidden name=uJob value=%u >\n",uJob);
//uMasterJob
	OpenRow("uMasterJob","black");
	printf("<input title='uMasterJob is for multi related job control' type=text name=uJob value=%u size=16 maxlength=10 ",uMasterJob);
	printf("disabled></td></tr>\n");
	printf("<input type=hidden name=uJob value=%u >\n",uJob);
//cLabel
	OpenRow(LANG_FL_tJob_cLabel,"black");
	printf("<input title='%s' type=text name=cLabel value=\"%s\" size=40 maxlength=100 ",LANG_FT_tJob_cLabel,
						EncodeDoubleQuotes(cLabel));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cLabel value=\"%s\">\n",EncodeDoubleQuotes(cLabel));
	}
//cJobName
	OpenRow(LANG_FL_tJob_cJobName,"black");
	printf("<input title='%s' type=text name=cJobName value=\"%s\" size=40 maxlength=64 ",LANG_FT_tJob_cJobName,
						EncodeDoubleQuotes(cJobName));
	if(guPermLevel>=0 && uMode)
	{
		printf("></td></tr>\n");
	}
	else
	{
		printf("disabled></td></tr>\n");
		printf("<input type=hidden name=cJobName value=\"%s\">\n",EncodeDoubleQuotes(cJobName));
	}
//uDatacenter
	OpenRow("uDatacenter","black");
	if(guPermLevel>=0 && uMode)
	{
		printf("%s<input type=hidden name=uDatacenter value=%u >\n",
			ForeignKey("tDatacenter","cLabel",uDatacenter),uDatacenter);
	}
	else
	{
		printf("%s<input type=hidden name=uDatacenter value=%u >\n",
			ForeignKey("tDatacenter","cLabel",uDatacenter),uDatacenter);
	}
//uNode
	OpenRow("uNode","black");
	printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tNode&uNode=%u>%s</a><input type=hidden name=uNode value=%u >\n",
				uNode,ForeignKey("tNode","cLabel",uNode),uNode);
//uContainer
	OpenRow("uContainer","black");
	printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%u>%s</a><input type=hidden name=uContainer value=%u >\n",
				uContainer,ForeignKey("tContainer","cLabel",uContainer),uContainer);
//cJobData
	OpenRow(LANG_FL_tJob_cJobData,"black");
	printf("<textarea title='%s' cols=80 wrap=soft rows=8 name=cJobData ",LANG_FT_tJob_cJobData);
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
		printf("<input type=text name=cuJobDate value='%s' size=40 disabled>\n",ctime(&uJobDate));
	else
		printf("<input type=text name=cuJobDate value='---' size=40 disabled>\n");
	printf("<input type=hidden name=uJobDate value=%lu>\n",uJobDate);
//uJobStatus
	OpenRow(LANG_FL_tJob_uJobStatus,"black");
	if(guPermLevel>=10 && uMode)
		tTablePullDown("tJobStatus;cuJobStatusPullDown","cLabel","cLabel",uJobStatus,1);
	else
		tTablePullDown("tJobStatus;cuJobStatusPullDown","cLabel","cLabel",uJobStatus,0);
//cRemoteMsg
	OpenRow(LANG_FL_tJob_cRemoteMsg,"black");
	printf("<input title='%s' type=text name=cRemoteMsg value=\"%s\" size=80 maxlength=64 ",LANG_FT_tJob_cRemoteMsg,
							EncodeDoubleQuotes(cRemoteMsg));
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
		printf("%s<input type=hidden name=uCreatedBy value=%u >\n",
				ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
	}
	else
	{
		printf("%s<input type=hidden name=uCreatedBy value=%u >\n",
				ForeignKey("tClient","cLabel",uCreatedBy),uCreatedBy);
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

	sprintf(gcQuery,"SELECT uJob FROM tJob WHERE uJob=%u",uJob);
	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	if(i) 
		//tJob("<blink>Record already exists");
		tJob(LANG_NBR_RECEXISTS);

	//insert query
	Insert_tJob();
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(gcQuery,"New record %u added");
	uJob=mysql_insert_id(&gMysql);
	uCreatedDate=luGetCreatedDate("tJob",uJob);
	unxsVZLog(uJob,"tJob","New");

	if(!uMode)
	{
		sprintf(gcQuery,LANG_NBR_NEWRECADDED,uJob);
		tJob(gcQuery);
	}

}//NewtJob(unsigned uMode)


void DeletetJob(void)
{
	sprintf(gcQuery,"DELETE FROM tJob WHERE uJob=%u AND ( uOwner=%u OR %u>9 )"
					,uJob,guLoginClient,guPermLevel);
	MYSQL_RUN;
	//tJob("Record Deleted");
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsVZLog(uJob,"tJob","Del");
		tJob(LANG_NBR_RECDELETED);
	}
	else
	{
		unxsVZLog(uJob,"tJob","DelError");
		tJob(LANG_NBR_RECNOTDELETED);
	}

}//void DeletetJob(void)


void Insert_tJob(void)
{

	//insert query
	sprintf(gcQuery,"INSERT INTO tJob SET uJob=%u,cLabel='%s',cJobName='%s',uDatacenter=%u,uNode=%u,uContainer=%u,"
			"cJobData='%s',uJobDate=%lu,uJobStatus=%u,cRemoteMsg='%s',uOwner=%u,uCreatedBy=%u,"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uJob
			,TextAreaSave(cLabel)
			,TextAreaSave(cJobName)
			,uDatacenter
			,uNode
			,uContainer
			,TextAreaSave(cJobData)
			,uJobDate
			,uJobStatus
			,TextAreaSave(cRemoteMsg)
			,uOwner
			,uCreatedBy
			);

	MYSQL_RUN;
	
}//void Insert_tJob(void)


void Update_tJob(char *cRowid)
{

	//update query
	sprintf(gcQuery,"UPDATE tJob SET uJob=%u,cLabel='%s',cJobName='%s',uDatacenter=%u,uNode=%u,uContainer=%u,"
			"cJobData='%s',uJobDate=%lu,uJobStatus=%u,cRemoteMsg='%s',uModBy=%u,"
			"uModDate=UNIX_TIMESTAMP(NOW()) WHERE _rowid=%s",
			uJob
			,TextAreaSave(cLabel)
			,TextAreaSave(cJobName)
			,uDatacenter
			,uNode
			,uContainer
			,TextAreaSave(cJobData)
			,uJobDate
			,uJobStatus
			,TextAreaSave(cRemoteMsg)
			,uModBy
			,cRowid);

	MYSQL_RUN;

}//void Update_tJob(void)


void ModtJob(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	if(guPermLevel<10)
	sprintf(gcQuery,"SELECT tJob.uJob,"
				"tJob.uModDate"
				"FROM tJob,tClient"
				"WHERE tJob.uJob=%u"
				"AND tJob.uOwner=tClient.uClient"
				"AND (tClient.uOwner=%u OR tClient.uClient=%u)"
					,uJob,guLoginClient,guLoginClient);
	else
	sprintf(gcQuery,"SELECT uJob,uModDate FROM tJob WHERE uJob=%u",uJob);

	MYSQL_RUN_STORE(res);
	i=mysql_num_rows(res);

	//if(i<1) tJob("<blink>Record does not exist");
	if(i<1) tJob(LANG_NBR_RECNOTEXIST);
	//if(i>1) tJob("<blink>Multiple rows!");
	if(i>1) tJob(LANG_NBR_MULTRECS);

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate) tJob(LANG_NBR_EXTMOD);

	Update_tJob(field[0]);
	if(mysql_errno(&gMysql)) htmlPlainTextError(mysql_error(&gMysql));
	//sprintf(query,"record %s modified",field[0]);
	sprintf(gcQuery,LANG_NBRF_REC_MODIFIED,field[0]);
	uModDate=luGetModDate("tJob",uJob);
	unxsVZLog(uJob,"tJob","Mod");
	tJob(gcQuery);

}//ModtJob(void)


void tJobList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	ExttJobListSelect();

	MYSQL_RUN_STORE(res);
	guI=mysql_num_rows(res);

	PageMachine("tJobList",1,"");//1 is auto header list guMode. Opens table!

	//Filter select drop down
	ExttJobListFilter();

	printf("<input type=text size=16 name=gcCommand maxlength=98 value=\"%s\" >",gcCommand);

	printf("</table>\n");

	printf("<table bgcolor=#9BC1B3 border=0 width=100%%>\n");
	printf("<tr bgcolor=black>"
		"<td><font face=arial,helvetica color=white>uJob"
		"<td><font face=arial,helvetica color=white>cLabel"
		"<td><font face=arial,helvetica color=white>cJobName"
		"<td><font face=arial,helvetica color=white>uDatacenter"
		"<td><font face=arial,helvetica color=white>uNode"
		"<td><font face=arial,helvetica color=white>uContainer"
		"<td><font face=arial,helvetica color=white>cJobData"
		"<td><font face=arial,helvetica color=white>uJobDate"
		"<td><font face=arial,helvetica color=white>uJobStatus"
		"<td><font face=arial,helvetica color=white>cRemoteMsg"
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
		printf("<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tJob&uJob=%s>%s</a>"
			"<td>%s"
			"<td>%s"
			"<td>%s"
			"<td>%s"
			"<td><a class=darkLink href=unxsVZ.cgi?gcFunction=tContainer&uContainer=%s>%s</a>"
			"<td><textarea disabled>%s</textarea>"
			"<td>%s"
			"<td>%s"
			"<td>%s"
			"<td>%s"
			"<td>%s"
			"<td>%s"
			"<td>%s"
			"<td>%s</tr>"
			,field[0],field[0]
			,field[1]
			,field[2]
			,ForeignKey("tDatacenter","cLabel",strtoul(field[3],NULL,10))
			,ForeignKey("tNode","cLabel",strtoul(field[4],NULL,10))
			,field[5],ForeignKey("tContainer","cLabel",strtoul(field[5],NULL,10))
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
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tJob ( "
			"uJob INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			"cLabel VARCHAR(100) NOT NULL DEFAULT '',"
			"cJobName VARCHAR(64) NOT NULL DEFAULT '',"
			"uDatacenter INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uDatacenter),"
			"uNode INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uNode),"
			"uContainer INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uContainer),"
			"cJobData TEXT NOT NULL DEFAULT '',"
			"uJobDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uJobStatus INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uJobStatus),"
			"cRemoteMsg VARCHAR(64) NOT NULL DEFAULT '',"
			"uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner),"
			"uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModBy INT UNSIGNED NOT NULL DEFAULT 0,"
			"uModDate INT UNSIGNED NOT NULL DEFAULT 0,"
			"uMasterJob INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uMasterJob))");
	MYSQL_RUN;
}//CreatetJob()


/*
candidate for non use removal
void PropertyDropDown(const char *cTableName, const char *cFieldName,
		const char *cOrderby, unsigned uSelector, unsigned uMode, const char *cDatacenter,
		unsigned uType, unsigned uDatacenter)
{
        register int i,n;
        char cLabel[256];
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        char cSelectName[100]={""};
	char cHidden[100]={""};
        char cLocalTableName[256]={""};
        char *cp;
	char *cMode="";

	if(!uMode)
		cMode="disabled";
      
        if(!cTableName[0] || !cFieldName[0] || !cOrderby[0])
        {
                printf("Invalid input PropertyDropDown()()");
                return;
        }

        //Extended functionality
        sprintf(cLocalTableName,"%.255s",cTableName);
        if((cp=strchr(cLocalTableName,';')))
        {
                sprintf(cSelectName,"%.99s",cp+1);
                *cp=0;
        }


	if(uType)
		//UBC safe
	       	sprintf(gcQuery,"SELECT _rowid AS uRowid,%s FROM %s WHERE"
			" LOCATE('All Datacenters',"
			"(SELECT cValue FROM tProperty WHERE cName='cDatacenter' AND uType=%u AND uKey=uRowid))>0"
			" OR LOCATE('%s',"
			"(SELECT cValue FROM tProperty WHERE cName='cDatacenter' AND uType=%u AND uKey=uRowid))>0"
			" ORDER BY %s",
				cFieldName,cLocalTableName,uType,cDatacenter,uType,cOrderby);
	else
	       	sprintf(gcQuery,"SELECT _rowid,%s FROM %s WHERE uDatacenter=%u ORDER BY %s",
				cFieldName,cLocalTableName,uDatacenter,cOrderby);
	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	i=mysql_num_rows(mysqlRes);

	if(cSelectName[0])
                sprintf(cLabel,"%s",cSelectName);
        else
                sprintf(cLabel,"%s_%sPullDown",cLocalTableName,cFieldName);

        if(i>0)
        {
		int unsigned field0;
                printf("<select name=%s %s>\n",cLabel,cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {

			field0=0;
                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                        {
                             printf("<option>%s</option>\n",mysqlField[1]);
                        }
                        else
                        {
                             printf("<option selected>%s</option>\n",mysqlField[1]);
			     if(!uMode)
			     sprintf(cHidden,"<input type=hidden name=%.99s value='%.99s'>\n",
			     		cLabel,mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=%s %s><option title='No selection'>---</option></select>\n"
                        ,cLabel,cMode);
		if(!uMode)
		sprintf(cHidden,"<input type=hidden name=%99s value='0'>\n",cLabel);
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//PropertyDropDown()
*/

void RecurringJobDropDown(unsigned uSelector, unsigned uMode)
{
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;
	register int i,n;
	char *cMode="";
	char cHidden[256]={""};

	if(!uMode)
		cMode="disabled";
     
	//UBC safe 
	sprintf(gcQuery,"SELECT uProperty,cName FROM tProperty WHERE uType=%u and uKey=0",uPROP_RECJOB);

	MYSQL_RUN_STORE_TEXT_RET_VOID(mysqlRes);
	i=mysql_num_rows(mysqlRes);

        if(i>0)
        {
		int unsigned field0;
                printf("<select name=cRecurringJobDropDown %s>\n",cMode);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {

			field0=0;
                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                        {
                             printf("<option>%s</option>\n",mysqlField[1]);
                        }
                        else
                        {
                             printf("<option selected>%s</option>\n",mysqlField[1]);
			     if(!uMode)
				sprintf(cHidden,"<input type=hidden name=cRecurringJobDropDown value='%.99s'>\n",
					mysqlField[1]);
                        }
                }
        }
        else
        {
		printf("<select name=cRecurringJobDropDown %s><option title='No selection'>---</option></select>\n",cMode);
		if(!uMode)
			sprintf(cHidden,"<input type=hidden name=cRecurringJobDropDown value='0'>\n");
        }
        printf("</select>\n");
	if(cHidden[0])
		printf("%s",cHidden);

}//void RecurringJobDropDown(unsigned uSelector, unsigned uMode)


unsigned ReadRecurringDropDown(char *cRecurringJobDropDown)
{
        MYSQL_RES *mysqlRes;
        MYSQL_ROW mysqlField;

        unsigned uRowid=0;//Not found

	//UBC safe
        sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uKey=0 AND uType=%u AND cName='%.99s'",
			uPROP_RECJOB,cRecurringJobDropDown);
        MYSQL_RUN_STORE(mysqlRes);
        if((mysqlField=mysql_fetch_row(mysqlRes)))
        	sscanf(mysqlField[0],"%u",&uRowid);
        mysql_free_result(mysqlRes);
        return(uRowid);

}//unsigned ReadRecurringDropDown(char *cRecurringJobDropDown)
