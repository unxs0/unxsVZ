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


void tMySQLUserNavList(void);

void ExtProcesstMySQLUserVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstMySQLUserVars(pentry entries[], int x)


void ExttMySQLUserCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tMySQLUserTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstMySQLUserVars(entries,x);
                        	guMode=2000;
	                        tMySQLUser(LANG_NB_CONFIRMNEW);
			}
			else
				tMySQLUser("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstMySQLUserVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uMySQLUser=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtMySQLUser(0);
			}
			else
				tMySQLUser("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstMySQLUserVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tMySQLUser(LANG_NB_CONFIRMDEL);
			}
			else
				tMySQLUser("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstMySQLUserVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetMySQLUser();
			}
			else
				tMySQLUser("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstMySQLUserVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tMySQLUser(LANG_NB_CONFIRMMOD);
			}
			else
				tMySQLUser("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstMySQLUserVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtMySQLUser();
			}
			else
				tMySQLUser("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttMySQLUserCommands(pentry entries[], int x)


void ExttMySQLUserButtons(void)
{
	OpenFieldSet("tMySQLUser Aux Panel",100);
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
			tMySQLUserNavList();
	}
	CloseFieldSet();

}//void ExttMySQLUserButtons(void)


void ExttMySQLUserAuxTable(void)
{

}//void ExttMySQLUserAuxTable(void)


void ExttMySQLUserGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uMySQLUser"))
		{
			sscanf(gentries[i].val,"%u",&uMySQLUser);
			guMode=6;
		}
	}
	tMySQLUser("");

}//void ExttMySQLUserGetHook(entry gentries[], int x)


void ExttMySQLUserSelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tMySQLUser ORDER BY"
				" uMySQLUser",
				VAR_LIST_tMySQLUser);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tMySQLUser,tClient WHERE tMySQLUser.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uMySQLUser",
					VAR_LIST_tMySQLUser,uContactParentCompany,uContactParentCompany);
					

}//void ExttMySQLUserSelect(void)


void ExttMySQLUserSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tMySQLUser WHERE uMySQLUser=%u",
			VAR_LIST_tMySQLUser,uMySQLUser);
	else
                sprintf(gcQuery,"SELECT %s FROM tMySQLUser,tClient"
                                " WHERE tMySQLUser.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tMySQLUser.uMySQLUser=%u",
                        		VAR_LIST_tMySQLUser
					,uContactParentCompany,uContactParentCompany
					,uMySQLUser);

}//void ExttMySQLUserSelectRow(void)


void ExttMySQLUserListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tMySQLUser",
				VAR_LIST_tMySQLUser);
	else
		sprintf(gcQuery,"SELECT %s FROM tMySQLUser,tClient"
				" WHERE tMySQLUser.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tMySQLUser
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttMySQLUserListFilter()
        if(!strcmp(gcFilter,"uMySQLUser"))
        {
                sscanf(gcCommand,"%u",&uMySQLUser);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tMySQLUser.uMySQLUser=%u"
						" ORDER BY uMySQLUser",
						uMySQLUser);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uMySQLUser");
        }

}//void ExttMySQLUserListSelect(void)


void ExttMySQLUserListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uMySQLUser"))
                printf("<option>uMySQLUser</option>");
        else
                printf("<option selected>uMySQLUser</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttMySQLUserListFilter(void)


void ExttMySQLUserNavBar(void)
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

}//void ExttMySQLUserNavBar(void)


void tMySQLUserNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uMySQLUser,cLabel FROM tMySQLUser ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tMySQLUser.uMySQLUser,"
				" tMySQLUser.cLabel"
				" FROM tMySQLUser,tClient"
				" WHERE tMySQLUser.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tMySQLUserNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tMySQLUserNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsApache.cgi?gcFunction=tMySQLUser"
				"&uMySQLUser=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tMySQLUserNavList(void)


