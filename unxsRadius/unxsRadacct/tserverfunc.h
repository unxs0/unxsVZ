/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author: (C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details

 
*/

//ModuleFunctionProtos()


void tServerNavList(void);
void BasictServerCheck(void);


void ExtProcesstServerVars(pentry entries[], int x)
{

	/*
	register int i;
	
	for(i=0;i<x;i++)
	{
	
	}
	*/

}//void ExtProcesstServerVars(pentry entries[], int x)


void ExttServerCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tServerTools"))
	{
		//ModuleFunctionProcess()

		//Default wizard like two step creation and deletion
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			ProcesstServerVars(entries,x);
			if(guPermLevel>=12)
			{
				//Check global conditions for new record here
                        	guMode=2000;
   	                	tServer(LANG_NB_CONFIRMNEW);
			}
			else
				tServer("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			ProcesstServerVars(entries,x);
			if(guPermLevel>=12)
			{
				BasictServerCheck();
				uServer=0;
				uCreatedBy=guLoginClient;
				uOwner=guLoginClient;
				uModBy=0;//Never modified
				NewtServer(0);
			}
			else
				tServer("<blink>Error</blink>: Denied by permissions settings"); 
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstServerVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
                        	guMode=2001;
                        	tServer(LANG_NB_CONFIRMDEL);
			}
			else
				tServer("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstServerVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
                        	guMode=5;
                        	DeletetServer();
			}
			else
				tServer("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstServerVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
                        	guMode=2002;
                        	tServer(LANG_NB_CONFIRMMOD);
			}
			else
				tServer("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstServerVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				BasictServerCheck();
				uModBy=guLoginClient;
                        	ModtServer();
			}
			else
				tServer("<blink>Error</blink>: Denied by permissions settings"); 
                }
	}

}//void ExttServerCommands(pentry entries[], int x)


void ExttServerButtons(void)
{
	OpenFieldSet("tServer Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("Enter required data<br>");
                        printf(LANG_NBB_CONFIRMNEW);
			printf("<br>\n");
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
			printf("<br>\n");
                break;

                case 2002:
			printf("Review record data<br>");
                        printf(LANG_NBB_CONFIRMMOD);
			printf("<br>\n");
                break;

	}

        printf("<p><u>tServerNavList</u><br>\n");
	tServerNavList();
	
	CloseFieldSet();

}//void ExttServerButtons(void)


void ExttServerAuxTable(void)
{

}//void ExttServerAuxTable(void)


void ExttServerGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uServer"))
		{
			sscanf(gentries[i].val,"%u",&uServer);
			guMode=6;
		}
	}
	tServer("");

}//void ExttServerGetHook(entry gentries[], int x)


void ExttServerSelect(void)
{
        //Set non search query here for tTableName()
	ExtSelect("tServer",VAR_LIST_tServer,0);

}//void ExttServerSelect(void)


void ExttServerSelectRow(void)
{
	ExtSelectRow("tServer",VAR_LIST_tServer,uServer);

}//void ExttServerSelectRow(void)


void ExttServerListSelect(void)
{
	char cCat[512];

	ExtListSelect("tServer",VAR_LIST_tServer);

	//Changes here must be reflected below in ExttServerListFilter()
        if(!strcmp(gcFilter,"uServer"))
        {
                sscanf(gcCommand,"%u",&uServer);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tServer.uServer=%u \
						ORDER BY uServer",
						uServer);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uServer");
        }

}//void ExttServerListSelect(void)


void ExttServerListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uServer"))
                printf("<option>uServer</option>");
        else
                printf("<option selected>uServer</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttServerListFilter(void)


void ExttServerNavBar(void)
{
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

}//void ExttServerNavBar(void)


void tServerNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tServer","tServer.uServer,tServer.cLabel",0);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
                printf("<a class=darkLink href=unxsRadius.cgi?gcFunction=tServer"
			"&uServer=%s>%s</a><br>\n",field[0],field[1]);
        mysql_free_result(res);

}//void tServerNavList(void)


void BasictServerCheck(void)
{
	if(!cLabel[0])
		tServer("<blink>Error: </blink> cLabel is required");
	
	if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
	{
		MYSQL_RES *res;
		sprintf(gcQuery,"SELECT uServer FROM tServer WHERE cLabel='%s'",TextAreaSave(cLabel));

		mysql_query(&gMysql,gcQuery);

		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);

		if(mysql_num_rows(res))
			tServer("<blink>Error: </blink> cLabel already used");
	}

}//void BasictServerCheck(void)

