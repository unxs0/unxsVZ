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



void tGatewayTypeNavList(void);

void ExtProcesstGatewayTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstGatewayTypeVars(pentry entries[], int x)


void ExttGatewayTypeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tGatewayTypeTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstGatewayTypeVars(entries,x);
                        	guMode=2000;
	                        tGatewayType(LANG_NB_CONFIRMNEW);
			}
			else
				tGatewayType("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstGatewayTypeVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uGatewayType=0;
#ifdef StandardFields
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
#endif
				NewtGatewayType(0);
			}
			else
				tGatewayType("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstGatewayTypeVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
	                        guMode=2001;
				tGatewayType(LANG_NB_CONFIRMDEL);
			}
			else
				tGatewayType("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstGatewayTypeVars(entries,x);
#ifdef StandardFields
			if(uAllowDel(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=5;
				DeletetGatewayType();
			}
			else
				tGatewayType("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstGatewayTypeVars(entries,x);
#ifdef StandardFields
			if(uAllowMod(uOwner,uCreatedBy))
#else
			if(guPermLevel>=9)
#endif
			{
				guMode=2002;
				tGatewayType(LANG_NB_CONFIRMMOD);
			}
			else
				tGatewayType("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstGatewayTypeVars(entries,x);
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
				ModtGatewayType();
			}
			else
				tGatewayType("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttGatewayTypeCommands(pentry entries[], int x)


void ExttGatewayTypeButtons(void)
{
	OpenFieldSet("tGatewayType Aux Panel",100);
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
			tGatewayTypeNavList();
	}
	CloseFieldSet();

}//void ExttGatewayTypeButtons(void)


void ExttGatewayTypeAuxTable(void)
{

}//void ExttGatewayTypeAuxTable(void)


void ExttGatewayTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uGatewayType"))
		{
			sscanf(gentries[i].val,"%u",&uGatewayType);
			guMode=6;
		}
	}
	tGatewayType("");

}//void ExttGatewayTypeGetHook(entry gentries[], int x)


void ExttGatewayTypeSelect(void)
{

#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tGatewayType ORDER BY"
				" uGatewayType",
				VAR_LIST_tGatewayType);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tGatewayType,tClient WHERE tGatewayType.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uGatewayType",
					VAR_LIST_tGatewayType,uContactParentCompany,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tGatewayType ORDER BY uGatewayType",VAR_LIST_tGatewayType);
#endif
					

}//void ExttGatewayTypeSelect(void)


void ExttGatewayTypeSelectRow(void)
{
#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tGatewayType WHERE uGatewayType=%u",
			VAR_LIST_tGatewayType,uGatewayType);
	else
                sprintf(gcQuery,"SELECT %s FROM tGatewayType,tClient"
                                " WHERE tGatewayType.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tGatewayType.uGatewayType=%u",
                        		VAR_LIST_tGatewayType
					,uContactParentCompany,uContactParentCompany
					,uGatewayType);
#else
	sprintf(gcQuery,"SELECT %s FROM tGatewayType WHERE uGatewayType=%u",VAR_LIST_tGatewayType,uGatewayType);
#endif

}//void ExttGatewayTypeSelectRow(void)


void ExttGatewayTypeListSelect(void)
{
	char cCat[512];
#ifdef StandardFields
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tGatewayType",
				VAR_LIST_tGatewayType);
	else
		sprintf(gcQuery,"SELECT %s FROM tGatewayType,tClient"
				" WHERE tGatewayType.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tGatewayType
				,uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tGatewayType",VAR_LIST_tGatewayType);
#endif

	//Changes here must be reflected below in ExttGatewayTypeListFilter()
        if(!strcmp(gcFilter,"uGatewayType"))
        {
                sscanf(gcCommand,"%u",&uGatewayType);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tGatewayType.uGatewayType=%u"
						" ORDER BY uGatewayType",
						uGatewayType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uGatewayType");
        }

}//void ExttGatewayTypeListSelect(void)


void ExttGatewayTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uGatewayType"))
                printf("<option>uGatewayType</option>");
        else
                printf("<option selected>uGatewayType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttGatewayTypeListFilter(void)


void ExttGatewayTypeNavBar(void)
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

}//void ExttGatewayTypeNavBar(void)


void tGatewayTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
#ifdef StandardFields
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uGatewayType,cLabel FROM tGatewayType ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tGatewayType.uGatewayType,"
				" tGatewayType.cLabel"
				" FROM tGatewayType,tClient"
				" WHERE tGatewayType.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT uGatewayType,cLabel FROM tGatewayType ORDER BY cLabel");
#endif
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tGatewayTypeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tGatewayTypeNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tGatewayType"
				"&uGatewayType=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tGatewayTypeNavList(void)


