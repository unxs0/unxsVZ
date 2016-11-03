/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author: (C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details

 
*/

#include "mysqlisp.h"

void JobList(void);
void RecentJobList(void);
#ifndef DEBUG_REPORT_STATS_OFF
	int UpdateInfo();
#endif

void ExtProcesstJobVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstJobVars(pentry entries[], int x)


void ExttJobCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tJobTools"))
	{
		//ModuleFunctionProcess()

		//Default wizard like two step creation and deletion
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstJobVars(entries,x);
                        	guMode=2000;
	                        tJob(LANG_NB_CONFIRMNEW);
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");   
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
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstJobVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
	                        guMode=2001;
				tJob(LANG_NB_CONFIRMDEL);
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstJobVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
				guMode=5;
				DeletetJob();
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstJobVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tJob(LANG_NB_CONFIRMMOD);
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");   
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
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");   
                }
	}

}//void ExttJobCommands(pentry entries[], int x)


void ExttJobButtons(void)
{
	OpenFieldSet("tJob Aux Panel",100);
	
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

	}

	JobList();//Customers with waiting jobs
	RecentJobList();//Recent jobs deployed etc.

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
	ExtSelect("tJob",VAR_LIST_tJob,0);
	
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
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tJob.uJob=%u ORDER BY uJob",uJob);
		strcat(gcQuery,cCat);
        }
	else if(!strcmp(gcFilter,"uJobClient"))
        {
                sscanf(gcCommand,"%u",&uJob);//Note more than one use
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tJob.uJobClient=%u ORDER BY uJob",uJob);
		strcat(gcQuery,cCat);
        }
	else if(!strcmp(gcFilter,"uJobStatus"))
        {
                sscanf(gcCommand,"%u",&uJobStatus);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tJob.uJobStatus=%u ORDER BY uJob",uJobStatus);
		strcat(gcQuery,cCat);
        }
	else if(!strcmp(gcFilter,"cJobName"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tJob.cJobName LIKE '%s' ORDER BY tJob.cJobName",gcCommand);
		strcat(gcQuery,cCat);
        }
	else if(!strcmp(gcFilter,"cServer"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tJob.cServer='%s' ORDER BY tJob.cServer",gcCommand);
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
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uJob"))
                printf("<option>uJob</option>");
        else
                printf("<option selected>uJob</option>");
        if(strcmp(gcFilter,"uJobClient"))
                printf("<option>uJobClient</option>");
        else
                printf("<option selected>uJobClient</option>");
        if(strcmp(gcFilter,"uJobStatus"))
                printf("<option>uJobStatus</option>");
        else
                printf("<option selected>uJobStatus</option>");
        if(strcmp(gcFilter,"cJobName"))
                printf("<option>cJobName</option>");
        else
                printf("<option selected>cJobName</option>");
        if(strcmp(gcFilter,"cServer"))
                printf("<option>cServer</option>");
        else
                printf("<option selected>cServer</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttJobListFilter(void)


void ExttJobNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

	if(uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);

	if(uAllowDel(uOwner,uCreatedBy)) 
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttJobNavBar(void)


void RecentJobList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	time_t u24HrsAgo;
	register unsigned uCount=0;
	unsigned uMyJobStatus=0;
	
	time(&u24HrsAgo);
	u24HrsAgo-= (24*3600);

	if(guLoginClient!=1 && guPermLevel<11)
		sprintf(gcQuery,"SELECT tJob.uJob"
				",tJob.cJobName," TCLIENT ".cLabel,tJob.uJobStatus "
				"FROM tJob," TCLIENT
                                " WHERE tJob.uOwner=" TCLIENT".uClient " 
				"AND tJob.uModDate>=%1$lu AND "
				"("TCLIENT".uClient=%2$u OR " TCLIENT ".uOwner IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%2$u)) AND "
			        "tJob.uJobClient=" TCLIENT ".uClient "
				"AND tJob.uJobClient=%2$u "
				"AND tJob.uJobStatus!=%3$u AND tJob.uJobStatus!=%4$u "
				"AND tJob.uModDate>=%1$lu ORDER BY uJobGroup DESC,uJob DESC LIMIT 101",
				u24HrsAgo
				,guCompany
				,unxsISP_Waiting
				,unxsISP_RemotelyQueued);
	else
	        sprintf(gcQuery,"SELECT tJob.uJob,tJob.cJobName," TCLIENT ".cLabel,tJob.uJobStatus FROM "
				"tJob," TCLIENT " WHERE tClient.uClient=tJob.uJobClient AND "
				"tJob.uModDate>=%lu AND tJob.uJobStatus!=%u AND tJob.uJobStatus!=%u "
				"ORDER BY uJobGroup DESC,uJob DESC LIMIT 101",
				u24HrsAgo,unxsISP_Waiting,unxsISP_RemotelyQueued);

        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		char *cColor="";
        	printf("<p><u>Recently Deployed Jobs</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
			sscanf(field[3],"%u",&uMyJobStatus);
			//if(uJobStatus==unxsISP_FatalError)
			if(uMyJobStatus!=4)
				cColor="red";
			else
				cColor="black";
				printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tJob&uJob=%s>"
					"<font color=%s>%s %s</font></a><br>\n",
						field[0],cColor,field[2],field[1]);
			uCount++;
	        }
		if(uCount>100)
			printf("More than 100 recent jobs...wow! Only first 101 shown<br>\n");
	}
        mysql_free_result(res);

}//void RecentJobList(void)



void JobList(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	MYSQL_RES *res2;
	MYSQL_ROW field2;
	unsigned uJobStatus=0;
	time_t uJobDate=(-1);
	char *cColor;
	char *cPostAtr;
	time_t clock;
	register unsigned uClientCount=0;
	register unsigned uJobCount;

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT DISTINCT " TCLIENT ".cLabel,tJob.uJobClient FROM tJob," TCLIENT " WHERE "
				"tJob.uJobClient=" TCLIENT ".uClient AND ( tJob.uJobStatus=%u OR tJob.uJobStatus=%u) LIMIT 101",
				unxsISP_Waiting,unxsISP_RemotelyQueued);
	else
		sprintf(gcQuery,"SELECT DISTINCT " TCLIENT ".cLabel,tJob.uJobClient FROM tJob," TCLIENT " WHERE "
				"tJob.uJobClient=" TCLIENT ".uClient AND ( tJob.uJobStatus=%1$u OR tJob.uJobStatus=%2$u ) "
				"AND (tJob.uOwner=%3$u OR " TCLIENT ".uOwner IN (SELECT uClient FROM " TCLIENT " WHERE uOwner=%3$u)"
				" OR tJob.uJobClient=%3$u) LIMIT 101",
				unxsISP_Waiting,unxsISP_RemotelyQueued,guCompany);

	/*printf("%s",gcQuery);
	retur/;*/
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("q1: %s",mysql_error(&gMysql));
		return;
	}

	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
	 printf("<p><u>Customers With Waiting Jobs</u><br>");
	 time(&clock);
	 while((field=mysql_fetch_row(res)) && uClientCount<101)
	 {
		printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tClient&uClient=%s>%s</a>:<br>\n", field[1],field[0]);

		sprintf(gcQuery,"SELECT uJob,cJobName,uJobStatus,uJobDate FROM tJob WHERE uJobClient=%s AND "
				"( uJobStatus=%u OR uJobStatus=%u ) ORDER BY uJob DESC LIMIT 11",field[1],unxsISP_Waiting,unxsISP_RemotelyQueued);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
		{
			printf("%s",mysql_error(&gMysql));
			return;
		}
		res2=mysql_store_result(&gMysql);
		uJobCount=0;
		while((field2=mysql_fetch_row(res2)) && uJobCount<11)
		{
			sscanf(field2[3],"%lu",&uJobDate);
			if(uJobDate>(clock+86400))//More than 24 hours in the future
				cPostAtr="*";
			else
				cPostAtr="";
			sscanf(field2[2],"%u",&uJobStatus);
			if(uJobStatus==unxsISP_RemotelyQueued)
				cColor="green";
			else
				cColor="gray";
			printf("&nbsp;&nbsp;<a class=darkLink href=unxsISP.cgi?gcFunction=tJob&uJob=%s><font color=%s>%s%s</font></a><br>\n",
				field2[0],cColor,field2[1],cPostAtr);
			uJobCount++;
		}
		if(uJobCount>10)
			printf("More than 10 waiting client jobs ...wow! Only first 11 shown<br>\n");
		uClientCount++;
		mysql_free_result(res2);
		//printf("<br>\n");
	 }
		if(uClientCount>100)
			printf("More than 100 clients with jobs ...wow! Only first 101 shown<br>\n");
	}
	mysql_free_result(res);

}//void JobList(void)


#ifndef DEBUG_REPORT_STATS_OFF

int UpdateInfo(void)
{
	register int sd, rc;
	struct sockaddr_in cliAddr, remoteServAddr;
	struct hostent *h;
	time_t luClock=0,luModDate=0;

	MYSQL_RES *res;
	MYSQL_ROW field;

	char cInfo[128]={"Emtpy"};
	unsigned uMaxuZone=0;

	time(&luClock);

	sprintf(gcQuery,"SELECT uModDate FROM tConfiguration WHERE cLabel='UpdateInfo'");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return(1);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%lu",&luModDate);
	mysql_free_result(res);

	if(luModDate && (luClock < (luModDate + 86400)) )
		return(2);

	sprintf(gcQuery,"SELECT MAX(uClient) FROM tClient");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		return(3);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		if(field[0]!=NULL) sscanf(field[0],"%u",&uMaxuZone);
	mysql_free_result(res);

	sprintf(cInfo,"unxsISP%s %u %u %u",RELEASESHORT,guLoginClient,guPermLevel,uMaxuZone);

	if(!cInfo[0])
		return(4);

	h=gethostbyname("saturn.openisp.net");
	if(h==NULL)
		return(5);


	remoteServAddr.sin_family = h->h_addrtype;
	memcpy((char *) &remoteServAddr.sin_addr.s_addr,h->h_addr_list[0], h->h_length);
	remoteServAddr.sin_port=htons(53);

	sd=socket(AF_INET,SOCK_DGRAM,0);
	if(sd<0)
		return(6);
  
	cliAddr.sin_family = AF_INET;
	cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	cliAddr.sin_port = htons(0);
  
	rc=bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
	if(rc<0)
		return(7);


	rc=sendto(sd,cInfo,strlen(cInfo)+1,0,(struct sockaddr *)&remoteServAddr,
				sizeof(remoteServAddr));

	if(rc<0)
		return(8);

	if(luModDate)
		sprintf(gcQuery,"UPDATE tConfiguration SET uModBy=1,uModDate=%lu,cComment='%s' WHERE cLabel='UpdateInfo'",luClock,cInfo);
	else
		sprintf(gcQuery,"INSERT INTO tConfiguration SET cLabel='UpdateInfo',cValue='uModDate',cComment='%s',"
				"uCreatedBy=1,uCreatedDate=%lu,uModDate=%lu,uModBy=1",cInfo,luClock,luClock);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		tJob(gcQuery);
		return(9);
	}
  
	return(0);

}//int UpdateInfo(void)

#endif
