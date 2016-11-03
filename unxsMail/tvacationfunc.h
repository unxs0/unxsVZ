/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis for Unixservice.
 
*/

//ModuleFunctionProtos()

void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
                unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);

void tVacationNavList(void);

void ExtProcesstVacationVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstVacationVars(pentry entries[], int x)


void ExttVacationCommands(pentry entries[], int x)
{
	char cLogin[100]={""};
	char cServerGroup[100]={""};

	if(!strcmp(gcFunction,"tVacationTools"))
	{
		//ModuleFunctionProcess()

		//Coding time-saver
		ProcesstVacationVars(entries,x);

		if(uUser)
		{
			MYSQL_RES *res;
			MYSQL_ROW field;

			sprintf(gcQuery,"SELECT tUser.cLogin,tServerGroup.cLabel FROM tUser,tServerGroup "
					"WHERE tServerGroup.uServerGroup=tUser.uServerGroup AND tUser.uUser=%u",uUser);
			macro_mySQLRunAndStore(res);
			if((field=mysql_fetch_row(res)))
			{
				sprintf(cLogin,"%.99s",field[0]);
				sprintf(cServerGroup,"%.99s",field[1]);
			}
			mysql_free_result(res);
		}
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstVacationVars(entries,x);
                        	guMode=2000;
	                        tVacation(LANG_NB_CONFIRMNEW);
			}
			else
				tVacation("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstVacationVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uVacation=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				SubmitJob("NewUserVacation","",cLogin,cServerGroup,
					"",0,uUser,guCompany,guLoginClient);
				htmlPlainTextError(cServerGroup);
				NewtVacation(0);
			}
			else
				tVacation("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstVacationVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tVacation(LANG_NB_CONFIRMDEL);
			}
			else
				tVacation("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstVacationVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				SubmitJob("DelUserVacation","",cLogin,cServerGroup,
					"",0,uUser,guCompany,guLoginClient);
				DeletetVacation();
			}
			else
				tVacation("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstVacationVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tVacation(LANG_NB_CONFIRMMOD);
			}
			else
				tVacation("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstVacationVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				SubmitJob("ModUserVacation","",cLogin,cServerGroup,
					"",0,uUser,guCompany,guLoginClient);
				ModtVacation();
			}
			else
				tVacation("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttVacationCommands(pentry entries[], int x)


void ExttVacationButtons(void)
{
	OpenFieldSet("tVacation Aux Panel",100);
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
			printf("<p><u>Record Context Info</u><br>");
			tVacationNavList();
	}
	CloseFieldSet();

}//void ExttVacationButtons(void)


void ExttVacationAuxTable(void)
{

}//void ExttVacationAuxTable(void)


void ExttVacationGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uVacation"))
		{
			sscanf(gentries[i].val,"%u",&uVacation);
			guMode=6;
		}
	}
	tVacation("");

}//void ExttVacationGetHook(entry gentries[], int x)


void ExttVacationSelect(void)
{
	ExtSelect("tVacation",VAR_LIST_tVacation,0);

}//void ExttVacationSelect(void)


void ExttVacationSelectRow(void)
{
	ExtSelectRow("tVacation",VAR_LIST_tVacation,uVacation);
	
}//void ExttVacationSelectRow(void)


void ExttVacationListSelect(void)
{
	char cCat[512];
	
	ExtListSelect("tVacation",VAR_LIST_tVacation);
	//Changes here must be reflected below in ExttVacationListFilter()
        if(!strcmp(gcFilter,"uVacation"))
        {
                sscanf(gcCommand,"%u",&uVacation);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tVacation.uVacation=%u"
						" ORDER BY uVacation",
						uVacation);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uVacation");
        }

}//void ExttVacationListSelect(void)


void ExttVacationListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uVacation"))
                printf("<option>uVacation</option>");
        else
                printf("<option selected>uVacation</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttVacationListFilter(void)


void ExttVacationNavBar(void)
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

}//void ExttVacationNavBar(void)


void tVacationNavList(void)
{
/*        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uVacation,cLabel FROM tVacation ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tVacation.uVacation,"
				" tVacation.cLabel"
				" FROM tVacation,tClient"
				" WHERE tVacation.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tVacationNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tVacationNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tVacation"
				"&uVacation=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);
*/
}//void tVacationNavList(void)


