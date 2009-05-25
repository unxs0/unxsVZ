/*
FILE
	$Id: tnasfunc.h,v 1.5 2004/03/11 03:10:43 ggw-ssl2 Exp $
	(tAuthorize.cPasswd template set member)
PURPOSE
	Provide web interface functionality as separate from schema as possible
	for RADIUS NAS/RAS server info. Usually administrates multiple server raddb/client.conf files
	or similar.

AUTHOR/LEGAL
	Template and mysqlRAD2 software: (C) 2001-2009 Gary Wallis.
	GPL License applies, see www.fsf.org for details
	LICENSE included in distribution.

 
*/

static unsigned uServer=0;
static char cuServerPullDown[256]={""};
void NASServerList(void);
unsigned GetuServer(char *cLabel);
#ifndef FREE_RADIUS
int MakeClientsFile(unsigned uHtml, unsigned uServer);
int MakeNASListFile(unsigned uHtml, unsigned uServer);
#else
int CreateClientConf(unsigned uHtml, unsigned uServer);
#endif 
void tNASNavList(void);

int SubmitNASJob(char *cJobName,char *cJobData,unsigned uNASGroup,unsigned uJobDate);

void tNASBasicCheck(void);
static void htmlRecordContext(void);

//Extern
int SubmitSingleJob(const char *cJobName,char *cJobData,char const *cServer,unsigned uJobDate);

void ExtProcesstNASVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cuServerPullDown"))
		{
			strcpy(cuServerPullDown,entries[i].val);
			uServer=ReadPullDownOwner("tServer","cLabel",cuServerPullDown,guCompany);
		}
	}

}//void ExtProcesstNASVars(pentry entries[], int x)


void ExttNASCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tNASTools"))
	{
		//ModuleFunctionProcess()

		//Default wizard like two step creation and deletion
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstNASVars(entries,x);
				//Check global conditions for new record here
              		        guMode=2000;
	                        tNAS(LANG_NB_CONFIRMNEW);
			}
			else
				tNAS("<blink>Error</blink>: Denied by permissions settings");  
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstNASVars(entries,x);
				tNASBasicCheck();
				if(!uServer)
				{
					guMode=2000;
					tNAS("Must select valid initial server");
				}
				uNAS=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;
				NewtNAS(1);
				//default uServer is the one selected via the dropdown at the left panel
				sprintf(gcQuery,"INSERT INTO tNASGroup SET uNAS=%u,uServer=%u"
								,uNAS,uServer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));

				SubmitNASJob("NewNAS",cLabel,uNAS,0);
				tNAS("New user added");
			}
			else
				tNAS("<blink>Error</blink>: Denied by permissions settings");  
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstNASVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))  
			{
                        	guMode=2001;
	                        tNAS(LANG_NB_CONFIRMDEL);
			}
			else
				tNAS("<blink>Error</blink>: Denied by permissions settings");  
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstNASVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))  
			{
                        	guMode=5;
				SubmitNASJob("DelNAS",cLabel,uNAS,0);
                        	DeletetNAS();
			}
			else
				tNAS("<blink>Error</blink>: Denied by permissions settings");  
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstNASVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
                        	guMode=2002;
	                        tNAS(LANG_NB_CONFIRMMOD);
			}
			else
				tNAS("<blink>Error</blink>: Denied by permissions settings");  
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstNASVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
				tNASBasicCheck();
				uModBy=guLoginClient;
				SubmitNASJob("ModNAS",cLabel,uNAS,0);
                	        ModtNAS();
			}
			else
				tNAS("<blink>Error</blink>: Denied by permissions settings");  
                }

		else if(!strcmp(gcCommand,"Add Server"))
		{
                        ProcesstNASVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
				time_t clock;
				MYSQL_RES *res;

				time(&clock);
				if(!uNAS || !uServer)
					tNAS("Must select valid NAS and server");
				sprintf(gcQuery,"SELECT uServer FROM tNASGroup WHERE uNAS=%u AND uServer=%u",uNAS,uServer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res)) tNAS("");
				mysql_free_result(res);

				sprintf(gcQuery,"INSERT INTO tNASGroup SET uNAS=%u,uServer=%u",uNAS,uServer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));

				sprintf(gcQuery,"UPDATE tNAS SET uModBy=%u,uModDate=%lu WHERE uNAS=%u",guLoginClient,clock,uNAS);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));

				SubmitSingleJob("AddNASServer",cLabel,
						ForeignKey("tServer","cLabel",uServer),0);
				tNAS("Server added");
			}
			else
				tNAS("<blink>Error</blink>: Denied by permissions settings");  

		}
		else if(!strcmp(gcCommand,"Remove Server"))
		{
                        ProcesstNASVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
				if(!uNAS || !uServer)
					tNAS("Must select valid NAS and server");
				sprintf(gcQuery,"DELETE FROM tNASGroup WHERE uNAS=%u AND uServer=%u",uNAS,uServer);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				SubmitSingleJob("DelNASServer",cLabel,ForeignKey("tServer","cLabel",uServer),0);
				tNAS("Server removed");
			}
			else
				tNAS("<blink>Error</blink>: Denied by permissions settings");  
		}

                else if(!strcmp(gcCommand,"Make Local NASList File"))
                {
			ProcesstNASVars(entries,x);
			if(guPermLevel>=12)
			{
#ifndef FREE_RADIUS
				MakeNASListFile(1,1);
				tNAS("raddb/naslist file created");
#else
				CreateClientConf(1,1);
				tNAS("raddb/clients.conf file created");
#endif
			}
			else
				tNAS("<blink>Error</blink>: Denied by permissions settings");  
		}
#ifndef FREE_RADIUS
                else if(!strcmp(gcCommand,"Make Local Clients File"))
                {
			ProcesstNASVars(entries,x);
			if(guPermLevel>=12)
			{
				MakeClientsFile(1,1);
				tNAS("raddb/clients file created");
			}
			else
				tNAS("<blink>Error</blink>: Denied by permissions settings");  
		}
#endif
	}

}//void ExttNASCommands(pentry entries[], int x)


void ExttNASButtons(void)
{
	OpenFieldSet("tNas Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<u>Enter required data</u><p>");
			printf("Please select one of your RADIUS servers<br>\n");
			tTablePullDownOwner("tServer;cuServerPullDown","cLabel","cLabel",uServer,1);
			printf("<p>\n");
                        printf(LANG_NBB_CONFIRMNEW);
			printf("<br>\n");
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
			printf("This NAS maybe in use double check!<br>\n");
                break;

                case 2002:
			printf("Review record data<br>");
                        printf(LANG_NBB_CONFIRMMOD);
			printf("<br>\n");
                break;

		default:
        		printf("<p><u>Table Tips</u><br>\n");
			printf("This table stores the RADIUS client settings. A RADIUS client is an authorized"
				" network/remote access server (NAS/RAS) identified/authenticated usually by it's IP"
				" (cIP) and authorized by a shared secret (cKey). Note that shared secrets are"
				" inherently unsecure. cType may have relevance for your RADIUS server setup."
				" cLogin and cPasswd are only used for special applications. cInfo is for your notes");

			htmlRecordContext();

        		printf("<p><u>tNASNavList</u><br>\n");
			tNASNavList();

			if(uNAS)
			{
				printf("<p><u>NAS Server Assignment</u><br>\n");
				NASServerList();

				if(guPermLevel>9)
				{
					tTablePullDownOwner("tServer;cuServerPullDown","cLabel","cLabel",uServer,1);
					printf("<br><input title='Add above server for service for this NAS' "
					"class=largeButton type=submit name=gcCommand value='Add Server'>\n");
					printf("<p><input title='Remove above server for service for this NAS' "
					"class=lwarnButton type=submit name=gcCommand value='Remove Server'>\n");
				}

				if(guPermLevel>11)
				{
			        	printf("<p><u>Local Server Management</u><br>\n");
#ifdef FREE_RADIUS
					printf("<input title='Make raddb/client.conf file on this server' class=largeButton "
					" type=submit name=gcCommand value='Make Local Clients File'><br>\n");
#endif
					printf("<input title='Make raddb/naslist file on this server' class=largeButton "
					"type=submit name=gcCommand value='Make Local NASList File'>\n");
				}

			}

		break;

	}


	CloseFieldSet();

}//void ExttNASButtons(void)


void ExttNASAuxTable(void)
{

}//void ExttNASAuxTable(void)


void ExttNASGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uNAS"))
		{
			sscanf(gentries[i].val,"%u",&uNAS);
			guMode=6;
		}
	}
	tNAS("");

}//void ExttNASGetHook(entry gentries[], int x)


void ExttNASSelect(void)
{
	ExtSelect("tNAS",VAR_LIST_tNAS,0);

}//void ExttNASSelect(void)


void ExttNASSelectRow(void)
{
	ExtSelectRow("tNAS",VAR_LIST_tNAS,uNAS);

}//void ExttNASSelectRow(void)


void ExttNASListSelect(void)
{
	char cCat[512];

	ExtListSelect("tNAS",VAR_LIST_tNAS);

	//Changes here must be reflected below in ExttNASListFilter()
        if(!strcmp(gcFilter,"uNAS"))
        {
                sscanf(gcCommand,"%u",&uNAS);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tNAS.uNAS=%u \
						ORDER BY uNAS",
						uNAS);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uNAS");
        }

}//void ExttNASListSelect(void)


void ExttNASListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uNAS"))
                printf("<option>uNAS</option>");
        else
                printf("<option selected>uNAS</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttNASListFilter(void)


void ExttNASNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10 && !guListMode)
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

}//void ExttNASNavBar(void)


void tNASNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelectSearch("tNAS","tNAS.uNAS,tNAS.cLabel,tNAS.cIP","tNAS.cLabel","",NULL,0);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
                printf("<a class=darkLink href=unxsRadius.cgi?gcFunction=tNAS"
			"&uNAS=%s>%s/%s</a><br>\n",field[0],field[1],field[2]);
        }
        mysql_free_result(res);

}//void tNASNavList(void)


void NASServerList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

        sprintf(gcQuery,"SELECT tServer.uServer,tServer.cLabel FROM tServer,tNASGroup WHERE tServer.uServer=tNASGroup.uServer AND tNASGroup.uNAS=%u",uNAS);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
                printf("<a class=darkLink href=unxsRadius.cgi?gcFunction=tServer"
			"&uServer=%s>%s</a><br>\n",field[0],field[1]);
        }
        mysql_free_result(res);

}//void NASServerList(void)


int SubmitNASJob(char *cJobName,char *cJobData,unsigned uNASGroup,unsigned uJobDate)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tServer.cLabel FROM tServer,tNASGroup WHERE tServer.uServer=tNASGroup.uServer AND tNASGroup.uNAS=%u",uNASGroup);


	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
		SubmitSingleJob(cJobName,cJobData,field[0],uJobDate);
	mysql_free_result(res);

	return(0);

}//int SubmitNASJob()


void tNASBasicCheck(void)
{
	//Basic sanity checks before deploying a record ;)
/*	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uNAS FROM tNAS WHERE cIP='%s'",cIP);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
		{
			guMode=2000;
			tNAS("cIP already in use. Can't create new record");
		}
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
		{
			unsigned udbNAS=0;
			field=mysql_fetch_row(res);
			sscanf(field[0],"%u",&udbNAS);
			if(udbNAS!=uNAS)
			{
				guMode=2002;
				tNAS("cIP already in use. Can't modify record");
			}
		}
	}
*/
}//void tNASBasicCheck(void)


void htmlRecordContext(void)
{
	printf("<p><u>Record Context Info</u><br>");
	printf("No context information currently available.");

}//void htmlRecordContext(void)


