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


void tServerGroupNavList(void);

void ExtProcesstServerGroupVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstServerGroupVars(pentry entries[], int x)


void ExttServerGroupCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tServerGroupTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstServerGroupVars(entries,x);
                        	guMode=2000;
	                        tServerGroup(LANG_NB_CONFIRMNEW);
			}
			else
				tServerGroup("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstServerGroupVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uServerGroup=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtServerGroup(0);
			}
			else
				tServerGroup("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstServerGroupVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tServerGroup(LANG_NB_CONFIRMDEL);
			}
			else
				tServerGroup("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstServerGroupVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetServerGroup();
			}
			else
				tServerGroup("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstServerGroupVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tServerGroup(LANG_NB_CONFIRMMOD);
			}
			else
				tServerGroup("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstServerGroupVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtServerGroup();
			}
			else
				tServerGroup("<blink>Error</blink>: Denied by permissions settings");

                }
	}

}//void ExttServerGroupCommands(pentry entries[], int x)


void ExttServerGroupButtons(void)
{
	OpenFieldSet("tServerGroup Aux Panel",100);
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
			tServerGroupNavList();
	}
	CloseFieldSet();

}//void ExttServerGroupButtons(void)


void ExttServerGroupAuxTable(void)
{

}//void ExttServerGroupAuxTable(void)


void ExttServerGroupGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uServerGroup"))
		{
			sscanf(gentries[i].val,"%u",&uServerGroup);
			guMode=6;
		}
	}
	tServerGroup("");

}//void ExttServerGroupGetHook(entry gentries[], int x)


void ExttServerGroupSelect(void)
{
        //Set non search gcQuery here for tTableName()
	ExtSelect("tServerGroup",VAR_LIST_tServerGroup,0);

}//void ExttServerGroupSelect(void)


void ExttServerGroupSelectRow(void)
{
	ExtSelectRow("tServerGroup",VAR_LIST_tServerGroup,uServerGroup);

}//void ExttServerGroupSelectRow(void)


void ExttServerGroupListSelect(void)
{
	char cCat[512];

	ExtListSelect("tServerGroup",VAR_LIST_tServerGroup);

	//Changes here must be reflected below in ExttServerGroupListFilter()
        if(!strcmp(gcFilter,"uServerGroup"))
        {
                sscanf(gcCommand,"%u",&uServerGroup);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tServerGroup.uServerGroup=%u \
						ORDER BY uServerGroup",
						uServerGroup);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uServerGroup");
        }

}//void ExttServerGroupListSelect(void)


void ExttServerGroupListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uServerGroup"))
                printf("<option>uServerGroup</option>");
        else
                printf("<option selected>uServerGroup</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttServerGroupListFilter(void)


void ExttServerGroupNavBar(void)
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

}//void ExttServerGroupNavBar(void)


void tServerGroupNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tServerGroupNavList</u><br>\n");

	ExtSelect("tServerGroup","tServerGroup.uServerGroup,tServerGroup.cLabel",0);

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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tServerGroup"
				"&uServerGroup=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tServerGroupNavList(void)


