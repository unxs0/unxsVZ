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


void tJobStatusNavList(void);

void ExtProcesstJobStatusVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstJobStatusVars(pentry entries[], int x)


void ExttJobStatusCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tJobStatusTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstJobStatusVars(entries,x);
                        	guMode=2000;
	                        tJobStatus(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstJobStatusVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uJobStatus=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtJobStatus(0);
			}
			else
				tJobStatus("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstJobStatusVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tJobStatus(LANG_NB_CONFIRMDEL);
			}
			else
				tJobStatus("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstJobStatusVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetJobStatus();
			}
			else
				tJobStatus("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstJobStatusVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tJobStatus(LANG_NB_CONFIRMMOD);
			}
			else
				tJobStatus("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstJobStatusVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtJobStatus();
			}
			else
				tJobStatus("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttJobStatusCommands(pentry entries[], int x)


void ExttJobStatusButtons(void)
{
	OpenFieldSet("tJobStatus Aux Panel",100);
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
			tJobStatusNavList();
	}
	CloseFieldSet();

}//void ExttJobStatusButtons(void)


void ExttJobStatusAuxTable(void)
{

}//void ExttJobStatusAuxTable(void)


void ExttJobStatusGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uJobStatus"))
		{
			sscanf(gentries[i].val,"%u",&uJobStatus);
			guMode=6;
		}
	}
	tJobStatus("");

}//void ExttJobStatusGetHook(entry gentries[], int x)


void ExttJobStatusSelect(void)
{
	ExtSelect("tJobStatus",VAR_LIST_tJobStatus,0);
	
}//void ExttJobStatusSelect(void)


void ExttJobStatusSelectRow(void)
{
	ExtSelectRow("tJobStatus",VAR_LIST_tJobStatus,uJobStatus);

}//void ExttJobStatusSelectRow(void)


void ExttJobStatusListSelect(void)
{
	char cCat[512];

	ExtListSelect("tJobStatus",VAR_LIST_tJobStatus);
	
	//Changes here must be reflected below in ExttJobStatusListFilter()
        if(!strcmp(gcFilter,"uJobStatus"))
        {
                sscanf(gcCommand,"%u",&uJobStatus);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tJobStatus.uJobStatus=%u \
						ORDER BY uJobStatus",
						uJobStatus);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uJobStatus");
        }

}//void ExttJobStatusListSelect(void)


void ExttJobStatusListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uJobStatus"))
                printf("<option>uJobStatus</option>");
        else
                printf("<option selected>uJobStatus</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttJobStatusListFilter(void)


void ExttJobStatusNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=12 && !guListMode)
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

}//void ExttJobStatusNavBar(void)


void tJobStatusNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tJobStatusNavList</u><br>\n");

	ExtSelect("tJobStatus","tJobStatus.uJobStatus,tJobStatus.cLabel",20);

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
			printf("<a class=darkLink href=unxsRadius.cgi?gcFunction=tJobStatus&uJobStatus=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tJobStatusNavList(void)


//perlSAR patch1
