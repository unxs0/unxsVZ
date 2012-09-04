/*
FILE
	$Id: tmonthfunc.h 1380 2010-04-27 15:02:47Z Gary $
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2007 Gary Wallis.
 
*/

//ModuleFunctionProtos()


void tRuleNavList(void);

void ExtProcesstRuleVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstRuleVars(pentry entries[], int x)


void ExttRuleCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tRuleTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstRuleVars(entries,x);
                        	guMode=2000;
	                        tRule(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstRuleVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uRule=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtRule(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstRuleVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
	                        guMode=2001;
				tRule(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstRuleVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=5;
				DeletetRule();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstRuleVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2002;
				tRule(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstRuleVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtRule();
			}
                }
	}

}//void ExttRuleCommands(pentry entries[], int x)


void ExttRuleButtons(void)
{
	OpenFieldSet("tRule Aux Panel",100);
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

			tRuleNavList();
	}
	CloseFieldSet();

}//void ExttRuleButtons(void)


void ExttRuleAuxTable(void)
{

}//void ExttRuleAuxTable(void)


void ExttRuleGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uRule"))
		{
			sscanf(gentries[i].val,"%u",&uRule);
			guMode=6;
		}
	}
	tRule("");

}//void ExttRuleGetHook(entry gentries[], int x)


void ExttRuleSelect(void)
{
        //Set non search gcQuery here for tTableName()
	ExtSelect("tRule",VAR_LIST_tRule);

}//void ExttRuleSelect(void)


void ExttRuleSelectRow(void)
{
	ExtSelectRow("tRule",VAR_LIST_tRule,uRule);

}//void ExttRuleSelectRow(void)


void ExttRuleListSelect(void)
{
	char cCat[512];

	ExtListSelect("tRule",VAR_LIST_tRule);

	//Changes here must be reflected below in ExttRuleListFilter()
        if(!strcmp(gcFilter,"uRule"))
        {
                sscanf(gcCommand,"%u",&uRule);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tRule.uRule=%u ORDER BY uRule",uRule);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uRule");
        }

}//void ExttRuleListSelect(void)


void ExttRuleListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uRule"))
                printf("<option>uRule</option>");
        else
                printf("<option selected>uRule</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttRuleListFilter(void)


void ExttRuleNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=12 && !guListMode)
		printf(LANG_NBB_NEW);

			if( (guPermLevel>=12 && uOwner==guLoginClient)
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

}//void ExttRuleNavBar(void)


void tRuleNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tRule","tRule.uRule,tRule.cLabel");
	
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tRuleNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tRuleNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tRule\
&uRule=%s>%s</a><br>\n",field[0],field[1]);
	        }
	}
        mysql_free_result(res);

}//void tRuleNavList(void)


