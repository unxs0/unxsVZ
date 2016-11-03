/*
FILE
	svn ID removed
PURPOSE
	Non-schema dependent tconfiguration.c expansion.
AUTHOR
	GPL License applies, see www.fsf.org for details
	See LICENSE file in this distribution
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
 
*/

void tConfigurationNavList(void);

void ExtProcesstConfigurationVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstConfigurationVars(pentry entries[], int x)


void ExttConfigurationCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tConfigurationTools"))
	{
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstConfigurationVars(entries,x);
                        	guMode=2000;
	                        tConfiguration(LANG_NB_CONFIRMNEW);
			}
			else
				tConfiguration("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstConfigurationVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uConfiguration=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtConfiguration(0);
			}
			else
				tConfiguration("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
			ProcesstConfigurationVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tConfiguration("Make sure this is the record you would like to delete");
			}
			else
				tConfiguration("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstConfigurationVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetConfiguration();
			}
			else
				tConfiguration("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstConfigurationVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tConfiguration(LANG_NB_CONFIRMMOD);
			}
			else
				tConfiguration("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstConfigurationVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtConfiguration();
			}
			else
				tConfiguration("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttConfigurationCommands(pentry entries[], int x)


void ExttConfigurationButtons(void)
{

	OpenFieldSet("Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			tConfigurationNavList();

	}
	CloseFieldSet();



}//void ExttConfigurationButtons(void)


void ExttConfigurationAuxTable(void)
{

}//void ExttConfigurationAuxTable(void)


void ExttConfigurationGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uConfiguration"))
		{
			sscanf(gentries[i].val,"%u",&uConfiguration);
			guMode=6;
		}
	}
	tConfiguration("");

}//void ExttConfigurationGetHook(entry gentries[], int x)


void ExttConfigurationSelect(void)
{
	ExtSelect("tConfiguration",VAR_LIST_tConfiguration,0);

}//void ExttConfigurationSelect(void)


void ExttConfigurationSelectRow(void)
{
	ExtSelectRow("tConfiguration",VAR_LIST_tConfiguration,uConfiguration);

}//void ExttConfigurationSelectRow(void)


void ExttConfigurationListSelect(void)
{
	char cCat[512];

	ExtListSelect("tConfiguration",VAR_LIST_tConfiguration);

	//Changes here must be reflected below in ExttConfigurationListFilter()
        if(!strcmp(gcFilter,"uConfiguration"))
        {
                sscanf(gcCommand,"%u",&uConfiguration);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tConfiguration.uConfiguration=%u \
						ORDER BY uConfiguration",
						uConfiguration);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uConfiguration");
        }

}//void ExttConfigurationListSelect(void)


void ExttConfigurationListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp; Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uConfiguration"))
                printf("<option>uConfiguration</option>");
        else
                printf("<option selected>uConfiguration</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttConfigurationListFilter(void)


void ExttConfigurationNavBar(void)
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

}//void ExttConfigurationNavBar(void)


void tConfigurationNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tConfigurationNavList</u><br>\n");
	
	ExtSelect("tConfiguration","tConfiguration.uConfiguration,tConfiguration.cLabel",0);

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
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tConfiguration&uConfiguration=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tConfigurationNavList(void)


// vim:tw=78
//perlSAR patch1
