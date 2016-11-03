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


void tRequestNavList(void);

void ExtProcesstRequestVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstRequestVars(pentry entries[], int x)


void ExttRequestCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tRequestTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstRequestVars(entries,x);
                        	guMode=2000;
	                        tRequest(LANG_NB_CONFIRMNEW);
			}
			else
				tRequest("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstRequestVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uRequest=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtRequest(0);
			}
			else
				tRequest("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstRequestVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tRequest(LANG_NB_CONFIRMDEL);
			}
			else
				tRequest("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstRequestVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetRequest();
			}
			else
				tRequest("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstRequestVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tRequest(LANG_NB_CONFIRMMOD);
			}
			else
				tRequest("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstRequestVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtRequest();
			}
			else
				tRequest("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttRequestCommands(pentry entries[], int x)


void ExttRequestButtons(void)
{
	OpenFieldSet("tRequest Aux Panel",100);
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
			tRequestNavList();
	}
	CloseFieldSet();

}//void ExttRequestButtons(void)


void ExttRequestAuxTable(void)
{

}//void ExttRequestAuxTable(void)


void ExttRequestGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uRequest"))
		{
			sscanf(gentries[i].val,"%u",&uRequest);
			guMode=6;
		}
	}
	tRequest("");

}//void ExttRequestGetHook(entry gentries[], int x)


void ExttRequestSelect(void)
{
	ExtSelect("tRequest",VAR_LIST_tRequest,0);
	
}//void ExttRequestSelect(void)


void ExttRequestSelectRow(void)
{
	ExtSelectRow("tRequest",VAR_LIST_tRequest,uRequest);

}//void ExttRequestSelectRow(void)


void ExttRequestListSelect(void)
{
	char cCat[512];

	ExtListSelect("tRequest",VAR_LIST_tRequest);

	//Changes here must be reflected below in ExttRequestListFilter()
        if(!strcmp(gcFilter,"uRequest"))
        {
                sscanf(gcCommand,"%u",&uRequest);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tRequest.uRequest=%u \
						ORDER BY uRequest",
						uRequest);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uRequest");
        }

}//void ExttRequestListSelect(void)


void ExttRequestListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uRequest"))
                printf("<option>uRequest</option>");
        else
                printf("<option selected>uRequest</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttRequestListFilter(void)


void ExttRequestNavBar(void)
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

}//void ExttRequestNavBar(void)


void tRequestNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tRequest","uRequest,cId,cLogin",20);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tRequestNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tRequestNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tRequest"
				"&uRequest=%s>%s/%s</a><br>\n",field[0],field[1],field[2]);
	}
        mysql_free_result(res);

}//void tRequestNavList(void)


