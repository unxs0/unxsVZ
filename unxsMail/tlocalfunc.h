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
void tLocalNavList(void);
void BasicLocalCheck(void);
void tLocalContextInfo(void);

void ExtProcesstLocalVars(pentry entries[], int x)
{
	
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",entries[i].val);
	}
	
}//void ExtProcesstLocalVars(pentry entries[], int x)


void ExttLocalCommands(pentry entries[], int x)
{
	char cServerGroup[100]={""};
	char cJobData[100]={""};
	
	if(!strcmp(gcFunction,"tLocalTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstLocalVars(entries,x);
                        	guMode=2000;
	                        tLocal(LANG_NB_CONFIRMNEW);
			}
			else
				tLocal("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstLocalVars(entries,x);

                        	guMode=2000;
				//Check entries here
				BasicLocalCheck();
                        	guMode=0;

				uLocal=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtLocal(1);

				if(!uLocal)
					htmlPlainTextError("Unexpected NewtLocal() error");
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uLocal=%u;\ncDomain=%s;\n",
						uLocal,cDomain);
				SubmitJob("NewLocal",cDomain,"",cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
			}
			else
				tLocal("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstLocalVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tLocal(LANG_NB_CONFIRMDEL);
			}
			else
				tLocal("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstLocalVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uLocal=%u;\ncDomain=%s;\n",
						uLocal,cDomain);
				SubmitJob("DelLocal",cDomain,"",cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				DeletetLocal();
			}
			else
				tLocal("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstLocalVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tLocal(LANG_NB_CONFIRMMOD);
			}
			else
				tLocal("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstLocalVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				BasicLocalCheck();
                        	guMode=0;

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uLocal=%u;\ncDomain=%s;\n",
						uLocal,cDomain);
				SubmitJob("ModLocal",cDomain,"",cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				uModBy=guLoginClient;
				ModtLocal();
			}
			else
				tLocal("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttLocalCommands(pentry entries[], int x)


void ExttLocalButtons(void)
{
	OpenFieldSet("tLocal Aux Panel",100);
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

			if(uLocal)
			{
				printf("<p><u>Record Context Info</u><br>");
				tLocalContextInfo();

				printf("<p><input title='Enter a mySQL search pattern for cDomain' type=text "
						"name=cSearch value='%s' maxlength=63 size=20> cSearch\n",cSearch);
				tLocalNavList();
			}
	}
	CloseFieldSet();

}//void ExttLocalButtons(void)


void ExttLocalAuxTable(void)
{

}//void ExttLocalAuxTable(void)


void ExttLocalGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uLocal"))
		{
			sscanf(gentries[i].val,"%u",&uLocal);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.63s",gentries[i].val);
		}
	}
	tLocal("");

}//void ExttLocalGetHook(entry gentries[], int x)


void ExttLocalSelect(void)
{
	if(cSearch[0])
		ExtSelectSearch("tLocal",VAR_LIST_tLocal,"cDomain",cSearch,NULL,101);
	else
		ExtSelect("tLocal",VAR_LIST_tLocal,0);
	
}//void ExttLocalSelect(void)


void ExttLocalSelectRow(void)
{
	ExtSelectRow("tLocal",VAR_LIST_tLocal,uLocal);

}//void ExttLocalSelectRow(void)


void ExttLocalListSelect(void)
{
	char cCat[512];

	ExtListSelect("tLocal",VAR_LIST_tLocal);

	//Changes here must be reflected below in ExttLocalListFilter()
        if(!strcmp(gcFilter,"uLocal"))
        {
                sscanf(gcCommand,"%u",&uLocal);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tLocal.uLocal=%u \
						ORDER BY uLocal",
						uLocal);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uLocal");
        }

}//void ExttLocalListSelect(void)


void ExttLocalListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uLocal"))
                printf("<option>uLocal</option>");
        else
                printf("<option selected>uLocal</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttLocalListFilter(void)


void ExttLocalNavBar(void)
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

}//void ExttLocalNavBar(void)


void tLocalNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;

	printf("<p><u>tLocalNavList</u><br>\n");
	
	if(!cSearch[0])
	{
        	printf("Must restrict via cSearch\n");
		return;
	}

	ExtSelectSearch("tLocal","uLocal,cDomain","cDomain",cSearch,NULL,101);

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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tLocal"
					"&uLocal=%s&cSearch=%s>%s</a><br>\n",field[0],cURLEncode(cSearch),field[1]);
			if(uCount>=100)
			{
				printf("More than 100 records: You must refine your search further<br>\n");
				break;
			}
	        }
	}
        mysql_free_result(res);

}//void tLocalNavList(void)


void BasicLocalCheck(void)
{
	MYSQL_RES *res;
	char *cp;

	if(!cDomain[0])
		tLocal("<blink>Error:</blink> cDomain can't be empty");
	
	if(strlen(cDomain)<4)
		tLocal("<blink>Error:</blink> Must provide valid cDomain");

	if((cp=strchr(cDomain,'.')))
	{
		cp++;
		if(strlen(cp)<2)
			tDomain("<blink>Error:</blink> cDomain after '.' part is too short");
	}
	else
	{
		tLocal("<blink>Error:</blink> cDomain must have '.' TLD part");
	}

	if(guMode==2000)
	{
		sprintf(gcQuery,"SELECT uLocal FROM tLocal WHERE cDomain='%s'",cDomain);
		mysql_query(&gMysql,gcQuery);
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res))
			tLocal("<blink>Error:</blink> tLocal record existent for cDomain");
	}

	sprintf(gcQuery,"SELECT uDomain FROM tDomain WHERE cDomain='%s'",cDomain);
	mysql_query(&gMysql,gcQuery);
	res=mysql_store_result(&gMysql);
	if(!mysql_num_rows(res))
		tLocal("<blink>Error:</blink> cDomain not in tDomain");
	
	if(!uServerGroup)
		tLocal("<blink>Error:</blink> Must select uServerGroup");

}//void BasicLocalCheck(void)


void tLocalContextInfo(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	printf("<u>Jobs</u><br>\n");
	sprintf(gcQuery,"SELECT tJob.uJob,tJob.cJobName,tJob.cServer FROM tJob WHERE tJob.cLabel LIKE '%%Local%%%s%%' AND tJob.uJobStatus!=3",
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

}//void tLocalContextInfo(void)


