/*
FILE
	$Id$
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR/LEGAL
	(C) 2001-2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file.
 
*/

//ModuleFunctionProtos()

static unsigned uForClient=0;
static char cForClientPullDown[256]={""};
static char cuClientPullDown[256]={""};
static char cuDatacenterPullDown[256]={""};
static char cuNodePullDown[256]={""};
static char cuContainerPullDown[256]={""};
static unsigned uMin=0,uHour=0,uDayOfWeek=0,uDayOfMonth=0,uMonth=0;
static char cStartDate[32]={""};
time_t luStartDate=0;

void tJobNavList(void);
char *strptime(const char *s, const char *format, struct tm *tm);
time_t cStartDateToUnixTime(char *cStartDate);

void ExtProcesstJobVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cuDatacenterPullDown"))
		{
			sprintf(cuDatacenterPullDown,"%.255s",entries[i].val);
			uDatacenter=ReadPullDown("tDatacenter","cLabel",cuDatacenterPullDown);
		}
		else if(!strcmp(entries[i].name,"uForClient"))
			sscanf(entries[i].val,"%u",&uForClient);
		else if(!strcmp(entries[i].name,"cuClientPullDown"))
		{
			sprintf(cuClientPullDown,"%.255s",entries[i].val);
			uForClient=ReadPullDown("tClient","cLabel",cuClientPullDown);
		}
		else if(!strcmp(entries[i].name,"cuNodePullDown"))
		{
			sprintf(cuNodePullDown,"%.255s",entries[i].val);
			uNode=ReadPullDown("tNode","cLabel",cuNodePullDown);
		}
		else if(!strcmp(entries[i].name,"cuContainerPullDown"))
		{
			sprintf(cuContainerPullDown,"%.255s",entries[i].val);
			uContainer=ReadPullDown("tContainer","cLabel",cuContainerPullDown);
		}
		else if(!strcmp(entries[i].name,"cForClientPullDown"))
		{
			sprintf(cForClientPullDown,"%.255s",entries[i].val);
			uForClient=ReadPullDown("tClient","cLabel",cForClientPullDown);
		}
		else if(!strcmp(entries[i].name,"uMin"))
			sscanf(entries[i].val,"%u",&uMin);
		else if(!strcmp(entries[i].name,"uHour"))
			sscanf(entries[i].val,"%u",&uHour);
		else if(!strcmp(entries[i].name,"uDayOfWeek"))
			sscanf(entries[i].val,"%u",&uDayOfWeek);
		else if(!strcmp(entries[i].name,"uDayOfMonth"))
			sscanf(entries[i].val,"%u",&uDayOfMonth);
		else if(!strcmp(entries[i].name,"uMonth"))
			sscanf(entries[i].val,"%u",&uMonth);
		else if(!strcmp(entries[i].name,"cStartDate"))
			sprintf(cStartDate,"%.31s",entries[i].val);
	}
}//void ExtProcesstJobVars(pentry entries[], int x)


void ExttJobCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tJobTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstJobVars(entries,x);
                        	guMode=2000;
	                        tJob(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstJobVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uJob=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtJob(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstJobVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tJob(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstJobVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetJob();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstJobVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tJob(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstJobVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtJob();
			}
                }

		//Recurring Job Wizard
		else if(!strcmp(gcCommand,"Recurring Job Wizard"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstJobVars(entries,x);
                        	guMode=9001;
	                        tJob("Recurring job wizard step 1");
			}
			else
			{
				tJob("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Select Datacenter/Org"))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstJobVars(entries,x);
                        	guMode=9001;
				if(!uDatacenter)
					tJob("<blink>Error:</blink> Must select a datacenter.");
				if(!uForClient)
					tJob("<blink>Error:</blink> Must select an organization"
							" (company, NGO or similar.)");
                        	guMode=9002;
	                        tJob("Recurring job wizard step 2");
			}
			else
			{
				tJob("<blink>Error:</blink> Denied by permissions settings");
			}
                }
		else if(!strcmp(gcCommand,"Confirm Fields"))
                {
			if(guPermLevel>=9)
			{

				unsigned uYear=0,uMon=0,uDay=0;
				char cBuffer[512];

                        	ProcesstJobVars(entries,x);
                        	guMode=9002;
				if(!uDatacenter)
					tJob("<blink>Error:</blink> Must select a datacenter.");
				if(!uForClient)
					tJob("<blink>Error:</blink> Must select an organization"
							" (company, NGO or similar.)");
				if(uContainer && !uNode)
					tJob("<blink>Unexpected error:</blink> Must select a node if selecting container!");
				if(uNode)
				{
					unsigned uNodeDatacenter=0;

					sscanf(ForeignKey("tNode","uDatacenter",uNode),"%u",&uNodeDatacenter);
					if(uDatacenter!=uNodeDatacenter)
						tJob("<blink>Unexpected error:</blink> The specified node does not "
							"belong to the specified datacenter!");
				}
				if(uNode && uContainer)
				{
					unsigned uNodeContainer=0;

					sscanf(ForeignKey("tContainer","uNode",uContainer),"%u",&uNodeContainer);
					if(uNode!=uNodeContainer)
						tJob("<blink>Error:</blink> The specified container does not "
							"belong to the specified node.");
				}
				if(!uRecurringJob)
					tJob("<blink>Error:</blink> Must specify a recurring job.");
				if(!cLabel[0])
					tJob("<blink>Error:</blink> Must specify a cLabel.");
				if(uMin>59)
					tJob("<blink>Error:</blink> Must specify a minute from 0-59.");
				if(uHour>24)
					tJob("<blink>Error:</blink> Must specify an hour from 0-24. 0 for all hours.");
				if(uDayOfWeek>7)
					tJob("<blink>Error:</blink> Must specify a day of week number from 0 to 7."
						" 0 for all days. Sunday is day 1.");
				if(uDayOfMonth>31)
					tJob("<blink>Error:</blink> Must specify a day of month number from 0-31."
						" 0 for all days. If not 0 this value trumps and day of week number set.");
				if(uMonth>12)
					tJob("<blink>Error:</blink> Must specify a month number from 0-12."
						" 0 for all months.");
				if(!cStartDate[0])
					tJob("<blink>Error:</blink> Must specify a year-month-day start date for "
								"initial job.");

				sscanf(cStartDate,"%u-%u-%u",&uYear,&uMon,&uDay);
				if(uYear>3010 || uYear<2010)
					tJob("<blink>Error:</blink> Year out-of-range, ex. 2010-01-22");
				if(uMon>12 || uMon<1)
					tJob("<blink>Error:</blink> Mon out-of-range, ex. 2010-01-22");
				if(uDay>31 || uDay<1)
					tJob("<blink>Error:</blink> Day out-of-range, ex. 2010-01-22");

				luStartDate=cStartDateToUnixTime(cStartDate);
				if(luStartDate == (time_t)(-1))
					tJob("<blink>Unexpected error:</blink> mktime() failed!");
				if(!luStartDate)
					tJob("<blink>Unexpected error:</blink> luStartDate==0!");

				uJob=0;
				guMode=0;
				uOwner=uForClient;
				uCreatedBy=guLoginClient;
				sprintf(cJobName,"RecurringJob");
				cJobData=cBuffer;
				uJobStatus=uWAITING;
				sprintf(cJobData,"uMin=%u;\nuHour=%u;\nuDayOfWeek=%u;\nuDayOfMonth=%u;\nuMonth=%u;\n"
						"cRecurringJob=%s;\n",
						uMin,uHour,uDayOfWeek,uDayOfMonth,uMonth,cRecurringJobDropDown);
				if(uHour==24) uHour=0;//Adjust for start after posting correct value above.
				luStartDate+=(uMin*60)+(uHour*3600);
				uJobDate=luStartDate;
				NewtJob(1);
				if(uJob)
	                        	tJob("Recurring job added");
				else
					tJob("<blink>Unexpected error:</blink> uJob==0!");
			}
		}
		else if(!strcmp(gcCommand,"Cancel"))
                {
			guMode=0;
		}
	}

}//void ExttJobCommands(pentry entries[], int x)


void ExttJobButtons(void)
{
	OpenFieldSet("tJob Aux Panel",100);
	switch(guMode)
        {
                case 9001:
			printf("<u>Recurring Job Datacenter and Owner</u><br>");
			printf("Select the datacenter and the company/organization (owner) of the job"
				" to be created.<p>");
			printf("<input type=submit class=largeButton title='Select datacenter and organization'"
				" name=gcCommand value='Select Datacenter/Org'>\n");
			printf("<p><input type=submit class=largeButton title='Cancel this operation'"
				" name=gcCommand value='Cancel'>\n");
                break;

                case 9002:
			printf("<u>Recurring Job Data</u><br>");
			printf("Enter the recurring job data."
				"<p>Starting date has to be in year-mon-day number format."
				"<p>Recurring hour/week-day/month-day and month numbers use number 0 as wildcard."
				" Day of month number trumps day of week number if used."
				"<p>The job label should be meaningful.<p>");
			printf("<input type=submit class=largeButton title='Select hardware node'"
				" name=gcCommand value='Confirm Fields'>\n");
			printf("<p><input type=submit class=largeButton title='Cancel this operation'"
				" name=gcCommand value='Cancel'>\n");
                break;

                case 2000:
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review changes</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("This is the job queue table. A good way to scale these infrastructure management"
				" systems is to use asynchronous job queues. Everything that needs to get done is queued"
				" here and then executed by the jobqueue.c on local or remote servers."
				"<p>Most jobs are created in their own areas, an exception are recurring jobs that are"
				" added here.");
			printf("<p><input title='Create a recurring job for containers, nodes or datacenters.'"
					" type=submit class=largeButton"
					" name=gcCommand value='Recurring Job Wizard'>\n");
			printf("<p><u>Record Context Info</u><br>");
			if(uJob)
				tJobNavList();
	}
	CloseFieldSet();

}//void ExttJobButtons(void)


void ExttJobAuxTable(void)
{

}//void ExttJobAuxTable(void)


void ExttJobGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uJob"))
		{
			sscanf(gentries[i].val,"%u",&uJob);
			guMode=6;
		}
	}
	tJob("");

}//void ExttJobGetHook(entry gentries[], int x)


void ExttJobSelect(void)
{
	ExtSelect("tJob",VAR_LIST_tJob);

}//void ExttJobSelect(void)


void ExttJobSelectRow(void)
{
	ExtSelectRow("tJob",VAR_LIST_tJob,uJob);

}//void ExttJobSelectRow(void)


void ExttJobListSelect(void)
{
	char cCat[512];

	ExtListSelect("tJob",VAR_LIST_tJob);
	
	//Changes here must be reflected below in ExttJobListFilter()
        if(!strcmp(gcFilter,"uJob"))
        {
                sscanf(gcCommand,"%u",&uJob);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tJob.uJob=%u",uJob);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uContainer"))
        {
                sscanf(gcCommand,"%u",&uContainer);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tJob.uContainer=%u ORDER BY uJob",uContainer);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cLabel"))
        {
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tJob.cLabel LIKE '%s' ORDER BY uJob",gcCommand);
		strcat(gcQuery,cCat);
	}
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uJob");
        }

}//void ExttJobListSelect(void)


void ExttJobListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uJob"))
                printf("<option>uJob</option>");
        else
                printf("<option selected>uJob</option>");
        if(strcmp(gcFilter,"uContainer"))
                printf("<option>uContainer</option>");
        else
                printf("<option selected>uContainer</option>");
        if(strcmp(gcFilter,"cLabel"))
                printf("<option>cLabel</option>");
        else
                printf("<option selected>cLabel</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttJobListFilter(void)


void ExttJobNavBar(void)
{
	if(guMode>=9000)
		return;

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=12 && !guListMode)
		printf(LANG_NBB_NEW);

	if(uAllowMod(uOwner,uCreatedBy) && !guListMode)
		printf(LANG_NBB_MODIFY);

	if(uAllowDel(uOwner,uCreatedBy) && !guListMode)
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttJobNavBar(void)


void tJobNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	//More magic numbers
/*
#define uWAITING 	1
#define uRUNNING 	2
#define uDONEOK		3
#define uDONEERROR	4
#define uSUSPENDED	5
#define uREDUNDANT	6
#define uCANCELED	7
#define uREMOTEWAITING	10
#define uERROR		14
*/
	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT tJob.uJob,tJob.cLabel,tNode.cLabel,tJobStatus.cLabel FROM tJob,tNode,tClient,tJobStatus WHERE"
				" tJob.uOwner=tClient.uClient AND tJob.uNode=tNode.uNode AND"
				" tJob.uJobStatus=tJobStatus.uJobStatus AND"
				" (tClient.uOwner=%u OR tClient.uClient=%u) AND"
				" (tJob.uJobStatus=1 OR tJob.uJobStatus=14 OR tJob.uJobStatus=2 OR"
				" tJob.uJobStatus=10 OR tJob.uJobStatus=4)",
						guLoginClient,guLoginClient);
	else
	        sprintf(gcQuery,"SELECT tJob.uJob,tJob.cLabel,tNode.cLabel,tJobStatus.cLabel FROM tJob,tNode,tJobStatus"
					" WHERE tJob.uNode=tNode.uNode AND"
					" tJob.uJobStatus=tJobStatus.uJobStatus AND"
					" (tJob.uJobStatus=1 OR tJob.uJobStatus=14 OR tJob.uJobStatus=2 OR"
					" tJob.uJobStatus=10 OR tJob.uJobStatus=4)"
													);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tJobNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tJobNavList (Waiting and Error)</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tJob&uJob=%s>%s/%s/%s</a><br>\n",
				field[0],field[1],field[2],field[3]);
	}
        mysql_free_result(res);

}//void tJobNavList(void)


time_t cStartDateToUnixTime(char *cStartDate)
{
        struct  tm locTime;
        time_t  res;

        bzero(&locTime, sizeof(struct tm));
	if(strchr(cStartDate,'-'))
        	strptime(cStartDate,"%Y-%m-%d", &locTime);
        locTime.tm_sec = 0;
        locTime.tm_min = 0;
        locTime.tm_hour = 0;
        res = mktime(&locTime);
        return(res);
}//time_t cStartDateToUnixTime(char *cDate)
