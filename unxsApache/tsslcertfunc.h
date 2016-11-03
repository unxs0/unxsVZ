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


void tSSLCertNavList(void);

void ExtProcesstSSLCertVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstSSLCertVars(pentry entries[], int x)


void ExttSSLCertCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tSSLCertTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstSSLCertVars(entries,x);
                        	guMode=2000;
	                        tSSLCert(LANG_NB_CONFIRMNEW);
			}
			else
				tSSLCert("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstSSLCertVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uSSLCert=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtSSLCert(0);
			}
			else
				tSSLCert("<blink>Error</blink>: Denied by permissions settings"); 
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstSSLCertVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy)) 
			{
	                        guMode=2001;
				tSSLCert(LANG_NB_CONFIRMDEL);
			}
			else
				tSSLCert("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstSSLCertVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy)) 
			{
				guMode=5;
				DeletetSSLCert();
			}
			else
				tSSLCert("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstSSLCertVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy)) 
			{
				guMode=2002;
				tSSLCert(LANG_NB_CONFIRMMOD);
			}
			else
				tSSLCert("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstSSLCertVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy)) 
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtSSLCert();
			}
			else
				tSSLCert("<blink>Error</blink>: Denied by permissions settings"); 
                }
	}

}//void ExttSSLCertCommands(pentry entries[], int x)


void ExttSSLCertButtons(void)
{
	OpenFieldSet("tSSLCert Aux Panel",100);
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
			tSSLCertNavList();
	}
	CloseFieldSet();

}//void ExttSSLCertButtons(void)


void ExttSSLCertAuxTable(void)
{

}//void ExttSSLCertAuxTable(void)


void ExttSSLCertGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uSSLCert"))
		{
			sscanf(gentries[i].val,"%u",&uSSLCert);
			guMode=6;
		}
	}
	tSSLCert("");

}//void ExttSSLCertGetHook(entry gentries[], int x)


void ExttSSLCertSelect(void)
{
        //Set non search gcQuery here for tTableName()
	ExtSelect("tSSLCert",VAR_LIST_tSSLCert,0);

}//void ExttSSLCertSelect(void)


void ExttSSLCertSelectRow(void)
{
	ExtSelectRow("tSSLCert",VAR_LIST_tSSLCert,uSSLCert);

}//void ExttSSLCertSelectRow(void)


void ExttSSLCertListSelect(void)
{
	char cCat[512];

	ExtListSelect("tSSLCert",VAR_LIST_tSSLCert);
	
	//Changes here must be reflected below in ExttSSLCertListFilter()
        if(!strcmp(gcFilter,"uSSLCert"))
        {
                sscanf(gcCommand,"%u",&uSSLCert);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tSSLCert.uSSLCert=%u \
						ORDER BY uSSLCert",
						uSSLCert);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uSSLCert");
        }

}//void ExttSSLCertListSelect(void)


void ExttSSLCertListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uSSLCert"))
                printf("<option>uSSLCert</option>");
        else
                printf("<option selected>uSSLCert</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttSSLCertListFilter(void)


void ExttSSLCertNavBar(void)
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

}//void ExttSSLCertNavBar(void)


void tSSLCertNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tSSLCert","uSSLCert,cDomain",0);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tSSLCertNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tSSLCertNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsApache.cgi?gcFunction=tSSLCert"
				"&uSSLCert=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tSSLCertNavList(void)


