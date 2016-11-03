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


void tShippingNavList(void);

void ExtProcesstShippingVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstShippingVars(pentry entries[], int x)


void ExttShippingCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tShippingTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstShippingVars(entries,x);
                        	guMode=2000;
	                        tShipping(LANG_NB_CONFIRMNEW);
			}
			else
				tShipping("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstShippingVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uShipping=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtShipping(0);
			}
			else
				tShipping("<blink>Error</blink>: Denied by permissions settings"); 
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstShippingVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))  
			{
	                        guMode=2001;
				tShipping(LANG_NB_CONFIRMDEL);
			}
			else
				tShipping("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstShippingVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))  
			{
				guMode=5;
				DeletetShipping();
			}
			else
				tShipping("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstShippingVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy)) 
			{
				guMode=2002;
				tShipping(LANG_NB_CONFIRMMOD);
			}
			else
				tShipping("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstShippingVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy)) 
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtShipping();
			}
			else
				tShipping("<blink>Error</blink>: Denied by permissions settings"); 
                }
	}

}//void ExttShippingCommands(pentry entries[], int x)


void ExttShippingButtons(void)
{
	OpenFieldSet("tShipping Aux Panel",100);
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
			tShippingNavList();
	}
	CloseFieldSet();

}//void ExttShippingButtons(void)


void ExttShippingAuxTable(void)
{

}//void ExttShippingAuxTable(void)


void ExttShippingGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uShipping"))
		{
			sscanf(gentries[i].val,"%u",&uShipping);
			guMode=6;
		}
	}
	tShipping("");

}//void ExttShippingGetHook(entry gentries[], int x)


void ExttShippingSelect(void)
{
	ExtSelect("tShipping",VAR_LIST_tShipping,0);
	
}//void ExttShippingSelect(void)


void ExttShippingSelectRow(void)
{
	ExtSelectRow("tShipping",VAR_LIST_tShipping,uShipping);

}//void ExttShippingSelectRow(void)


void ExttShippingListSelect(void)
{
	char cCat[512];

	ExtListSelect("tShipping",VAR_LIST_tShipping);

	//Changes here must be reflected below in ExttShippingListFilter()
        if(!strcmp(gcFilter,"uShipping"))
        {
                sscanf(gcCommand,"%u",&uShipping);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tShipping.uShipping=%u \
						ORDER BY uShipping",
						uShipping);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uShipping");
        }

}//void ExttShippingListSelect(void)


void ExttShippingListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uShipping"))
                printf("<option>uShipping</option>");
        else
                printf("<option selected>uShipping</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttShippingListFilter(void)


void ExttShippingNavBar(void)
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

}//void ExttShippingNavBar(void)


void tShippingNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tShippingNavList</u><br>\n");
	
	ExtSelect("tShipping","tShipping.uShipping,tShipping.cLabel",0);

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
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tShipping"
				"&uShipping=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tShippingNavList(void)


