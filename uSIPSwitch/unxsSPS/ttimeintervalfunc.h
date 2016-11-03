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



void tTimeIntervalNavList(void);

void ExtProcesstTimeIntervalVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstTimeIntervalVars(pentry entries[], int x)


void ExttTimeIntervalCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tTimeIntervalTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstTimeIntervalVars(entries,x);
                        	guMode=2000;
	                        tTimeInterval(LANG_NB_CONFIRMNEW);
			}
			else
				tTimeInterval("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstTimeIntervalVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uTimeInterval=0;
#ifdef StandardFields
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
#endif
				NewtTimeInterval(0);
			}
			else
				tTimeInterval("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstTimeIntervalVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
	                        guMode=2001;
				tTimeInterval(LANG_NB_CONFIRMDEL);
			}
			else
				tTimeInterval("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstTimeIntervalVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=5;
				DeletetTimeInterval();
			}
			else
				tTimeInterval("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstTimeIntervalVars(entries,x);
#ifdef StandardFields
			if(uAllowMod(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=2002;
				tTimeInterval(LANG_NB_CONFIRMMOD);
			}
			else
				tTimeInterval("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstTimeIntervalVars(entries,x);
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
				ModtTimeInterval();
			}
			else
				tTimeInterval("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttTimeIntervalCommands(pentry entries[], int x)


void ExttTimeIntervalButtons(void)
{
	OpenFieldSet("tTimeInterval Aux Panel",100);
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
			tTimeIntervalNavList();
	}
	CloseFieldSet();

}//void ExttTimeIntervalButtons(void)


void ExttTimeIntervalAuxTable(void)
{

}//void ExttTimeIntervalAuxTable(void)


void ExttTimeIntervalGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uTimeInterval"))
		{
			sscanf(gentries[i].val,"%u",&uTimeInterval);
			guMode=6;
		}
	}
	tTimeInterval("");

}//void ExttTimeIntervalGetHook(entry gentries[], int x)


void ExttTimeIntervalSelect(void)
{

#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTimeInterval ORDER BY"
				" uTimeInterval",
				VAR_LIST_tTimeInterval);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tTimeInterval,tClient WHERE tTimeInterval.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uTimeInterval",
					VAR_LIST_tTimeInterval,uContactParentCompany,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tTimeInterval ORDER BY uTimeInterval",VAR_LIST_tTimeInterval);
#endif
					

}//void ExttTimeIntervalSelect(void)


void ExttTimeIntervalSelectRow(void)
{
#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tTimeInterval WHERE uTimeInterval=%u",
			VAR_LIST_tTimeInterval,uTimeInterval);
	else
                sprintf(gcQuery,"SELECT %s FROM tTimeInterval,tClient"
                                " WHERE tTimeInterval.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tTimeInterval.uTimeInterval=%u",
                        		VAR_LIST_tTimeInterval
					,uContactParentCompany,uContactParentCompany
					,uTimeInterval);
#else
	sprintf(gcQuery,"SELECT %s FROM tTimeInterval WHERE uTimeInterval=%u",VAR_LIST_tTimeInterval,uTimeInterval);
#endif

}//void ExttTimeIntervalSelectRow(void)


void ExttTimeIntervalListSelect(void)
{
	char cCat[512];
#ifdef StandardFields
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTimeInterval",
				VAR_LIST_tTimeInterval);
	else
		sprintf(gcQuery,"SELECT %s FROM tTimeInterval,tClient"
				" WHERE tTimeInterval.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tTimeInterval
				,uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tTimeInterval",VAR_LIST_tTimeInterval);
#endif

	//Changes here must be reflected below in ExttTimeIntervalListFilter()
        if(!strcmp(gcFilter,"uTimeInterval"))
        {
                sscanf(gcCommand,"%u",&uTimeInterval);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tTimeInterval.uTimeInterval=%u"
						" ORDER BY uTimeInterval",
						uTimeInterval);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uTimeInterval");
        }

}//void ExttTimeIntervalListSelect(void)


void ExttTimeIntervalListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uTimeInterval"))
                printf("<option>uTimeInterval</option>");
        else
                printf("<option selected>uTimeInterval</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttTimeIntervalListFilter(void)


void ExttTimeIntervalNavBar(void)
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

}//void ExttTimeIntervalNavBar(void)


void tTimeIntervalNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
#ifdef StandardFields
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uTimeInterval,cLabel FROM tTimeInterval ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tTimeInterval.uTimeInterval,"
				" tTimeInterval.cLabel"
				" FROM tTimeInterval,tClient"
				" WHERE tTimeInterval.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT uTimeInterval,cLabel FROM tTimeInterval ORDER BY cLabel");
#endif
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tTimeIntervalNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tTimeIntervalNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tTimeInterval"
				"&uTimeInterval=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tTimeIntervalNavList(void)


