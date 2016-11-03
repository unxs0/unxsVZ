/*
FILE
	svn ID removed
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2012 Gary Wallis for Unixservice, LLC.
TEMPLATE VARS AND FUNCTIONS
	ModuleFunctionProcess
	ModuleFunctionProtos
	cProject
	cTableKey
	cTableName
*/



void tDatacenterNavList(void);

void ExtProcesstDatacenterVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstDatacenterVars(pentry entries[], int x)


void ExttDatacenterCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tDatacenterTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstDatacenterVars(entries,x);
                        	guMode=2000;
	                        tDatacenter(LANG_NB_CONFIRMNEW);
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstDatacenterVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uDatacenter=0;
#ifdef StandardFields
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
#endif
				NewtDatacenter(0);
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstDatacenterVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
	                        guMode=2001;
				tDatacenter(LANG_NB_CONFIRMDEL);
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstDatacenterVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=5;
				DeletetDatacenter();
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstDatacenterVars(entries,x);
#ifdef StandardFields
			if(uAllowMod(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=2002;
				tDatacenter(LANG_NB_CONFIRMMOD);
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstDatacenterVars(entries,x);
#ifdef StandardFields
			if(uAllowMod(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

#ifdef StandardFields
				uModBy=guLoginClient;
#endif
				ModtDatacenter();
			}
			else
				tDatacenter("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttDatacenterCommands(pentry entries[], int x)


void ExttDatacenterButtons(void)
{
	OpenFieldSet("tDatacenter Aux Panel",100);
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
			//printf("<br><input type=submit class=largeButton title='Sample button help'"
			//		" name=gcCommand value='Sample Button'>");
			tDatacenterNavList();
	}
	CloseFieldSet();

}//void ExttDatacenterButtons(void)


void ExttDatacenterAuxTable(void)
{

}//void ExttDatacenterAuxTable(void)


void ExttDatacenterGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uDatacenter"))
		{
			sscanf(gentries[i].val,"%u",&uDatacenter);
			guMode=6;
		}
	}
	tDatacenter("");

}//void ExttDatacenterGetHook(entry gentries[], int x)


void ExttDatacenterSelect(void)
{

#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tDatacenter ORDER BY"
				" uDatacenter",
				VAR_LIST_tDatacenter);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tDatacenter,tClient WHERE tDatacenter.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uDatacenter",
					VAR_LIST_tDatacenter,uContactParentCompany,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tDatacenter ORDER BY uDatacenter",VAR_LIST_tDatacenter);
#endif
					

}//void ExttDatacenterSelect(void)


void ExttDatacenterSelectRow(void)
{
#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tDatacenter WHERE uDatacenter=%u",
			VAR_LIST_tDatacenter,uDatacenter);
	else
                sprintf(gcQuery,"SELECT %s FROM tDatacenter,tClient"
                                " WHERE tDatacenter.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tDatacenter.uDatacenter=%u",
                        		VAR_LIST_tDatacenter
					,uContactParentCompany,uContactParentCompany
					,uDatacenter);
#else
	sprintf(gcQuery,"SELECT %s FROM tDatacenter WHERE uDatacenter=%u",VAR_LIST_tDatacenter,uDatacenter);
#endif

}//void ExttDatacenterSelectRow(void)


void ExttDatacenterListSelect(void)
{
	char cCat[512];
#ifdef StandardFields
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tDatacenter",
				VAR_LIST_tDatacenter);
	else
		sprintf(gcQuery,"SELECT %s FROM tDatacenter,tClient"
				" WHERE tDatacenter.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tDatacenter
				,uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tDatacenter",VAR_LIST_tDatacenter);
#endif

	//Changes here must be reflected below in ExttDatacenterListFilter()
        if(!strcmp(gcFilter,"uDatacenter"))
        {
                sscanf(gcCommand,"%u",&uDatacenter);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tDatacenter.uDatacenter=%u"
						" ORDER BY uDatacenter",
						uDatacenter);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uDatacenter");
        }

}//void ExttDatacenterListSelect(void)


void ExttDatacenterListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uDatacenter"))
                printf("<option>uDatacenter</option>");
        else
                printf("<option selected>uDatacenter</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttDatacenterListFilter(void)


void ExttDatacenterNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

#ifdef StandardFields
	if(uAllowMod(uOwner,uCreatedBy))
#else
	if(guPermLevel>=9)
#endif
		printf(LANG_NBB_MODIFY);

#ifdef StandardFields
	if(uAllowDel(uOwner,uCreatedBy)) 
#else
	if(guPermLevel>=9)
#endif
		printf(LANG_NBB_DELETE);

#ifdef StandardFields
	if(uOwner)
#else
	if(guPermLevel>=9)
#endif
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttDatacenterNavBar(void)


void tDatacenterNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
#ifdef StandardFields
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uDatacenter,cLabel FROM tDatacenter ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tDatacenter.uDatacenter,"
				" tDatacenter.cLabel"
				" FROM tDatacenter,tClient"
				" WHERE tDatacenter.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT uDatacenter,cLabel FROM tDatacenter ORDER BY cLabel");
#endif
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tDatacenterNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tDatacenterNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tDatacenter"
				"&uDatacenter=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tDatacenterNavList(void)


