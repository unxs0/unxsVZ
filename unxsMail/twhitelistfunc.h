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


void tWhiteListNavList(void);
void BasicWhiteListCheck(void);

void ExtProcesstWhiteListVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstWhiteListVars(pentry entries[], int x)


void ExttWhiteListCommands(pentry entries[], int x)
{
	char cLogin[100]={""};
	char cServerGroup[100]={""};
	
	if(!strcmp(gcFunction,"tWhiteListTools"))
	{
		//ModuleFunctionProcess()
		
		//Coding time-saver
		ProcesstWhiteListVars(entries,x);

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
		else
			sprintf(cServerGroup,ForeignKey("tServerGroup","cLabel",uServerGroup));
			
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstWhiteListVars(entries,x);
                        	guMode=2000;
	                        tWhiteList(LANG_NB_CONFIRMNEW);
			}
			else
				tWhiteList("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned guCompany=0;
                        	ProcesstWhiteListVars(entries,x);
				GetClientOwner(guLoginClient,&guCompany);
				
                        	guMode=2000;
				BasicWhiteListCheck();
                        	guMode=0;

				uWhiteList=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified

				//Guess which kind of job we must submit based on data
				if(uUser)
					SubmitJob("NewUserPrefs","",cLogin,cServerGroup,
						"",0,uUser,guCompany,guLoginClient);
				else 
					SubmitJob("NewWhiteList","","global",cServerGroup,
						"",0,0,guCompany,guLoginClient);
				NewtWhiteList(0);
			}
			else
				tWhiteList("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstWhiteListVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tWhiteList(LANG_NB_CONFIRMDEL);
			}
			else
				tWhiteList("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstWhiteListVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				if(uUser)
					SubmitJob("DelUserPrefs","",cLogin,cServerGroup,
						"",0,uUser,guCompany,guLoginClient);
				else
					SubmitJob("DelWhiteList","","global",cServerGroup,
						"",0,0,guCompany,guLoginClient);
				DeletetWhiteList();
			}
			else
				tWhiteList("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstWhiteListVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tWhiteList(LANG_NB_CONFIRMMOD);
			}
			else
				tWhiteList("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstWhiteListVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				BasicWhiteListCheck();
                        	guMode=0;

				uModBy=guLoginClient;
				if(uUser)
					SubmitJob("ModUserPrefs","",cLogin,cServerGroup,
						"",0,uUser,guCompany,guLoginClient);
				else
					SubmitJob("ModWhiteList","","global",cServerGroup,
						"",0,0,guCompany,guLoginClient);
				//htmlPlainTextError(cServerGroup);
				ModtWhiteList();
				
			}
			else
				tWhiteList("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttWhiteListCommands(pentry entries[], int x)


void ExttWhiteListButtons(void)
{
	OpenFieldSet("tWhiteList Aux Panel",100);
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
			tWhiteListNavList();
	}
	CloseFieldSet();

}//void ExttWhiteListButtons(void)


void ExttWhiteListAuxTable(void)
{

}//void ExttWhiteListAuxTable(void)


void ExttWhiteListGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uWhiteList"))
		{
			sscanf(gentries[i].val,"%u",&uWhiteList);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"uUser"))
		{
			sscanf(gentries[i].val,"%u",&uUser);
			guMode=2000;
		}
	}
	tWhiteList("");

}//void ExttWhiteListGetHook(entry gentries[], int x)


void ExttWhiteListSelect(void)
{
	ExtSelect("tWhiteList",VAR_LIST_tWhiteList,0);

}//void ExttWhiteListSelect(void)


void ExttWhiteListSelectRow(void)
{
	ExtSelectRow("tWhiteList",VAR_LIST_tWhiteList,uWhiteList);

}//void ExttWhiteListSelectRow(void)


void ExttWhiteListListSelect(void)
{
	char cCat[512];
	
	ExtListSelect("tWhiteList",VAR_LIST_tWhiteList);
	
	//Changes here must be reflected below in ExttWhiteListListFilter()
        if(!strcmp(gcFilter,"uWhiteList"))
        {
                sscanf(gcCommand,"%u",&uWhiteList);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tWhiteList.uWhiteList=%u"
						" ORDER BY uWhiteList",
						uWhiteList);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uWhiteList");
        }

}//void ExttWhiteListListSelect(void)


void ExttWhiteListListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uWhiteList"))
                printf("<option>uWhiteList</option>");
        else
                printf("<option selected>uWhiteList</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttWhiteListListFilter(void)


void ExttWhiteListNavBar(void)
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

}//void ExttWhiteListNavBar(void)


void tWhiteListNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

//void ExtSelectSearch(const char *cTable,const char *cVarList,const char *cSearchField, const char *cSearch,const char *cExtraCond,unsigned uMaxResults);


	if(!uUser)
	{
		printf("<p><u>tWhiteListNavList</u><br>\n");
		
		ExtSelect("tWhiteList","tWhiteList.uWhiteList,tWhiteList.cLabel",0);
	}
	else
	{
		char cExtra[32]={""};

		printf("<p><u>tWhiteListNavList (for tUser.cLogin=%s)</u><br>\n",ForeignKey("tUser","cLogin",uUser));
		
		sprintf(cExtra,"uUser=%u",uUser);
		ExtSelectSearch("tWhiteList","tWhiteList.uWhiteList,tWhiteList.cLabel","tWhiteList.cLabel","%",cExtra,0);
	}
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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tWhiteList"
				"&uWhiteList=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

	if(uUser)
		printf("<p><a class=darkLink href=unxsMail.cgi?gcFunction=tUser&uUser=%u "
			"title='Go back to the tUser record you were working on'>"
			"Back to tUser record</a></p>\n",uUser);

}//void tWhiteListNavList(void)


void BasicWhiteListCheck(void)
{
	MYSQL_RES *res;

	if(uUser)
		sprintf(gcQuery,"SELECT uWhiteList FROM tWhiteList WHERE cLabel='%s' AND uUser=%u",cLabel,uUser);
	else
		sprintf(gcQuery,"SELECT uWhiteList FROM tWhiteList WHERE cLabel='%s' AND uUser=0 AND uServerGroup!=0",cLabel);
	
	macro_mySQLRunAndStore(res);
	if(mysql_num_rows(res))
	{
		if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
			guMode=2000;
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
			guMode=2002;

		tWhiteList("<blink>Error:</blink> The record already exists in tWhiteList");
	}
	
	mysql_free_result(res);
	
	if(uUser)
		sprintf(gcQuery,"SELECT uWhiteList FROM tWhiteList  WHERE cLabel='%s' AND uUser=%u",cLabel,uUser);
	else
		sprintf(gcQuery,"SELECT uWhiteList FROM tWhiteList WHERE cLabel='%s' AND uUser=0 AND uServerGroup!=0",cLabel);
	
	macro_mySQLRunAndStore(res);
	if(mysql_num_rows(res))
	{
		if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
			guMode=2000;
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
			guMode=2002;

		tWhiteList("<blink>Error:</blink> The record already exists in tWhiteList ");
	}

	if(!uUser && !uServerGroup)
	{
		if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
			guMode=2000;
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
			guMode=2002;

		tBlackList("<blink>Error:</blink> You must either enter uUser or select uServerGroup");
	}

	if(uUser && !uServerGroup)
		sscanf(ForeignKey("tUser","uServerGroup",uUser),"%u",&uServerGroup);

}//void BasicWhiteListCheck(void)

