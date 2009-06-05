/*
FILE
	$Id$
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2007 Gary Wallis.
 
*/

//ModuleFunctionProtos()


void tSearchdomainNavList(void);

void ExtProcesstSearchdomainVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstSearchdomainVars(pentry entries[], int x)


void ExttSearchdomainCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tSearchdomainTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstSearchdomainVars(entries,x);
                        	guMode=2000;
	                        tSearchdomain(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstSearchdomainVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uSearchdomain=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtSearchdomain(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstSearchdomainVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
	                        guMode=2001;
				tSearchdomain(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstSearchdomainVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=5;
				DeletetSearchdomain();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstSearchdomainVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2002;
				tSearchdomain(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstSearchdomainVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtSearchdomain();
			}
                }
	}

}//void ExttSearchdomainCommands(pentry entries[], int x)


void ExttSearchdomainButtons(void)
{
	OpenFieldSet("tSearchdomain Aux Panel",100);
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
			tSearchdomainNavList();
	}
	CloseFieldSet();

}//void ExttSearchdomainButtons(void)


void ExttSearchdomainAuxTable(void)
{

}//void ExttSearchdomainAuxTable(void)


void ExttSearchdomainGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uSearchdomain"))
		{
			sscanf(gentries[i].val,"%u",&uSearchdomain);
			guMode=6;
		}
	}
	tSearchdomain("");

}//void ExttSearchdomainGetHook(entry gentries[], int x)


void ExttSearchdomainSelect(void)
{
	ExtSelect("tSearchdomain",VAR_LIST_tSearchdomain);

}//void ExttSearchdomainSelect(void)


void ExttSearchdomainSelectRow(void)
{
	ExtSelectRow("tSearchdomain",VAR_LIST_tSearchdomain,uSearchdomain);

}//void ExttSearchdomainSelectRow(void)


void ExttSearchdomainListSelect(void)
{
	char cCat[512];

	ExtListSelect("tSearchdomain",VAR_LIST_tSearchdomain);
	
	//Changes here must be reflected below in ExttSearchdomainListFilter()
        if(!strcmp(gcFilter,"uSearchdomain"))
        {
                sscanf(gcCommand,"%u",&uSearchdomain);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tSearchdomain.uSearchdomain=%u \
						ORDER BY uSearchdomain",
						uSearchdomain);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uSearchdomain");
        }

}//void ExttSearchdomainListSelect(void)


void ExttSearchdomainListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uSearchdomain"))
                printf("<option>uSearchdomain</option>");
        else
                printf("<option selected>uSearchdomain</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttSearchdomainListFilter(void)


void ExttSearchdomainNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10 && !guListMode)
		printf(LANG_NBB_NEW);

			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_MODIFY);

			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttSearchdomainNavBar(void)


void tSearchdomainNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tSearchdomain","tSearchdomain.uSearchdomain,tSearchdomain.cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tSearchdomainNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tSearchdomainNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tSearchdomain\
&uSearchdomain=%s>%s</a><br>\n",field[0],field[1]);
	        }
	}
        mysql_free_result(res);

}//void tSearchdomainNavList(void)


