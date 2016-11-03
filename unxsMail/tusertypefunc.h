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


void tUserTypeNavList(void);

void ExtProcesstUserTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstUserTypeVars(pentry entries[], int x)


void ExttUserTypeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tUserTypeTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstUserTypeVars(entries,x);
                        	guMode=2000;
	                        tUserType(LANG_NB_CONFIRMNEW);
			}
			else
				tUserType("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstUserTypeVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uUserType=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtUserType(0);
			}
			else
				tUserType("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstUserTypeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tUserType(LANG_NB_CONFIRMDEL);
			}
			else
				tUserType("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstUserTypeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetUserType();
			}
			else
				tUserType("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstUserTypeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tUserType(LANG_NB_CONFIRMMOD);
			}
			else
				tUserType("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstUserTypeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtUserType();
			}
			else
				tUserType("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttUserTypeCommands(pentry entries[], int x)


void ExttUserTypeButtons(void)
{
	OpenFieldSet("tUserType Aux Panel",100);
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
			tUserTypeNavList();
	}
	CloseFieldSet();

}//void ExttUserTypeButtons(void)


void ExttUserTypeAuxTable(void)
{

}//void ExttUserTypeAuxTable(void)


void ExttUserTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uUserType"))
		{
			sscanf(gentries[i].val,"%u",&uUserType);
			guMode=6;
		}
	}
	tUserType("");

}//void ExttUserTypeGetHook(entry gentries[], int x)


void ExttUserTypeSelect(void)
{
	ExtSelect("tUserType",VAR_LIST_tUserType,0);

}//void ExttUserTypeSelect(void)


void ExttUserTypeSelectRow(void)
{
	ExtSelectRow("tUserType",VAR_LIST_tUserType,uUserType);

}//void ExttUserTypeSelectRow(void)


void ExttUserTypeListSelect(void)
{
	char cCat[512];

	ExtListSelect("tUserType",VAR_LIST_tUserType);

	//Changes here must be reflected below in ExttUserTypeListFilter()
        if(!strcmp(gcFilter,"uUserType"))
        {
                sscanf(gcCommand,"%u",&uUserType);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tUserType.uUserType=%u \
						ORDER BY uUserType",
						uUserType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uUserType");
        }

}//void ExttUserTypeListSelect(void)


void ExttUserTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uUserType"))
                printf("<option>uUserType</option>");
        else
                printf("<option selected>uUserType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttUserTypeListFilter(void)


void ExttUserTypeNavBar(void)
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

}//void ExttUserTypeNavBar(void)


void tUserTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	
	ExtSelect("tUserType","tUserType.uUserType,tUserType.cLabel",20);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tUserTypeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tUserTypeNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tUserType&uUserType=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tUserTypeNavList(void)


