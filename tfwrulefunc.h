/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2016 Gary Wallis for Unixservice, LLC.
 
*/

//ModuleFunctionProtos()


void tFWRuleNavList(void);

void ExtProcesstFWRuleVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstFWRuleVars(pentry entries[], int x)


void ExttFWRuleCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tFWRuleTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstFWRuleVars(entries,x);
                        	guMode=2000;
	                        tFWRule(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstFWRuleVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uFWRule=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtFWRule(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstFWRuleVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tFWRule(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstFWRuleVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetFWRule();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstFWRuleVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tFWRule(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstFWRuleVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtFWRule();
			}
                }
	}

}//void ExttFWRuleCommands(pentry entries[], int x)


void ExttFWRuleButtons(void)
{
	OpenFieldSet("tFWRule Aux Panel",100);
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
			printf("<p><u>Table Tips (%s)</u><br>",cGitVersion);

			tFWRuleNavList();
	}
	CloseFieldSet();

}//void ExttFWRuleButtons(void)


void ExttFWRuleAuxTable(void)
{

}//void ExttFWRuleAuxTable(void)


void ExttFWRuleGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uFWRule"))
		{
			sscanf(gentries[i].val,"%u",&uFWRule);
			guMode=6;
		}
	}
	tFWRule("");

}//void ExttFWRuleGetHook(entry gentries[], int x)


void ExttFWRuleSelect(void)
{
	ExtSelect("tFWRule",VAR_LIST_tFWRule);

}//void ExttFWRuleSelect(void)


void ExttFWRuleSelectRow(void)
{
	ExtSelectRow("tFWRule",VAR_LIST_tFWRule,uFWRule);

}//void ExttFWRuleSelectRow(void)


void ExttFWRuleListSelect(void)
{
	char cCat[512];

	ExtListSelect("tFWRule",VAR_LIST_tFWRule);

	//Changes here must be reflected below in ExttFWRuleListFilter()
        if(!strcmp(gcFilter,"uFWRule"))
        {
                sscanf(gcCommand,"%u",&uFWRule);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tFWRule.uFWRule=%u \
						ORDER BY uFWRule",
						uFWRule);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uFWRule");
        }

}//void ExttFWRuleListSelect(void)


void ExttFWRuleListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uFWRule"))
                printf("<option>uFWRule</option>");
        else
                printf("<option selected>uFWRule</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttFWRuleListFilter(void)


void ExttFWRuleNavBar(void)
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

}//void ExttFWRuleNavBar(void)


void tFWRuleNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tFWRule","tFWRule.uFWRule,tFWRule.cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tFWRuleNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tFWRuleNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tFWRule&uFWRule=%s>%s</a><br>",
					field[0],field[1]);
	}
        mysql_free_result(res);

}//void tFWRuleNavList(void)


