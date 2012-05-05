/*
FILE
	$Id: modulefunc.h.template 2459 2009-02-11 12:04:10Z Gary $
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis for Unixservice.
 
*/

//ModuleFunctionProtos()


void tTableNavList(void);

void ExtProcesstTableVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstTableVars(pentry entries[], int x)


void ExttTableCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tTableTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstTableVars(entries,x);
                        	guMode=2000;
	                        tTable(LANG_NB_CONFIRMNEW);
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstTableVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uTable=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtTable(0);
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tTable(LANG_NB_CONFIRMDEL);
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetTable();
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tTable(LANG_NB_CONFIRMMOD);
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstTableVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtTable();
			}
			else
				tTable("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttTableCommands(pentry entries[], int x)


void ExttTableButtons(void)
{
	OpenFieldSet("tTable Aux Panel",100);
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
			tTableNavList();
	}
	CloseFieldSet();

}//void ExttTableButtons(void)


void ExttTableAuxTable(void)
{

}//void ExttTableAuxTable(void)


void ExttTableGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uTable"))
		{
			sscanf(gentries[i].val,"%u",&uTable);
			guMode=6;
		}
	}
	tTable("");

}//void ExttTableGetHook(entry gentries[], int x)


void ExttTableSelect(void)
{

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTable ORDER BY"
				" uTable",
				VAR_LIST_tTable);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tTable,tClient WHERE tTable.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uTable",
					VAR_LIST_tTable,uContactParentCompany,uContactParentCompany);
					

}//void ExttTableSelect(void)


void ExttTableSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tTable WHERE uTable=%u",
			VAR_LIST_tTable,uTable);
	else
                sprintf(gcQuery,"SELECT %s FROM tTable,tClient"
                                " WHERE tTable.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tTable.uTable=%u",
                        		VAR_LIST_tTable
					,uContactParentCompany,uContactParentCompany
					,uTable);

}//void ExttTableSelectRow(void)


void ExttTableListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tTable",
				VAR_LIST_tTable);
	else
		sprintf(gcQuery,"SELECT %s FROM tTable,tClient"
				" WHERE tTable.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tTable
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttTableListFilter()
        if(!strcmp(gcFilter,"uTable"))
        {
                sscanf(gcCommand,"%u",&uTable);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tTable.uTable=%u"
						" ORDER BY uTable",
						uTable);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uTable");
        }

}//void ExttTableListSelect(void)


void ExttTableListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uTable"))
                printf("<option>uTable</option>");
        else
                printf("<option selected>uTable</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttTableListFilter(void)


void ExttTableNavBar(void)
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

}//void ExttTableNavBar(void)


void tTableNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uTable,cLabel FROM tTable ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tTable.uTable,"
				" tTable.cLabel"
				" FROM tTable,tClient"
				" WHERE tTable.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tTableNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tTableNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsRAD.cgi?gcFunction=tTable"
				"&uTable=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tTableNavList(void)


