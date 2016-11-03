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


void tBlackListNavList(void);
void BasicBlackListCheck(void);

void ExtProcesstBlackListVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstBlackListVars(pentry entries[], int x)


void ExttBlackListCommands(pentry entries[], int x)
{
	char cLogin[100]={""};
	char cServerGroup[100]={""};
	
	if(!strcmp(gcFunction,"tBlackListTools"))
	{
		//ModuleFunctionProcess()
		
		//Coding time-saver
		ProcesstBlackListVars(entries,x);

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
	                        ProcesstBlackListVars(entries,x);
                        	guMode=2000;
	                        tBlackList(LANG_NB_CONFIRMNEW);
			}
			else
				tBlackList("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned guCompany=0;
                        	ProcesstBlackListVars(entries,x);
				GetClientOwner(guLoginClient,&guCompany);
				
                        	guMode=2000;
				BasicBlackListCheck();
                        	guMode=0;

				uBlackList=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified

				//Guess which kind of job we must submit based on data
				if(uUser )
					SubmitJob("NewUserPrefs","",cLogin,cServerGroup,
						"",0,uUser,guCompany,guLoginClient);
				else 
					SubmitJob("NewBlackList","","global",cServerGroup,
						"",0,0,guCompany,guLoginClient);
				NewtBlackList(0);
			}
			else
				tBlackList("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstBlackListVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tBlackList(LANG_NB_CONFIRMDEL);
			}
			else
				tBlackList("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstBlackListVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				if(uUser )
					SubmitJob("DelUserPrefs","",cLogin,cServerGroup,
						"",0,uUser,guCompany,guLoginClient);
				else
					SubmitJob("DelBlackList","","global",cServerGroup,
						"",0,0,guCompany,guLoginClient);
				DeletetBlackList();
			}
			else
				tBlackList("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstBlackListVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tBlackList(LANG_NB_CONFIRMMOD);
			}
			else
				tBlackList("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstBlackListVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				BasicBlackListCheck();
                        	guMode=0;

				uModBy=guLoginClient;
				if(uUser )
					SubmitJob("ModUserPrefs","",cLogin,cServerGroup,
						"",0,uUser,guCompany,guLoginClient);
				else
					SubmitJob("ModBlackList","","global",cServerGroup,
						"",0,0,guCompany,guLoginClient);
				//htmlPlainTextError(cServerGroup);
				ModtBlackList();
				
			}
			else
				tBlackList("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttBlackListCommands(pentry entries[], int x)


void ExttBlackListButtons(void)
{
	OpenFieldSet("tBlackList Aux Panel",100);
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
			tBlackListNavList();
	}
	CloseFieldSet();

}//void ExttBlackListButtons(void)


void ExttBlackListAuxTable(void)
{

}//void ExttBlackListAuxTable(void)


void ExttBlackListGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uBlackList"))
		{
			sscanf(gentries[i].val,"%u",&uBlackList);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"uUser"))
		{
			sscanf(gentries[i].val,"%u",&uUser);
			guMode=2000;
		}
	}
	tBlackList("");

}//void ExttBlackListGetHook(entry gentries[], int x)


void ExttBlackListSelect(void)
{
	ExtSelect("tBlackList",VAR_LIST_tBlackList,0);

}//void ExttBlackListSelect(void)


void ExttBlackListSelectRow(void)
{
	ExtSelectRow("tBlackList",VAR_LIST_tBlackList,uBlackList);

}//void ExttBlackListSelectRow(void)


void ExttBlackListListSelect(void)
{
	char cCat[512];
	
	ExtListSelect("tBlackList",VAR_LIST_tBlackList);
	
	//Changes here must be reflected below in ExttBlackListListFilter()
        if(!strcmp(gcFilter,"uBlackList"))
        {
                sscanf(gcCommand,"%u",&uBlackList);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tBlackList.uBlackList=%u"
						" ORDER BY uBlackList",
						uBlackList);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uBlackList");
        }

}//void ExttBlackListListSelect(void)


void ExttBlackListListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uBlackList"))
                printf("<option>uBlackList</option>");
        else
                printf("<option selected>uBlackList</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttBlackListListFilter(void)


void ExttBlackListNavBar(void)
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

}//void ExttBlackListNavBar(void)


void tBlackListNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

//void ExtSelectSearch(const char *cTable,const char *cVarList,const char *cSearchField, const char *cSearch,const char *cExtraCond,unsigned uMaxResults);


	if(!uUser)
	{
		printf("<p><u>tBlackListNavList</u><br>\n");
		
		ExtSelect("tBlackList","tBlackList.uBlackList,tBlackList.cLabel",0);
	}
	else
	{
		char cExtra[32]={""};

		printf("<p><u>tBlackListNavList (for tUser.cLogin=%s)</u><br>\n",ForeignKey("tUser","cLogin",uUser));
		
		sprintf(cExtra,"uUser=%u",uUser);
		ExtSelectSearch("tBlackList","tBlackList.uBlackList,tBlackList.cLabel","tBlackList.cLabel","%",cExtra,0);
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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tBlackList"
				"&uBlackList=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

	if(uUser)
		printf("<p><a class=darkLink href=unxsMail.cgi?gcFunction=tUser&uUser=%u "
			"title='Go back to the tUser record you were working on'>"
			"Back to tUser record</a></p>\n",uUser);

}//void tBlackListNavList(void)


void BasicBlackListCheck(void)
{
	MYSQL_RES *res;

	if(uUser)
		sprintf(gcQuery,"SELECT uBlackList FROM tBlackList WHERE cLabel='%s' AND uUser=%u",cLabel,uUser);
	else
		sprintf(gcQuery,"SELECT uBlackList FROM tBlackList WHERE cLabel='%s' AND uUser=0 AND uServerGroup!=0",cLabel);
	
	macro_mySQLRunAndStore(res);
	if(mysql_num_rows(res))
	{
		if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
			guMode=2000;
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
			guMode=2002;

		tBlackList("<blink>Error:</blink> The record already exists in tBlackList");
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

		tBlackList("<blink>Error:</blink> The record already exists in tWhiteList ");
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

}//void BasicBlackListCheck(void)

