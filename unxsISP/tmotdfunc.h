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


void tMotdNavList(void);

void ExtProcesstMotdVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstMotdVars(pentry entries[], int x)


void ExttMotdCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tMotdTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstMotdVars(entries,x);
                        	guMode=2000;
	                        tMotd(LANG_NB_CONFIRMNEW);
			}
			else
				tMotd("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstMotdVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uMotd=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtMotd(0);
			}
			else
				tMotd("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstMotdVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tMotd(LANG_NB_CONFIRMDEL);
			}
			else
				tMotd("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstMotdVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetMotd();
			}
			else
				tMotd("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstMotdVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tMotd(LANG_NB_CONFIRMMOD);
			}
			else
				tMotd("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstMotdVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtMotd();
			}
			else
				tMotd("<blink>Error</blink>: Denied by permissions settings");   
                }
	}

}//void ExttMotdCommands(pentry entries[], int x)


void ExttMotdButtons(void)
{
	OpenFieldSet("tMotd Aux Panel",100);
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
			tMotdNavList();
	}
	CloseFieldSet();

}//void ExttMotdButtons(void)


void ExttMotdAuxTable(void)
{

}//void ExttMotdAuxTable(void)


void ExttMotdGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uMotd"))
		{
			sscanf(gentries[i].val,"%u",&uMotd);
			guMode=6;
		}
	}
	tMotd("");

}//void ExttMotdGetHook(entry gentries[], int x)


void ExttMotdSelect(void)
{
	ExtSelect("tMotd",VAR_LIST_tMotd,0);
	
}//void ExttMotdSelect(void)


void ExttMotdSelectRow(void)
{
	ExtSelectRow("tMotd",VAR_LIST_tMotd,uMotd);
	
}//void ExttMotdSelectRow(void)


void ExttMotdListSelect(void)
{
	char cCat[512];

	ExtListSelect("tMotd",VAR_LIST_tMotd);

	//Changes here must be reflected below in ExttMotdListFilter()
        if(!strcmp(gcFilter,"uMotd"))
        {
                sscanf(gcCommand,"%u",&uMotd);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tMotd.uMotd=%u \
						ORDER BY uMotd",
						uMotd);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uMotd");
        }

}//void ExttMotdListSelect(void)


void ExttMotdListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uMotd"))
                printf("<option>uMotd</option>");
        else
                printf("<option selected>uMotd</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttMotdListFilter(void)


void ExttMotdNavBar(void)
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

}//void ExttMotdNavBar(void)


void tMotdNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tMotd","tMotd.uMotd,FROM_UNIXTIME(GREATEST(tMotd.uCreatedDate,tMotd.uModDate))",0);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tMotdNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tMotdNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tMotd"
				"&uMotd=%s>%s</a><br>\n",field[0],field[1]);
	        
	}
        mysql_free_result(res);

}//void tMotdNavList(void)


