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


void tTemplateTypeNavList(void);

void ExtProcesstTemplateTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstTemplateTypeVars(pentry entries[], int x)


void ExttTemplateTypeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tTemplateTypeTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstTemplateTypeVars(entries,x);
                        	guMode=2000;
	                        tTemplateType(LANG_NB_CONFIRMNEW);
			}
			else
				tTemplateType("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstTemplateTypeVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uTemplateType=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtTemplateType(0);
			}
			else
				tTemplateType("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstTemplateTypeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tTemplateType(LANG_NB_CONFIRMDEL);
			}
			else
				tTemplateType("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstTemplateTypeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetTemplateType();
			}
			else
				tTemplateType("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstTemplateTypeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tTemplateType(LANG_NB_CONFIRMMOD);
			}
			else
				tTemplateType("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstTemplateTypeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtTemplateType();
			}
			else
				tTemplateType("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttTemplateTypeCommands(pentry entries[], int x)


void ExttTemplateTypeButtons(void)
{
	OpenFieldSet("tTemplateType Aux Panel",100);
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
			tTemplateTypeNavList();
	}
	CloseFieldSet();

}//void ExttTemplateTypeButtons(void)


void ExttTemplateTypeAuxTable(void)
{

}//void ExttTemplateTypeAuxTable(void)


void ExttTemplateTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uTemplateType"))
		{
			sscanf(gentries[i].val,"%u",&uTemplateType);
			guMode=6;
		}
	}
	tTemplateType("");

}//void ExttTemplateTypeGetHook(entry gentries[], int x)


void ExttTemplateTypeSelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTemplateType ORDER BY"
				" uTemplateType",
				VAR_LIST_tTemplateType);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tTemplateType,tClient WHERE tTemplateType.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uTemplateType",
					VAR_LIST_tTemplateType,uContactParentCompany,uContactParentCompany);
					

}//void ExttTemplateTypeSelect(void)


void ExttTemplateTypeSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tTemplateType WHERE uTemplateType=%u",
			VAR_LIST_tTemplateType,uTemplateType);
	else
                sprintf(gcQuery,"SELECT %s FROM tTemplateType,tClient"
                                " WHERE tTemplateType.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tTemplateType.uTemplateType=%u",
                        		VAR_LIST_tTemplateType
					,uContactParentCompany,uContactParentCompany
					,uTemplateType);

}//void ExttTemplateTypeSelectRow(void)


void ExttTemplateTypeListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTemplateType",
				VAR_LIST_tTemplateType);
	else
		sprintf(gcQuery,"SELECT %s FROM tTemplateType,tClient"
				" WHERE tTemplateType.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tTemplateType
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttTemplateTypeListFilter()
        if(!strcmp(gcFilter,"uTemplateType"))
        {
                sscanf(gcCommand,"%u",&uTemplateType);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tTemplateType.uTemplateType=%u"
						" ORDER BY uTemplateType",
						uTemplateType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uTemplateType");
        }

}//void ExttTemplateTypeListSelect(void)


void ExttTemplateTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uTemplateType"))
                printf("<option>uTemplateType</option>");
        else
                printf("<option selected>uTemplateType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttTemplateTypeListFilter(void)


void ExttTemplateTypeNavBar(void)
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

}//void ExttTemplateTypeNavBar(void)


void tTemplateTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uTemplateType,cLabel FROM tTemplateType ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tTemplateType.uTemplateType,"
				" tTemplateType.cLabel"
				" FROM tTemplateType,tClient"
				" WHERE tTemplateType.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tTemplateTypeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tTemplateTypeNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tTemplateType"
				"&uTemplateType=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tTemplateTypeNavList(void)


