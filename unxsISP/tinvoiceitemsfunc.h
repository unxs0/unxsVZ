/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author: (C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details

 
*/


static unsigned uSearch=0;

void ExtProcesstInvoiceItemsVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstInvoiceItemsVars(pentry entries[], int x)


void ExttInvoiceItemsCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tInvoiceItemsTools"))
	{
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstInvoiceItemsVars(entries,x);
                        	guMode=2000;
	                        tInvoiceItems(LANG_NB_CONFIRMNEW);
			}
			else
				tInvoiceItems("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstInvoiceItemsVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uInvoiceItems=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtInvoiceItems(0);
			}
			else
				tInvoiceItems("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstInvoiceItemsVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
	                        guMode=2001;
				tInvoiceItems(LANG_NB_CONFIRMDEL);
			}
			else
				tInvoiceItems("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstInvoiceItemsVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
				guMode=5;
				DeletetInvoiceItems();
			}
			else
				tInvoiceItems("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstInvoiceItemsVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				guMode=2002;
				tInvoiceItems(LANG_NB_CONFIRMMOD);
			}
			else
				tInvoiceItems("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstInvoiceItemsVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtInvoiceItems();
			}
			else
				tInvoiceItems("<blink>Error</blink>: Denied by permissions settings");   
                }
	}

}//void ExttInvoiceItemsCommands(pentry entries[], int x)


void ExttInvoiceItemsButtons(void)
{
	OpenFieldSet("tInvoiceItems Aux Panel",100);
	
	if(uInvoice)
	{
		if(uSearch)
		printf("<a class=darkLink title='Return to same tInvoice' href=unxsISP.cgi?gcFunction=tInvoice&uInvoice=%u&uSearch=%u>[Return]</a><p>\n",
				uInvoice,uSearch);
		else
		printf("<a class=darkLink title='Return to same tInvoice' href=unxsISP.cgi?gcFunction=tInvoice&uInvoice=%u>[Return]</a><p>\n",uInvoice);
	}

	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data</u><br>");
                        printf("<font size=1>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p><font size=1>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf("<font size=1>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

	}

	CloseFieldSet();

}//void ExttInvoiceItemsButtons(void)


void ExttInvoiceItemsAuxTable(void)
{

}//void ExttInvoiceItemsAuxTable(void)


void ExttInvoiceItemsGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uInvoiceItems"))
		{
			sscanf(gentries[i].val,"%u",&uInvoiceItems);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"uSearch"))
		{
			sscanf(gentries[i].val,"%u",&uSearch);
		}
	}
	tInvoiceItems("");

}//void ExttInvoiceItemsGetHook(entry gentries[], int x)


void ExttInvoiceItemsSelect(void)
{
	ExtSelect("tInvoiceItems",VAR_LIST_tInvoiceItems,0);
	
}//void ExttInvoiceItemsSelect(void)


void ExttInvoiceItemsSelectRow(void)
{
	ExtSelectRow("tInvoiceItems",VAR_LIST_tInvoiceItems,uInvoiceItems);

}//void ExttInvoiceItemsSelectRow(void)


void ExttInvoiceItemsListSelect(void)
{
	char cCat[512];

	ExtListSelect("tInvoiceItems",VAR_LIST_tInvoiceItems);

	//Changes here must be reflected below in ExttInvoiceItemsListFilter()
        if(!strcmp(gcFilter,"uInvoiceItems"))
        {
                sscanf(gcCommand,"%u",&uInvoiceItems);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tInvoiceItems.uInvoiceItems=%u \
						ORDER BY uInvoiceItems",
						uInvoiceItems);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uInvoiceItems");
        }

}//void ExttInvoiceItemsListSelect(void)


void ExttInvoiceItemsListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uInvoiceItems"))
                printf("<option>uInvoiceItems</option>");
        else
                printf("<option selected>uInvoiceItems</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttInvoiceItemsListFilter(void)


void ExttInvoiceItemsNavBar(void)
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

}//void ExttInvoiceItemsNavBar(void)


