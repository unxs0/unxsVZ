/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
 
*/

static char cSearch[64]={""};


void tConfigSpecNavList(void);

void ExtProcesstConfigSpecVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.63s",entries[i].val);
		}
	}
}//void ExtProcesstConfigSpecVars(pentry entries[], int x)


void ExttConfigSpecCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tConfigSpecTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstConfigSpecVars(entries,x);
                        	guMode=2000;
	                        tConfigSpec(LANG_NB_CONFIRMNEW);
			}
			else
				tConfigSpec("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstConfigSpecVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uConfigSpec=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtConfigSpec(0);
			}
			else
				tConfigSpec("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstConfigSpecVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tConfigSpec(LANG_NB_CONFIRMDEL);
			}
			else
				tConfigSpec("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstConfigSpecVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetConfigSpec();
			}
			else
				tConfigSpec("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstConfigSpecVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tConfigSpec(LANG_NB_CONFIRMMOD);
			}
			else
				tConfigSpec("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstConfigSpecVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtConfigSpec();
			}
			else
				tConfigSpec("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttConfigSpecCommands(pentry entries[], int x)


void ExttConfigSpecButtons(void)
{
	OpenFieldSet("tConfigSpec Aux Panel",100);
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
			printf("The tConfigSpec table is used to associate a uTemplate template with a destination file path and "
				"other file related properties. This allows us to push new configuration files based on templates "
				"to a set of servers via a server agent (usually just another unxsMail running a job queue via "
				"cron.) This agent then installs them in the correct place with the correct ownership and file permissions.");
			if(uConfigSpec)
			{
				printf("<p><u>Record Context Info</u><br>");
				printf("<p><input title='Enter a mySQL search pattern for cDomain' type=text "
					"name=cSearch value='%s' maxlength=63 size=20> cSearch\n",cSearch);
				tConfigSpecNavList();
			}
	}
	CloseFieldSet();

}//void ExttConfigSpecButtons(void)


void ExttConfigSpecAuxTable(void)
{

}//void ExttConfigSpecAuxTable(void)


void ExttConfigSpecGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uConfigSpec"))
		{
			sscanf(gentries[i].val,"%u",&uConfigSpec);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.63s",gentries[i].val);
		}
	}
	tConfigSpec("");

}//void ExttConfigSpecGetHook(entry gentries[], int x)


void ExttConfigSpecSelect(void)
{
	if(cSearch[0])
		ExtSelectSearch("tConfigSpec",VAR_LIST_tConfigSpec,"tConfigSpec.cLabel",cSearch,NULL,20);
	else
		ExtSelect("tConfigSpec",VAR_LIST_tConfigSpec,0);

}//void ExttConfigSpecSelect(void)


void ExttConfigSpecSelectRow(void)
{
	ExtSelectRow("tConfigSpec",VAR_LIST_tConfigSpec,uConfigSpec);

}//void ExttConfigSpecSelectRow(void)


void ExttConfigSpecListSelect(void)
{
	char cCat[512];
	
	ExtListSelect("tConfigSpec",VAR_LIST_tConfigSpec);

	//Changes here must be reflected below in ExttConfigSpecListFilter()
        if(!strcmp(gcFilter,"uConfigSpec"))
        {
                sscanf(gcCommand,"%u",&uConfigSpec);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tConfigSpec.uConfigSpec=%u \
						ORDER BY uConfigSpec",
						uConfigSpec);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uConfigSpec");
        }

}//void ExttConfigSpecListSelect(void)


void ExttConfigSpecListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uConfigSpec"))
                printf("<option>uConfigSpec</option>");
        else
                printf("<option selected>uConfigSpec</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttConfigSpecListFilter(void)


void ExttConfigSpecNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

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

}//void ExttConfigSpecNavBar(void)


void tConfigSpecNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tConfigSpecNavList</u><br>\n");
	
	if(!cSearch[0])
	{
        	printf("<br>Must restrict via cSearch\n");
		return;
	}


	ExtSelectSearch("tConfigSpec","tConfigSpec.uConfigSpec,tConfigSpec.cLabel","tConfigSpec.cLabel",cSearch,NULL,20);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tConfigSpecNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tConfigSpec&uConfigSpec=%s&cSearch=%s>%s</a><br>\n",
					field[0]
					,cURLEncode(cSearch)
					,field[1]);
	}
        mysql_free_result(res);

}//void tConfigSpecNavList(void)

