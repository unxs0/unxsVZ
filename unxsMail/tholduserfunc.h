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


void tHoldUserNavList(void);

void ExtProcesstHoldUserVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstHoldUserVars(pentry entries[], int x)


void ExttHoldUserCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tHoldUserTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=8)
			{
	                        ProcesstHoldUserVars(entries,x);
                        	guMode=2000;
	                        tHoldUser(LANG_NB_CONFIRMNEW);
			}
			else
				tHoldUser("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=8)
			{
                        	ProcesstHoldUserVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uHoldUser=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtHoldUser(0);
			}
			else
				tHoldUser("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstHoldUserVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tHoldUser(LANG_NB_CONFIRMDEL);
			}
			else
				tHoldUser("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstHoldUserVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetHoldUser();
			}
			else
				tHoldUser("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstHoldUserVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tHoldUser(LANG_NB_CONFIRMMOD);
			}
			else
				tHoldUser("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstHoldUserVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtHoldUser();
			}
			else
				tHoldUser("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttHoldUserCommands(pentry entries[], int x)


void ExttHoldUserButtons(void)
{
	OpenFieldSet("tHoldUser Aux Panel",100);
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
			tHoldUserNavList();
	}
	CloseFieldSet();

}//void ExttHoldUserButtons(void)


void ExttHoldUserAuxTable(void)
{

}//void ExttHoldUserAuxTable(void)


void ExttHoldUserGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uHoldUser"))
		{
			sscanf(gentries[i].val,"%u",&uHoldUser);
			guMode=6;
		}
	}
	tHoldUser("");

}//void ExttHoldUserGetHook(entry gentries[], int x)


void ExttHoldUserSelect(void)
{
	ExtSelect("tHoldUser",VAR_LIST_tHoldUser,0);
	
}//void ExttHoldUserSelect(void)


void ExttHoldUserSelectRow(void)
{
	ExtSelectRow("tHoldUser",VAR_LIST_tHoldUser,uHoldUser);

}//void ExttHoldUserSelectRow(void)


void ExttHoldUserListSelect(void)
{
	char cCat[512];

	ExtListSelect("tHoldUser",VAR_LIST_tHoldUser);
	
	//Changes here must be reflected below in ExttHoldUserListFilter()
        if(!strcmp(gcFilter,"uHoldUser"))
        {
                sscanf(gcCommand,"%u",&uHoldUser);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tHoldUser.uHoldUser=%u \
						ORDER BY uHoldUser",
						uHoldUser);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uHoldUser");
        }

}//void ExttHoldUserListSelect(void)


void ExttHoldUserListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uHoldUser"))
                printf("<option>uHoldUser</option>");
        else
                printf("<option selected>uHoldUser</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttHoldUserListFilter(void)


void ExttHoldUserNavBar(void)
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

}//void ExttHoldUserNavBar(void)


void tHoldUserNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tHoldUserNavList</u><br>\n");
	
	ExtSelect("tHoldUser","tHoldUser.uHoldUser,tHoldUser.cLogin",20);

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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tHoldUser"
					"&uHoldUser=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tHoldUserNavList(void)


