/*
FILE
	svn ID removed
PURPOSE
	tConfig table left panel web gui extensions to tconfig.c

AUTHOR
	Template and mysqlRAD2 author: (c) 2001,2002 Gary Wallis.
	GPL License applies, see www.fsf.org for details
	See included LICENSE
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.

 
*/


void ExtProcesstConfigVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstConfigVars(pentry entries[], int x)


void ExttConfigCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tConfigTools"))
	{
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstConfigVars(entries,x);
                        	guMode=2000;
	                        tConfig(LANG_NB_CONFIRMNEW);
			}
			else
				tConfig("<blink>Error</blink>: Denied by permissions settings");  
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstConfigVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uConfig=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtConfig(0);
			}
			else
				tConfig("<blink>Error</blink>: Denied by permissions settings");  
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstConfigVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
	                        guMode=2001;
				tConfig(LANG_NB_CONFIRMDEL);
			}
			else
				tConfig("<blink>Error</blink>: Denied by permissions settings");  
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstConfigVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
				guMode=5;
				DeletetConfig();
			}
			else
				tConfig("<blink>Error</blink>: Denied by permissions settings");  
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
				guMode=2002;
				tConfig(LANG_NB_CONFIRMMOD);
			}
			else
				tConfig("<blink>Error</blink>: Denied by permissions settings");  
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        
			ProcesstConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtConfig();
			}
			else
				tConfig("<blink>Error</blink>: Denied by permissions settings");  
                }
	}

}//void ExttConfigCommands(pentry entries[], int x)


void ExttConfigButtons(void)
{
	OpenFieldSet("tConfig Aux Panel",100);
	
	if(uGroup)
		printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tService&uService=%u>[Return]</a>",uGroup);

	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			//Quick link to parameter
			if(uParameter)
        			printf("<p><a class=darkLink href=unxsISP.cgi?gcFunction=tParameter&uParameter=%u>%s</tr>\n",
					uParameter,ForeignKey("tParameter","cParameter",uParameter));


	}

	CloseFieldSet();

}//void ExttConfigButtons(void)


void ExttConfigAuxTable(void)
{

}//void ExttConfigAuxTable(void)


void ExttConfigGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uConfig"))
		{
			sscanf(gentries[i].val,"%u",&uConfig);
			guMode=6;
		}
	}
	tConfig("");

}//void ExttConfigGetHook(entry gentries[], int x)


void ExttConfigSelect(void)
{
	ExtSelect("tConfig",VAR_LIST_tConfig,0);

}//void ExttConfigSelect(void)


void ExttConfigSelectRow(void)
{
	ExtSelectRow("tConfig",VAR_LIST_tConfig,uConfig);
}//void ExttConfigSelectRow(void)


void ExttConfigListSelect(void)
{
	char cCat[512];

	ExtListSelect("tConfig",VAR_LIST_tConfig);

	//Changes here must be reflected below in ExttConfigListFilter()
        if(!strcmp(gcFilter,"uConfig"))
        {
                sscanf(gcCommand,"%u",&uConfig);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tConfig.uConfig=%u \
						ORDER BY uConfig",
						uConfig);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uConfig");
        }

}//void ExttConfigListSelect(void)


void ExttConfigListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uConfig"))
                printf("<option>uConfig</option>");
        else
                printf("<option selected>uConfig</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttConfigListFilter(void)


void ExttConfigNavBar(void)
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

}//void ExttConfigNavBar(void)

// vim:tw=78
