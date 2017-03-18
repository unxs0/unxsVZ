/*
FILE
	ttemplatefunc.c
PURPOSE
	Non schema based ops
AUTHOR
	(C) 2001-2017 Gary Wallis for Unixservice, LLC.
 
*/

//ModuleFunctionProtos()

void tTemplateNavList(void);

void ExtProcesstTemplateVars(pentry entries[], int x)
{
	//register int i;
	//for(i=0;i<x;i++)
	//{
	//	if(!strcmp(entries[i].name,"uTable"))
	//		sscanf(entries[i].val,"%u",&guTable);
	//}
}//void ExtProcesstTemplateVars(pentry entries[], int x)


void ExttTemplateCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tTemplateTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstTemplateVars(entries,x);
                        	guMode=2000;
	                        tTemplate(LANG_NB_CONFIRMNEW);
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstTemplateVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uTemplate=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtTemplate(0);
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstTemplateVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tTemplate(LANG_NB_CONFIRMDEL);
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstTemplateVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetTemplate();
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstTemplateVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tTemplate(LANG_NB_CONFIRMMOD);
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstTemplateVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtTemplate();
			}
			else
				tTemplate("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttTemplateCommands(pentry entries[], int x)


void ExttTemplateButtons(void)
{
	OpenFieldSet("tTemplate Aux Panel",100);
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
			printf("Templates are used to generate source code directory structures. See tTemplateType for more info.");
			printf("<p><u>Record Context Info</u><br>");
			if(guCookieProject)
			{
				unsigned uProjectTemplateSet=0;
				sscanf(ForeignKey("tProject","uTemplateSet",guCookieProject),"%u",&uProjectTemplateSet);
				printf("Showing only tTemplateSet.cLabel=%s templates.",
					ForeignKey("tTemplateSet","cLabel",uProjectTemplateSet));
			}
			tTemplateNavList();
	}
	CloseFieldSet();

}//void ExttTemplateButtons(void)


void ExttTemplateAuxTable(void)
{

}//void ExttTemplateAuxTable(void)


void ExttTemplateGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uTemplate"))
		{
			sscanf(gentries[i].val,"%u",&uTemplate);
			guMode=6;
		}
	}
	tTemplate("");

}//void ExttTemplateGetHook(entry gentries[], int x)


void ExttTemplateSelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTemplate ORDER BY"
				" uTemplate",
				VAR_LIST_tTemplate);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tTemplate,tClient WHERE tTemplate.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uTemplate",
					VAR_LIST_tTemplate,uContactParentCompany,uContactParentCompany);
					

}//void ExttTemplateSelect(void)


void ExttTemplateSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tTemplate WHERE uTemplate=%u",
			VAR_LIST_tTemplate,uTemplate);
	else
                sprintf(gcQuery,"SELECT %s FROM tTemplate,tClient"
                                " WHERE tTemplate.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tTemplate.uTemplate=%u",
                        		VAR_LIST_tTemplate
					,uContactParentCompany,uContactParentCompany
					,uTemplate);

}//void ExttTemplateSelectRow(void)


void ExttTemplateListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTemplate",
				VAR_LIST_tTemplate);
	else
		sprintf(gcQuery,"SELECT %s FROM tTemplate,tClient"
				" WHERE tTemplate.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tTemplate
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttTemplateListFilter()
        if(!strcmp(gcFilter,"uTemplate"))
        {
                sscanf(gcCommand,"%u",&uTemplate);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tTemplate.uTemplate=%u ORDER BY uTemplate", uTemplate);
		strcat(gcQuery,cCat);
        }
        if(!strcmp(gcFilter,"cLabel"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tTemplate.cLabel LIKE '%s%%' ORDER BY uTemplate",gcCommand);
		strcat(gcQuery,cCat);
	}
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uTemplate");
        }

}//void ExttTemplateListSelect(void)


void ExttTemplateListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uTemplate"))
                printf("<option>uTemplate</option>");
        else
                printf("<option selected>uTemplate</option>");
        if(strcmp(gcFilter,"cLabel"))
                printf("<option>cLabel</option>");
        else
                printf("<option selected>cLabel</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttTemplateListFilter(void)


void ExttTemplateNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

	if(uOwner && uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);

	if(uOwner && uAllowDel(uOwner,uCreatedBy)) 
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttTemplateNavBar(void)


void tTemplateNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies

	unsigned uProjectTemplateSet=0;
	if(guCookieProject)
		sscanf(ForeignKey("tProject","uTemplateSet",guCookieProject),"%u",&uProjectTemplateSet);

	if(uProjectTemplateSet)
	{
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
			sprintf(gcQuery,"SELECT uTemplate,cLabel FROM tTemplate WHERE uTemplateSet=%u ORDER BY cLabel",uProjectTemplateSet);
		else
			sprintf(gcQuery,"SELECT tTemplate.uTemplate,"
					" tTemplate.cLabel"
					" FROM tTemplate,tClient"
					" WHERE tTemplate.uOwner=tClient.uClient AND tTemplate.uTemplateSet=%u"
					" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
						uProjectTemplateSet, uContactParentCompany ,uContactParentCompany);
	}
	else
	{
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
			sprintf(gcQuery,"SELECT uTemplate,cLabel FROM tTemplate ORDER BY cLabel");
		else
			sprintf(gcQuery,"SELECT tTemplate.uTemplate,"
					" tTemplate.cLabel"
					" FROM tTemplate,tClient"
					" WHERE tTemplate.uOwner=tClient.uClient"
					" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
						uContactParentCompany
						,uContactParentCompany);
	}
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tTemplateNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		if(uProjectTemplateSet)
        		printf("<p><u>tTemplateNavList Workflow</u><br>\n");
		else
        		printf("<p><u>tTemplateNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsRAD.cgi?gcFunction=tTemplate"
				"&uTemplate=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tTemplateNavList(void)

