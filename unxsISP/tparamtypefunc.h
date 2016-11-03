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


void tParamTypeNavList(void);

void ExtProcesstParamTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstParamTypeVars(pentry entries[], int x)


void ExttParamTypeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tParamTypeTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=11)
			{
	                        ProcesstParamTypeVars(entries,x);
                        	guMode=2000;
	                        tParamType(LANG_NB_CONFIRMNEW);
			}
			else
				tParamType("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=11)
			{
                        	ProcesstParamTypeVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uParamType=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtParamType(0);
			}
			else
				tParamType("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstParamTypeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
	                        guMode=2001;
				tParamType(LANG_NB_CONFIRMDEL);
			}
			else
				tParamType("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstParamTypeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
				guMode=5;
				DeletetParamType();
			}
			else
				tParamType("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstParamTypeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				guMode=2002;
				tParamType(LANG_NB_CONFIRMMOD);
			}
			else
				tParamType("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstParamTypeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtParamType();
			}
			else
				tParamType("<blink>Error</blink>: Denied by permissions settings");   
                }
	}

}//void ExttParamTypeCommands(pentry entries[], int x)


void ExttParamTypeButtons(void)
{
	OpenFieldSet("tParamType Aux Panel",100);
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
			tParamTypeNavList();
	}
	CloseFieldSet();

}//void ExttParamTypeButtons(void)


void ExttParamTypeAuxTable(void)
{

}//void ExttParamTypeAuxTable(void)


void ExttParamTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uParamType"))
		{
			sscanf(gentries[i].val,"%u",&uParamType);
			guMode=6;
		}
	}
	tParamType("");

}//void ExttParamTypeGetHook(entry gentries[], int x)


void ExttParamTypeSelect(void)
{
	ExtSelectAdmin("tParamType",VAR_LIST_tParamType,0);
	
}//void ExttParamTypeSelect(void)


void ExttParamTypeSelectRow(void)
{
	ExtSelectRowAdmin("tParamType",VAR_LIST_tParamType,uParamType);

}//void ExttParamTypeSelectRow(void)


void ExttParamTypeListSelect(void)
{
	char cCat[512];
	
	ExtListSelectAdmin("tParamType",VAR_LIST_tParamType);

	//Changes here must be reflected below in ExttParamTypeListFilter()
        if(!strcmp(gcFilter,"uParamType"))
        {
                sscanf(gcCommand,"%u",&uParamType);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tParamType.uParamType=%u \
						ORDER BY uParamType",
						uParamType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uParamType");
        }

}//void ExttParamTypeListSelect(void)


void ExttParamTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uParamType"))
                printf("<option>uParamType</option>");
        else
                printf("<option selected>uParamType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttParamTypeListFilter(void)


void ExttParamTypeNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=11 && !guListMode)
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

}//void ExttParamTypeNavBar(void)


void tParamTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelectAdmin("tParamType","tParamType.uParamType,tParamType.cLabel",0);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tParamTypeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tParamTypeNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tParamType"
				"&uParamType=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tParamTypeNavList(void)


