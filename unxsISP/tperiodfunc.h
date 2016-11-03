/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
 
*/

//ModuleFunctionProtos()


void tPeriodNavList(void);

void ExtProcesstPeriodVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstPeriodVars(pentry entries[], int x)


void ExttPeriodCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tPeriodTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstPeriodVars(entries,x);
                        	guMode=2000;
	                        tPeriod(LANG_NB_CONFIRMNEW);
			}
			else
				tPeriod("<blink>Error</blink>: Denied by permissions settings");    
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstPeriodVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uPeriod=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtPeriod(0);
			}
			else
				tPeriod("<blink>Error</blink>: Denied by permissions settings");    
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstPeriodVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tPeriod(LANG_NB_CONFIRMDEL);
			}
			else
				tPeriod("<blink>Error</blink>: Denied by permissions settings");    
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstPeriodVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetPeriod();
			}
			else
				tPeriod("<blink>Error</blink>: Denied by permissions settings");    
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstPeriodVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy)) 
			{
				guMode=2002;
				tPeriod(LANG_NB_CONFIRMMOD);
			}
			else
				tPeriod("<blink>Error</blink>: Denied by permissions settings");    
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstPeriodVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy)) 
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtPeriod();
			}
			else
				tPeriod("<blink>Error</blink>: Denied by permissions settings");    
                }
	}

}//void ExttPeriodCommands(pentry entries[], int x)


void ExttPeriodButtons(void)
{
	OpenFieldSet("tPeriod Aux Panel",100);
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
			tPeriodNavList();
	}
	CloseFieldSet();

}//void ExttPeriodButtons(void)


void ExttPeriodAuxTable(void)
{

}//void ExttPeriodAuxTable(void)


void ExttPeriodGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uPeriod"))
		{
			sscanf(gentries[i].val,"%u",&uPeriod);
			guMode=6;
		}
	}
	tPeriod("");

}//void ExttPeriodGetHook(entry gentries[], int x)


void ExttPeriodSelect(void)
{
	ExtSelect("tPeriod",VAR_LIST_tPeriod,0);

}//void ExttPeriodSelect(void)


void ExttPeriodSelectRow(void)
{
	ExtSelectRow("tPeriod",VAR_LIST_tPeriod,uPeriod);

}//void ExttPeriodSelectRow(void)


void ExttPeriodListSelect(void)
{
	char cCat[512];

	ExtListSelect("tPeriod",VAR_LIST_tPeriod);

	//Changes here must be reflected below in ExttPeriodListFilter()
        if(!strcmp(gcFilter,"uPeriod"))
        {
                sscanf(gcCommand,"%u",&uPeriod);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tPeriod.uPeriod=%u \
						ORDER BY uPeriod",
						uPeriod);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uPeriod");
        }

}//void ExttPeriodListSelect(void)


void ExttPeriodListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uPeriod"))
                printf("<option>uPeriod</option>");
        else
                printf("<option selected>uPeriod</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttPeriodListFilter(void)


void ExttPeriodNavBar(void)
{

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10 && !guListMode)
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

}//void ExttPeriodNavBar(void)


void tPeriodNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tPeriodNavList</u><br>\n");
	
	ExtSelect("tPeriod","tPeriod.uPeriod,tPeriod.cLabel",0);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tPeriod"
				"&uPeriod=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tPeriodNavList(void)


