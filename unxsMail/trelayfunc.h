/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
 
*/

//ModuleFunctionProtos()

static char cSearch[64]={""};

//Extern
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);

//Local
void tRelayNavList(void);
void BasicRelayCheck(void);
void tRelayContextInfo(void);

void ExtProcesstRelayVars(pentry entries[], int x)
{
	
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",entries[i].val);
	}
	
}//void ExtProcesstRelayVars(pentry entries[], int x)


void ExttRelayCommands(pentry entries[], int x)
{
	char cServerGroup[100]={""};
	char cJobData[100]={""};
	
	if(!strcmp(gcFunction,"tRelayTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstRelayVars(entries,x);
                        	guMode=2000;
	                        tRelay(LANG_NB_CONFIRMNEW);
			}
			else
				tRelay("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstRelayVars(entries,x);

                        	guMode=2000;
				//Check entries here
				BasicRelayCheck();
                        	guMode=0;

				uRelay=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtRelay(1);

				if(!uRelay)
					htmlPlainTextError("Unexpected NewtRelay() error");
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uRelay=%u;\ncDomain=%s;\ncTransport=%s;\n",
						uRelay,cDomain,cTransport);
				SubmitJob("NewRelay",cDomain,"",cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
			}
			else
				tRelay("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstRelayVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tRelay(LANG_NB_CONFIRMDEL);
			}
			else
				tRelay("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstRelayVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uRelay=%u;\ncDomain=%s;\ncTransport=%s;\n",
						uRelay,cDomain,cTransport);
				SubmitJob("DelRelay",cDomain,"",cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				DeletetRelay();
			}
			else
				tRelay("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstRelayVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tRelay(LANG_NB_CONFIRMMOD);
			}
			else
				tRelay("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstRelayVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				BasicRelayCheck();
                        	guMode=0;

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uRelay=%u;\ncDomain=%s;\ncTransport=%s;\n",
						uRelay,cDomain,cTransport);
				SubmitJob("ModRelay",cDomain,"",cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				uModBy=guLoginClient;
				ModtRelay();
			}
			else
				tRelay("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttRelayCommands(pentry entries[], int x)


void ExttRelayButtons(void)
{
	OpenFieldSet("tRelay Aux Panel",100);
	switch(guMode)
        {
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
			if(uRelay)
			{
				printf("<p><u>Record Context Info</u><br>");
				tRelayContextInfo();

				printf("<p><input title='Enter a mySQL search pattern for cDomain'"
					" type=text name=cSearch value='%s' maxlength=63 size=20> cSearch\n",cSearch);
				tRelayNavList();
			}
	}
	CloseFieldSet();

}//void ExttRelayButtons(void)


void ExttRelayAuxTable(void)
{

}//void ExttRelayAuxTable(void)


void ExttRelayGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uRelay"))
		{
			sscanf(gentries[i].val,"%u",&uRelay);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.63s",gentries[i].val);
		}
	}
	tRelay("");

}//void ExttRelayGetHook(entry gentries[], int x)


void ExttRelaySelect(void)
{
	if(cSearch[0])
		ExtSelectSearch("tRelay",VAR_LIST_tRelay,"cDomain",cSearch,NULL,101);
	else
		ExtSelect("tRelay",VAR_LIST_tRelay,0);
	
}//void ExttRelaySelect(void)


void ExttRelaySelectRow(void)
{
	ExtSelectRow("tRelay",VAR_LIST_tRelay,uRelay);

}//void ExttRelaySelectRow(void)


void ExttRelayListSelect(void)
{
	char cCat[512];

	ExtListSelect("tRelay",VAR_LIST_tRelay);

	//Changes here must be reflected below in ExttRelayListFilter()
        if(!strcmp(gcFilter,"uRelay"))
        {
                sscanf(gcCommand,"%u",&uRelay);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tRelay.uRelay=%u \
						ORDER BY uRelay",
						uRelay);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uRelay");
        }

}//void ExttRelayListSelect(void)


void ExttRelayListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uRelay"))
                printf("<option>uRelay</option>");
        else
                printf("<option selected>uRelay</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttRelayListFilter(void)


void ExttRelayNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

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

}//void ExttRelayNavBar(void)


void tRelayNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;

	printf("<p><u>tRelayNavList</u><br>\n");
	
	if(!cSearch[0])
	{
        	printf("Must restrict via cSearch\n");
		return;
	}


	ExtSelectSearch("tRelay","uRelay,cDomain","cDomain",cSearch,NULL,101);
	
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
	        while((field=mysql_fetch_row(res)))
		{
			uCount++;
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tRelay"
					"&uRelay=%s&cSearch=%s>%s</a><br>\n",field[0],cURLEncode(cSearch),field[1]);
			if(uCount>=100)
			{
				printf("More than 100 records: You must refine your search further<br>\n");
				break;
			}
	        }
	}
        mysql_free_result(res);

}//void tRelayNavList(void)


void BasicRelayCheck(void)
{
	MYSQL_RES *res;
	char *cp;

	if(!cDomain[0])
		tRelay("<blink>Error:</blink> cDomain can't be empty");
	
	if(strlen(cDomain)<4)
		tRelay("<blink>Error:</blink> Must provide valid cDomain");

	if((cp=strchr(cDomain,'.')))
	{
		cp++;
		if(strlen(cp)<2)
			tDomain("<blink>Error:</blink> cDomain after '.' part is too short");
	}
	else
	{
		tRelay("<blink>Error:</blink> cDomain must have '.' TLD part");
	}

	sprintf(gcQuery,"SELECT uDomain FROM tDomain WHERE cDomain='%s'",cDomain);
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if(!mysql_num_rows(res))
		tRelay("<blink>Error:</blink> cDomain not in tDomain");
	
	if(!cTransport[0])
		tRelay("<blink>Error:</blink> cTransport can't be empty");
	
	if(!uServerGroup)
		tRelay("<blink>Error:</blink> Must select uServerGroup");

	if(guMode==2000)
	{
		sprintf(gcQuery,"SELECT uRelay FROM tRelay WHERE cDomain='%s'",cDomain);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res))
			tRelay("<blink>Error:</blink> tRelay record existent for cDomain");
	}

	
}//void  BasicRelayCheck(void)


void tRelayContextInfo(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	printf("<u>Jobs</u><br>\n");
	sprintf(gcQuery,"SELECT tJob.uJob,tJob.cJobName,tJob.cServer FROM tJob WHERE tJob.cLabel LIKE '%%Relay%%%s%%' AND tJob.uJobStatus!=3",
						cDomain);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tJob&uJob=%s>%s/%s</a><br>\n",
						field[0],field[1],field[2]);
	}
	else
	{
		printf("No pending jobs<br>\n");
	}
        mysql_free_result(res);

}//void tRelayContextInfo(void)

	


