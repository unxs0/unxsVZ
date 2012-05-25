/*
FILE
	$Id$
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis for Unixservice.
 
*/

//ModuleFunctionProtos()


void tProjectNavList(void);

void ExtProcesstProjectVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstProjectVars(pentry entries[], int x)


void ExttProjectCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tProjectTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstProjectVars(entries,x);
                        	guMode=2000;
	                        tProject(LANG_NB_CONFIRMNEW);
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstProjectVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uProject=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtProject(0);
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tProject(LANG_NB_CONFIRMDEL);
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetProject();
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tProject(LANG_NB_CONFIRMMOD);
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtProject();
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,"Select"))
                {
                        ProcesstProjectVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(uProject)
				{
					guCookieProject=uProject;
					guCookieTable=0;
					guCookieField=0;
					SetSessionCookie();
					tProject("This project selected for workflow");
				}
			}
			else
				tProject("<blink>Error</blink>: Denied by permissions settings");
		}
	}

}//void ExttProjectCommands(pentry entries[], int x)


void ExttProjectButtons(void)
{
	OpenFieldSet("tProject Aux Panel",100);
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
			printf("<p><u>Operations</u><br>");
			printf("<input type=submit class=largeButton"
				" title='Select and keep this project marked for current work flow. Releases any saved table and field'"
				" name=gcCommand value='Select'>");
			tProjectNavList();
	}
	CloseFieldSet();

}//void ExttProjectButtons(void)


void ExttProjectAuxTable(void)
{

}//void ExttProjectAuxTable(void)


void ExttProjectGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uProject"))
		{
			sscanf(gentries[i].val,"%u",&uProject);
			guMode=6;
		}
		else if(guCookieProject)
		{
			uProject=guCookieProject;
			guMode=7;
		}
	}
	tProject("");

}//void ExttProjectGetHook(entry gentries[], int x)


void ExttProjectSelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tProject ORDER BY"
				" uProject",
				VAR_LIST_tProject);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tProject,tClient WHERE tProject.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uProject",
					VAR_LIST_tProject,uContactParentCompany,uContactParentCompany);
					

}//void ExttProjectSelect(void)


void ExttProjectSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tProject WHERE uProject=%u",
			VAR_LIST_tProject,uProject);
	else
                sprintf(gcQuery,"SELECT %s FROM tProject,tClient"
                                " WHERE tProject.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tProject.uProject=%u",
                        		VAR_LIST_tProject
					,uContactParentCompany,uContactParentCompany
					,uProject);

}//void ExttProjectSelectRow(void)


void ExttProjectListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tProject",
				VAR_LIST_tProject);
	else
		sprintf(gcQuery,"SELECT %s FROM tProject,tClient"
				" WHERE tProject.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tProject
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttProjectListFilter()
        if(!strcmp(gcFilter,"uProject"))
        {
                sscanf(gcCommand,"%u",&uProject);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tProject.uProject=%u"
						" ORDER BY uProject",
						uProject);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uProject");
        }

}//void ExttProjectListSelect(void)


void ExttProjectListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uProject"))
                printf("<option>uProject</option>");
        else
                printf("<option selected>uProject</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttProjectListFilter(void)


void ExttProjectNavBar(void)
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

}//void ExttProjectNavBar(void)


void tProjectNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uProject,cLabel FROM tProject ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tProject.uProject,"
				" tProject.cLabel"
				" FROM tProject,tClient"
				" WHERE tProject.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tProjectNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		char *cColor;
        	printf("<p><u>tProjectNavList</u><br>\n");
	        while((field=mysql_fetch_row(res)))
		{
			if(atoi(field[0])==uProject)
				cColor="red";
			else
				cColor="black";
			printf("<a class=darkLink href=unxsRAD.cgi?gcFunction=tProject"
				"&uProject=%s><font color=%s>%s</font></a><br>\n",
				field[0],cColor,field[1]);
		}
	}
        mysql_free_result(res);

}//void tProjectNavList(void)


