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



void tPrefixNavList(void);

void ExtProcesstPrefixVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstPrefixVars(pentry entries[], int x)


void ExttPrefixCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tPrefixTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstPrefixVars(entries,x);
                        	guMode=2000;
	                        tPrefix(LANG_NB_CONFIRMNEW);
			}
			else
				tPrefix("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstPrefixVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uPrefix=0;
#ifdef StandardFields
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
#endif
				NewtPrefix(0);
			}
			else
				tPrefix("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstPrefixVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
	                        guMode=2001;
				tPrefix(LANG_NB_CONFIRMDEL);
			}
			else
				tPrefix("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstPrefixVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=5;
				DeletetPrefix();
			}
			else
				tPrefix("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstPrefixVars(entries,x);
#ifdef StandardFields
			if(uAllowMod(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=2002;
				tPrefix(LANG_NB_CONFIRMMOD);
			}
			else
				tPrefix("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstPrefixVars(entries,x);
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
				ModtPrefix();
			}
			else
				tPrefix("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttPrefixCommands(pentry entries[], int x)


void ExttPrefixButtons(void)
{
	OpenFieldSet("tPrefix Aux Panel",100);
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
			tPrefixNavList();
	}
	CloseFieldSet();

}//void ExttPrefixButtons(void)


void ExttPrefixAuxTable(void)
{

}//void ExttPrefixAuxTable(void)


void ExttPrefixGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uPrefix"))
		{
			sscanf(gentries[i].val,"%u",&uPrefix);
			guMode=6;
		}
	}
	tPrefix("");

}//void ExttPrefixGetHook(entry gentries[], int x)


void ExttPrefixSelect(void)
{

#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tPrefix ORDER BY"
				" uPrefix",
				VAR_LIST_tPrefix);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tPrefix,tClient WHERE tPrefix.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uPrefix",
					VAR_LIST_tPrefix,uContactParentCompany,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tPrefix ORDER BY uPrefix",VAR_LIST_tPrefix);
#endif
					

}//void ExttPrefixSelect(void)


void ExttPrefixSelectRow(void)
{
#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tPrefix WHERE uPrefix=%u",
			VAR_LIST_tPrefix,uPrefix);
	else
                sprintf(gcQuery,"SELECT %s FROM tPrefix,tClient"
                                " WHERE tPrefix.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tPrefix.uPrefix=%u",
                        		VAR_LIST_tPrefix
					,uContactParentCompany,uContactParentCompany
					,uPrefix);
#else
	sprintf(gcQuery,"SELECT %s FROM tPrefix WHERE uPrefix=%u",VAR_LIST_tPrefix,uPrefix);
#endif

}//void ExttPrefixSelectRow(void)


void ExttPrefixListSelect(void)
{
	char cCat[512];
#ifdef StandardFields
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tPrefix",
				VAR_LIST_tPrefix);
	else
		sprintf(gcQuery,"SELECT %s FROM tPrefix,tClient"
				" WHERE tPrefix.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tPrefix
				,uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tPrefix",VAR_LIST_tPrefix);
#endif

	//Changes here must be reflected below in ExttPrefixListFilter()
        if(!strcmp(gcFilter,"uPrefix"))
        {
                sscanf(gcCommand,"%u",&uPrefix);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tPrefix.uPrefix=%u"
						" ORDER BY uPrefix",
						uPrefix);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uPrefix");
        }

}//void ExttPrefixListSelect(void)


void ExttPrefixListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uPrefix"))
                printf("<option>uPrefix</option>");
        else
                printf("<option selected>uPrefix</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttPrefixListFilter(void)


void ExttPrefixNavBar(void)
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

}//void ExttPrefixNavBar(void)


void tPrefixNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
#ifdef StandardFields
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uPrefix,cLabel FROM tPrefix ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tPrefix.uPrefix,"
				" tPrefix.cLabel"
				" FROM tPrefix,tClient"
				" WHERE tPrefix.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT uPrefix,cLabel FROM tPrefix ORDER BY cLabel");
#endif
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tPrefixNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tPrefixNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tPrefix"
				"&uPrefix=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tPrefixNavList(void)


