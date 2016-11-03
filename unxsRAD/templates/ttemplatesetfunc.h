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


void tTemplateSetNavList(void);

void ExtProcesstTemplateSetVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstTemplateSetVars(pentry entries[], int x)


void ExttTemplateSetCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tTemplateSetTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstTemplateSetVars(entries,x);
                        	guMode=2000;
	                        tTemplateSet(LANG_NB_CONFIRMNEW);
			}
			else
				tTemplateSet("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstTemplateSetVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uTemplateSet=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtTemplateSet(0);
			}
			else
				tTemplateSet("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstTemplateSetVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tTemplateSet(LANG_NB_CONFIRMDEL);
			}
			else
				tTemplateSet("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstTemplateSetVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetTemplateSet();
			}
			else
				tTemplateSet("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstTemplateSetVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tTemplateSet(LANG_NB_CONFIRMMOD);
			}
			else
				tTemplateSet("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstTemplateSetVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtTemplateSet();
			}
			else
				tTemplateSet("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttTemplateSetCommands(pentry entries[], int x)


void ExttTemplateSetButtons(void)
{
	OpenFieldSet("tTemplateSet Aux Panel",100);
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
			tTemplateSetNavList();
	}
	CloseFieldSet();

}//void ExttTemplateSetButtons(void)


void ExttTemplateSetAuxTable(void)
{

}//void ExttTemplateSetAuxTable(void)


void ExttTemplateSetGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uTemplateSet"))
		{
			sscanf(gentries[i].val,"%u",&uTemplateSet);
			guMode=6;
		}
	}
	tTemplateSet("");

}//void ExttTemplateSetGetHook(entry gentries[], int x)


void ExttTemplateSetSelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTemplateSet ORDER BY"
				" uTemplateSet",
				VAR_LIST_tTemplateSet);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tTemplateSet,tClient WHERE tTemplateSet.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uTemplateSet",
					VAR_LIST_tTemplateSet,uContactParentCompany,uContactParentCompany);
					

}//void ExttTemplateSetSelect(void)


void ExttTemplateSetSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tTemplateSet WHERE uTemplateSet=%u",
			VAR_LIST_tTemplateSet,uTemplateSet);
	else
                sprintf(gcQuery,"SELECT %s FROM tTemplateSet,tClient"
                                " WHERE tTemplateSet.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tTemplateSet.uTemplateSet=%u",
                        		VAR_LIST_tTemplateSet
					,uContactParentCompany,uContactParentCompany
					,uTemplateSet);

}//void ExttTemplateSetSelectRow(void)


void ExttTemplateSetListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTemplateSet",
				VAR_LIST_tTemplateSet);
	else
		sprintf(gcQuery,"SELECT %s FROM tTemplateSet,tClient"
				" WHERE tTemplateSet.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tTemplateSet
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttTemplateSetListFilter()
        if(!strcmp(gcFilter,"uTemplateSet"))
        {
                sscanf(gcCommand,"%u",&uTemplateSet);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tTemplateSet.uTemplateSet=%u"
						" ORDER BY uTemplateSet",
						uTemplateSet);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uTemplateSet");
        }

}//void ExttTemplateSetListSelect(void)


void ExttTemplateSetListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uTemplateSet"))
                printf("<option>uTemplateSet</option>");
        else
                printf("<option selected>uTemplateSet</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttTemplateSetListFilter(void)


void ExttTemplateSetNavBar(void)
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

}//void ExttTemplateSetNavBar(void)


void tTemplateSetNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uTemplateSet,cLabel FROM tTemplateSet ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tTemplateSet.uTemplateSet,"
				" tTemplateSet.cLabel"
				" FROM tTemplateSet,tClient"
				" WHERE tTemplateSet.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tTemplateSetNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tTemplateSetNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href={{cProject}}.cgi?gcFunction=tTemplateSet"
				"&uTemplateSet=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tTemplateSetNavList(void)


