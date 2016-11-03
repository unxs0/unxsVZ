/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquizaw.
 
*/

//ModuleFunctionProtos()


void tIPNavList(void);

void ExtProcesstIPVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstIPVars(pentry entries[], int x)


void ExttIPCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tIPTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstIPVars(entries,x);
                        	guMode=2000;
	                        tIP(LANG_NB_CONFIRMNEW);
			}
			else
				tIP("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstIPVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uIP=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtIP(0);
			}
			else
				tIP("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstIPVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy)) 
			{
	                        guMode=2001;
				tIP(LANG_NB_CONFIRMDEL);
			}
			else
				tIP("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstIPVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy)) 
			{
				guMode=5;
				DeletetIP();
			}
			else
				tIP("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstIPVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy)) 
			{
				guMode=2002;
				tIP(LANG_NB_CONFIRMMOD);
			}
			else
				tIP("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstIPVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy)) 
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtIP();
			}
			else
				tIP("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttIPCommands(pentry entries[], int x)


void ExttIPButtons(void)
{
	OpenFieldSet("tIP Aux Panel",100);
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
			tIPNavList();
	}
	CloseFieldSet();

}//void ExttIPButtons(void)


void ExttIPAuxTable(void)
{

}//void ExttIPAuxTable(void)


void ExttIPGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uIP"))
		{
			sscanf(gentries[i].val,"%u",&uIP);
			guMode=6;
		}
	}
	tIP("");

}//void ExttIPGetHook(entry gentries[], int x)


void ExttIPSelect(void)
{
	ExtSelect("tIP",VAR_LIST_tIP,0);
	
}//void ExttIPSelect(void)


void ExttIPSelectRow(void)
{
	ExtSelectRow("tIP",VAR_LIST_tIP,uIP);

}//void ExttIPSelectRow(void)


void ExttIPListSelect(void)
{
	char cCat[512];

	ExtListSelect("tIP",VAR_LIST_tIP);

	//Changes here must be reflected below in ExttIPListFilter()
        if(!strcmp(gcFilter,"uIP"))
        {
                sscanf(gcCommand,"%u",&uIP);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tIP.uIP=%u \
						ORDER BY uIP",
						uIP);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uIP");
        }

}//void ExttIPListSelect(void)


void ExttIPListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uIP"))
                printf("<option>uIP</option>");
        else
                printf("<option selected>uIP</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttIPListFilter(void)


void ExttIPNavBar(void)
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
	
}//void ExttIPNavBar(void)


void tIPNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tIPNavList</u><br>\n");
	
	ExtSelect("tIP","uIP,cIP",0);

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
			printf("<a class=darkLink href=unxsApache.cgi?gcFunction=tIP"
				"&uIP=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tIPNavList(void)


//perlSAR patch1
