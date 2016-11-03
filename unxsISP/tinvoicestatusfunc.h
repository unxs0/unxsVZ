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


void tInvoiceStatusNavList(void);

void ExtProcesstInvoiceStatusVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstInvoiceStatusVars(pentry entries[], int x)


void ExttInvoiceStatusCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tInvoiceStatusTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstInvoiceStatusVars(entries,x);
                        	guMode=2000;
	                        tInvoiceStatus(LANG_NB_CONFIRMNEW);
			}
			else
				tInvoiceStatus("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstInvoiceStatusVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uInvoiceStatus=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtInvoiceStatus(0);
			}
			else
				tInvoiceStatus("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstInvoiceStatusVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
	                        guMode=2001;
				tInvoiceStatus(LANG_NB_CONFIRMDEL);
			}
			else
				tInvoiceStatus("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstInvoiceStatusVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
				guMode=5;
				DeletetInvoiceStatus();
			}
			else
				tInvoiceStatus("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstInvoiceStatusVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tInvoiceStatus(LANG_NB_CONFIRMMOD);
			}
			else
				tInvoiceStatus("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstInvoiceStatusVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtInvoiceStatus();
			}
			else
				tInvoiceStatus("<blink>Error</blink>: Denied by permissions settings");   
                }
	}

}//void ExttInvoiceStatusCommands(pentry entries[], int x)


void ExttInvoiceStatusButtons(void)
{
	OpenFieldSet("tInvoiceStatus Aux Panel",100);
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
			tInvoiceStatusNavList();
	}
	CloseFieldSet();

}//void ExttInvoiceStatusButtons(void)


void ExttInvoiceStatusAuxTable(void)
{

}//void ExttInvoiceStatusAuxTable(void)


void ExttInvoiceStatusGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uInvoiceStatus"))
		{
			sscanf(gentries[i].val,"%u",&uInvoiceStatus);
			guMode=6;
		}
	}
	tInvoiceStatus("");

}//void ExttInvoiceStatusGetHook(entry gentries[], int x)


void ExttInvoiceStatusSelect(void)
{
	ExtSelect("tInvoiceStatus",VAR_LIST_tInvoiceStatus,0);

}//void ExttInvoiceStatusSelect(void)


void ExttInvoiceStatusSelectRow(void)
{
	ExtSelectRow("tInvoiceStatus",VAR_LIST_tInvoiceStatus,uInvoiceStatus);

}//void ExttInvoiceStatusSelectRow(void)


void ExttInvoiceStatusListSelect(void)
{
	char cCat[512];

	ExtListSelect("tInvoiceStatus",VAR_LIST_tInvoiceStatus);

	//Changes here must be reflected below in ExttInvoiceStatusListFilter()
        if(!strcmp(gcFilter,"uInvoiceStatus"))
        {
                sscanf(gcCommand,"%u",&uInvoiceStatus);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tInvoiceStatus.uInvoiceStatus=%u \
						ORDER BY uInvoiceStatus",
						uInvoiceStatus);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uInvoiceStatus");
        }

}//void ExttInvoiceStatusListSelect(void)


void ExttInvoiceStatusListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uInvoiceStatus"))
                printf("<option>uInvoiceStatus</option>");
        else
                printf("<option selected>uInvoiceStatus</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttInvoiceStatusListFilter(void)


void ExttInvoiceStatusNavBar(void)
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

}//void ExttInvoiceStatusNavBar(void)


void tInvoiceStatusNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tInvoiceStatusNavList</u><br>\n");
	ExtSelect("tInvoiceStatus","tInvoiceStatus.uInvoiceStatus,tInvoiceStatus.cLabel",0);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tInvoiceStatusNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tInvoiceStatusNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tInvoiceStatus"
				"&uInvoiceStatus=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tInvoiceStatusNavList(void)


