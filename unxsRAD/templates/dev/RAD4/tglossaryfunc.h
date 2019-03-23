/*
FILE
	$Id: tglossaryfunc.h 1953 2012-05-22 15:03:17Z Colin $
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis for Unixservice.
 
*/

//ModuleFunctionProtos()


void tGlossaryNavList(void);

void ExtProcesstGlossaryVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstGlossaryVars(pentry entries[], int x)


void ExttGlossaryCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tGlossaryTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstGlossaryVars(entries,x);
                        	guMode=2000;
	                        tGlossary(LANG_NB_CONFIRMNEW);
			}
			else
				tGlossary("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstGlossaryVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uGlossary=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtGlossary(0);
			}
			else
				tGlossary("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstGlossaryVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tGlossary(LANG_NB_CONFIRMDEL);
			}
			else
				tGlossary("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstGlossaryVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetGlossary();
			}
			else
				tGlossary("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstGlossaryVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tGlossary(LANG_NB_CONFIRMMOD);
			}
			else
				tGlossary("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstGlossaryVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtGlossary();
			}
			else
				tGlossary("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttGlossaryCommands(pentry entries[], int x)


void ExttGlossaryButtons(void)
{
	OpenFieldSet("tGlossary Aux Panel",100);
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
			tGlossaryNavList();
	}
	CloseFieldSet();

}//void ExttGlossaryButtons(void)


void ExttGlossaryAuxTable(void)
{

}//void ExttGlossaryAuxTable(void)


void ExttGlossaryGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uGlossary"))
		{
			sscanf(gentries[i].val,"%u",&uGlossary);
			guMode=6;
		}
	}
	tGlossary("");

}//void ExttGlossaryGetHook(entry gentries[], int x)


void ExttGlossarySelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tGlossary ORDER BY"
				" uGlossary",
				VAR_LIST_tGlossary);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tGlossary,tClient WHERE tGlossary.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uGlossary",
					VAR_LIST_tGlossary,uContactParentCompany,uContactParentCompany);
					

}//void ExttGlossarySelect(void)


void ExttGlossarySelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tGlossary WHERE uGlossary=%u",
			VAR_LIST_tGlossary,uGlossary);
	else
                sprintf(gcQuery,"SELECT %s FROM tGlossary,tClient"
                                " WHERE tGlossary.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tGlossary.uGlossary=%u",
                        		VAR_LIST_tGlossary
					,uContactParentCompany,uContactParentCompany
					,uGlossary);

}//void ExttGlossarySelectRow(void)


void ExttGlossaryListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tGlossary",
				VAR_LIST_tGlossary);
	else
		sprintf(gcQuery,"SELECT %s FROM tGlossary,tClient"
				" WHERE tGlossary.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tGlossary
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttGlossaryListFilter()
        if(!strcmp(gcFilter,"uGlossary"))
        {
                sscanf(gcCommand,"%u",&uGlossary);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tGlossary.uGlossary=%u"
						" ORDER BY uGlossary",
						uGlossary);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uGlossary");
        }

}//void ExttGlossaryListSelect(void)


void ExttGlossaryListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uGlossary"))
                printf("<option>uGlossary</option>");
        else
                printf("<option selected>uGlossary</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttGlossaryListFilter(void)


void ExttGlossaryNavBar(void)
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

}//void ExttGlossaryNavBar(void)


void tGlossaryNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uGlossary,cLabel FROM tGlossary ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tGlossary.uGlossary,"
				" tGlossary.cLabel"
				" FROM tGlossary,tClient"
				" WHERE tGlossary.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tGlossaryNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tGlossaryNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=?gcFunction=tGlossary"
				"&uGlossary=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tGlossaryNavList(void)


