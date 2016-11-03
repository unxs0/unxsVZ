/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza for Unixservice.
 
*/

//ModuleFunctionProtos()


void tMySQLNavList(void);

void ExtProcesstMySQLVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstMySQLVars(pentry entries[], int x)


void ExttMySQLCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tMySQLTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstMySQLVars(entries,x);
                        	guMode=2000;
	                        tMySQL(LANG_NB_CONFIRMNEW);
			}
			else
				tMySQL("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstMySQLVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uMySQL=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtMySQL(0);
			}
			else
				tMySQL("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstMySQLVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tMySQL(LANG_NB_CONFIRMDEL);
			}
			else
				tMySQL("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstMySQLVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetMySQL();
			}
			else
				tMySQL("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstMySQLVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tMySQL(LANG_NB_CONFIRMMOD);
			}
			else
				tMySQL("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstMySQLVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtMySQL();
			}
			else
				tMySQL("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttMySQLCommands(pentry entries[], int x)


void ExttMySQLButtons(void)
{
	OpenFieldSet("tMySQL Aux Panel",100);
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
			tMySQLNavList();
	}
	CloseFieldSet();

}//void ExttMySQLButtons(void)


void ExttMySQLAuxTable(void)
{

}//void ExttMySQLAuxTable(void)


void ExttMySQLGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uMySQL"))
		{
			sscanf(gentries[i].val,"%u",&uMySQL);
			guMode=6;
		}
	}
	tMySQL("");

}//void ExttMySQLGetHook(entry gentries[], int x)


void ExttMySQLSelect(void)
{

	ExtSelect("tMySQL",VAR_LIST_tMySQL,0);	

}//void ExttMySQLSelect(void)


void ExttMySQLSelectRow(void)
{
	ExtSelectRow("tMySQL",VAR_LIST_tMySQL,uMySQL);

}//void ExttMySQLSelectRow(void)


void ExttMySQLListSelect(void)
{
	char cCat[512];

	ExtListSelect("tMySQL",VAR_LIST_tMySQL);

	//Changes here must be reflected below in ExttMySQLListFilter()
        if(!strcmp(gcFilter,"uMySQL"))
        {
                sscanf(gcCommand,"%u",&uMySQL);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tMySQL.uMySQL=%u"
						" ORDER BY uMySQL",
						uMySQL);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uMySQL");
        }

}//void ExttMySQLListSelect(void)


void ExttMySQLListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uMySQL"))
                printf("<option>uMySQL</option>");
        else
                printf("<option selected>uMySQL</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttMySQLListFilter(void)


void ExttMySQLNavBar(void)
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

}//void ExttMySQLNavBar(void)


void tMySQLNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies
	
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uMySQL,cLabel FROM tMySQL ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tMySQL.uMySQL,"
				" tMySQL.cLabel"
				" FROM tMySQL,tClient"
				" WHERE tMySQL.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				uContactParentCompany
				,uContactParentCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tMySQLNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tMySQLNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsApache.cgi?gcFunction=tMySQL"
				"&uMySQL=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tMySQLNavList(void)


