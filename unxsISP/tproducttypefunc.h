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


void tProductTypeNavList(void);

void ExtProcesstProductTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstProductTypeVars(pentry entries[], int x)


void ExttProductTypeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tProductTypeTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstProductTypeVars(entries,x);
                        	guMode=2000;
	                        tProductType(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstProductTypeVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uProductType=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtProductType(0);
			}
			else
				tProductType("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstProductTypeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy)) 
			{
	                        guMode=2001;
				tProductType(LANG_NB_CONFIRMDEL);
			}
			else
				tProductType("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstProductTypeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy)) 
			{
				guMode=5;
				DeletetProductType();
			}
			else
				tProductType("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstProductTypeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tProductType(LANG_NB_CONFIRMMOD);
			}
			else
				tProductType("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstProductTypeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtProductType();
			}
			else
				tProductType("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttProductTypeCommands(pentry entries[], int x)


void ExttProductTypeButtons(void)
{
	OpenFieldSet("tProductType Aux Panel",100);
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
			tProductTypeNavList();
	}
	CloseFieldSet();

}//void ExttProductTypeButtons(void)


void ExttProductTypeAuxTable(void)
{

}//void ExttProductTypeAuxTable(void)


void ExttProductTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uProductType"))
		{
			sscanf(gentries[i].val,"%u",&uProductType);
			guMode=6;
		}
	}
	tProductType("");

}//void ExttProductTypeGetHook(entry gentries[], int x)


void ExttProductTypeSelect(void)
{
	ExtSelect("tProductType",VAR_LIST_tProductType,0);
	
}//void ExttProductTypeSelect(void)


void ExttProductTypeSelectRow(void)
{
	ExtSelectRow("tProductType",VAR_LIST_tProductType,uProductType);

}//void ExttProductTypeSelectRow(void)


void ExttProductTypeListSelect(void)
{
	char cCat[512];

	ExtListSelect("tProductType",VAR_LIST_tProductType);

	//Changes here must be reflected below in ExttProductTypeListFilter()
        if(!strcmp(gcFilter,"uProductType"))
        {
                sscanf(gcCommand,"%u",&uProductType);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tProductType.uProductType=%u \
						ORDER BY uProductType",
						uProductType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uProductType");
        }

}//void ExttProductTypeListSelect(void)


void ExttProductTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uProductType"))
                printf("<option>uProductType</option>");
        else
                printf("<option selected>uProductType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttProductTypeListFilter(void)


void ExttProductTypeNavBar(void)
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

}//void ExttProductTypeNavBar(void)


void tProductTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tProductTypeNavList</u><br>\n");
	
	ExtSelect("tProductType","tProductType.uProductType,tProductType.cLabel",0);

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
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tProductType"
				"&uProductType=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tProductTypeNavList(void)


