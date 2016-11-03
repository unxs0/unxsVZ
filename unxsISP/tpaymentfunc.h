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


void tPaymentNavList(void);

void ExtProcesstPaymentVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstPaymentVars(pentry entries[], int x)


void ExttPaymentCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tPaymentTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstPaymentVars(entries,x);
                        	guMode=2000;
	                        tPayment(LANG_NB_CONFIRMNEW);
			}
			else
				tPayment("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstPaymentVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uPayment=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtPayment(0);
			}
			else
				tPayment("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstPaymentVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
	                        guMode=2001;
				tPayment(LANG_NB_CONFIRMDEL);
			}
			else
				tPayment("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstPaymentVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
				guMode=5;
				DeletetPayment();
			}
			else
				tPayment("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstPaymentVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tPayment(LANG_NB_CONFIRMMOD);
			}
			else
				tPayment("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstPaymentVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtPayment();
			}
			else
				tPayment("<blink>Error</blink>: Denied by permissions settings");   
                }
	}

}//void ExttPaymentCommands(pentry entries[], int x)


void ExttPaymentButtons(void)
{
	OpenFieldSet("tPayment Aux Panel",100);
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
			tPaymentNavList();
	}
	CloseFieldSet();

}//void ExttPaymentButtons(void)


void ExttPaymentAuxTable(void)
{

}//void ExttPaymentAuxTable(void)


void ExttPaymentGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uPayment"))
		{
			sscanf(gentries[i].val,"%u",&uPayment);
			guMode=6;
		}
	}
	tPayment("");

}//void ExttPaymentGetHook(entry gentries[], int x)


void ExttPaymentSelect(void)
{
	ExtSelect("tPayment",VAR_LIST_tPayment,0);
	
}//void ExttPaymentSelect(void)


void ExttPaymentSelectRow(void)
{
	ExtSelectRow("tPayment",VAR_LIST_tPayment,uPayment);

}//void ExttPaymentSelectRow(void)


void ExttPaymentListSelect(void)
{
	char cCat[512];

	ExtListSelect("tPayment",VAR_LIST_tPayment);

	//Changes here must be reflected below in ExttPaymentListFilter()
        if(!strcmp(gcFilter,"uPayment"))
        {
                sscanf(gcCommand,"%u",&uPayment);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tPayment.uPayment=%u \
						ORDER BY uPayment",
						uPayment);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uPayment");
        }

}//void ExttPaymentListSelect(void)


void ExttPaymentListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uPayment"))
                printf("<option>uPayment</option>");
        else
                printf("<option selected>uPayment</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttPaymentListFilter(void)


void ExttPaymentNavBar(void)
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

}//void ExttPaymentNavBar(void)


void tPaymentNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tPaymentNavList</u><br>\n");

	ExtSelect("tPayment","tPayment.uPayment,tPayment.cLabel",0);

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
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tPayment"
				"&uPayment=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tPaymentNavList(void)


