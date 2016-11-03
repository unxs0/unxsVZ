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



void tGroupTypeNavList(void);

void ExtProcesstGroupTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstGroupTypeVars(pentry entries[], int x)


void ExttGroupTypeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tGroupTypeTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstGroupTypeVars(entries,x);
                        	guMode=2000;
	                        tGroupType(LANG_NB_CONFIRMNEW);
			}
			else
				tGroupType("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstGroupTypeVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uGroupType=0;
#ifdef StandardFields
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
#endif
				NewtGroupType(0);
			}
			else
				tGroupType("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstGroupTypeVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
	                        guMode=2001;
				tGroupType(LANG_NB_CONFIRMDEL);
			}
			else
				tGroupType("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstGroupTypeVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=5;
				DeletetGroupType();
			}
			else
				tGroupType("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstGroupTypeVars(entries,x);
#ifdef StandardFields
			if(uAllowMod(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=2002;
				tGroupType(LANG_NB_CONFIRMMOD);
			}
			else
				tGroupType("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstGroupTypeVars(entries,x);
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
				ModtGroupType();
			}
			else
				tGroupType("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttGroupTypeCommands(pentry entries[], int x)


void ExttGroupTypeButtons(void)
{
	OpenFieldSet("tGroupType Aux Panel",100);
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
			tGroupTypeNavList();
	}
	CloseFieldSet();

}//void ExttGroupTypeButtons(void)


void ExttGroupTypeAuxTable(void)
{

}//void ExttGroupTypeAuxTable(void)


void ExttGroupTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uGroupType"))
		{
			sscanf(gentries[i].val,"%u",&uGroupType);
			guMode=6;
		}
	}
	tGroupType("");

}//void ExttGroupTypeGetHook(entry gentries[], int x)


void ExttGroupTypeSelect(void)
{

#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tGroupType ORDER BY"
				" uGroupType",
				VAR_LIST_tGroupType);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tGroupType,tClient WHERE tGroupType.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uGroupType",
					VAR_LIST_tGroupType,uContactParentCompany,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tGroupType ORDER BY uGroupType",VAR_LIST_tGroupType);
#endif
					

}//void ExttGroupTypeSelect(void)


void ExttGroupTypeSelectRow(void)
{
#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tGroupType WHERE uGroupType=%u",
			VAR_LIST_tGroupType,uGroupType);
	else
                sprintf(gcQuery,"SELECT %s FROM tGroupType,tClient"
                                " WHERE tGroupType.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tGroupType.uGroupType=%u",
                        		VAR_LIST_tGroupType
					,uContactParentCompany,uContactParentCompany
					,uGroupType);
#else
	sprintf(gcQuery,"SELECT %s FROM tGroupType WHERE uGroupType=%u",VAR_LIST_tGroupType,uGroupType);
#endif

}//void ExttGroupTypeSelectRow(void)


void ExttGroupTypeListSelect(void)
{
	char cCat[512];
#ifdef StandardFields
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tGroupType",
				VAR_LIST_tGroupType);
	else
		sprintf(gcQuery,"SELECT %s FROM tGroupType,tClient"
				" WHERE tGroupType.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tGroupType
				,uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tGroupType",VAR_LIST_tGroupType);
#endif

	//Changes here must be reflected below in ExttGroupTypeListFilter()
        if(!strcmp(gcFilter,"uGroupType"))
        {
                sscanf(gcCommand,"%u",&uGroupType);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tGroupType.uGroupType=%u"
						" ORDER BY uGroupType",
						uGroupType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uGroupType");
        }

}//void ExttGroupTypeListSelect(void)


void ExttGroupTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uGroupType"))
                printf("<option>uGroupType</option>");
        else
                printf("<option selected>uGroupType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttGroupTypeListFilter(void)


void ExttGroupTypeNavBar(void)
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

}//void ExttGroupTypeNavBar(void)


void tGroupTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
#ifdef StandardFields
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uGroupType,cLabel FROM tGroupType ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tGroupType.uGroupType,"
				" tGroupType.cLabel"
				" FROM tGroupType,tClient"
				" WHERE tGroupType.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT uGroupType,cLabel FROM tGroupType ORDER BY cLabel");
#endif
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tGroupTypeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tGroupTypeNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tGroupType"
				"&uGroupType=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tGroupTypeNavList(void)


