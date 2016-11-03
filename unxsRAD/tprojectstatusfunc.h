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


void tProjectStatusNavList(void);

void ExtProcesstProjectStatusVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstProjectStatusVars(pentry entries[], int x)


void ExttProjectStatusCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tProjectStatusTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstProjectStatusVars(entries,x);
                        	guMode=2000;
	                        tProjectStatus(LANG_NB_CONFIRMNEW);
			}
			else
				tProjectStatus("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstProjectStatusVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uStatus=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtProjectStatus(0);
			}
			else
				tProjectStatus("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstProjectStatusVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tProjectStatus(LANG_NB_CONFIRMDEL);
			}
			else
				tProjectStatus("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstProjectStatusVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetProjectStatus();
			}
			else
				tProjectStatus("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstProjectStatusVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tProjectStatus(LANG_NB_CONFIRMMOD);
			}
			else
				tProjectStatus("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstProjectStatusVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtProjectStatus();
			}
			else
				tProjectStatus("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttProjectStatusCommands(pentry entries[], int x)


void ExttProjectStatusButtons(void)
{
	OpenFieldSet("tProjectStatus Aux Panel",100);
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
			tProjectStatusNavList();
	}
	CloseFieldSet();

}//void ExttProjectStatusButtons(void)


void ExttProjectStatusAuxTable(void)
{

}//void ExttProjectStatusAuxTable(void)


void ExttProjectStatusGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uStatus"))
		{
			sscanf(gentries[i].val,"%u",&uStatus);
			guMode=6;
		}
	}
	tProjectStatus("");

}//void ExttProjectStatusGetHook(entry gentries[], int x)


void ExttProjectStatusSelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tProjectStatus ORDER BY"
				" uStatus",
				VAR_LIST_tProjectStatus);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tProjectStatus,tClient WHERE tProjectStatus.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uStatus",
					VAR_LIST_tProjectStatus,uContactParentCompany,uContactParentCompany);
					

}//void ExttProjectStatusSelect(void)


void ExttProjectStatusSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tProjectStatus WHERE uStatus=%u",
			VAR_LIST_tProjectStatus,uStatus);
	else
                sprintf(gcQuery,"SELECT %s FROM tProjectStatus,tClient"
                                " WHERE tProjectStatus.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tProjectStatus.uStatus=%u",
                        		VAR_LIST_tProjectStatus
					,uContactParentCompany,uContactParentCompany
					,uStatus);

}//void ExttProjectStatusSelectRow(void)


void ExttProjectStatusListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tProjectStatus",
				VAR_LIST_tProjectStatus);
	else
		sprintf(gcQuery,"SELECT %s FROM tProjectStatus,tClient"
				" WHERE tProjectStatus.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tProjectStatus
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttProjectStatusListFilter()
        if(!strcmp(gcFilter,"uStatus"))
        {
                sscanf(gcCommand,"%u",&uStatus);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tProjectStatus.uStatus=%u"
						" ORDER BY uStatus",
						uStatus);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uStatus");
        }

}//void ExttProjectStatusListSelect(void)


void ExttProjectStatusListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uStatus"))
                printf("<option>uStatus</option>");
        else
                printf("<option selected>uStatus</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttProjectStatusListFilter(void)


void ExttProjectStatusNavBar(void)
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

}//void ExttProjectStatusNavBar(void)


void tProjectStatusNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uStatus,cLabel FROM tProjectStatus ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tProjectStatus.uStatus,"
				" tProjectStatus.cLabel"
				" FROM tProjectStatus,tClient"
				" WHERE tProjectStatus.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tProjectStatusNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tProjectStatusNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsRAD.cgi?gcFunction=tProjectStatus"
				"&uStatus=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tProjectStatusNavList(void)


