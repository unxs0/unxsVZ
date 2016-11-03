/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author:(C) 2001-2009 Gary Wallis and Hugo Urquiza 
	GPL License applies, see www.fsf.org for details

 
*/

//ModuleFunctionProtos()


void tJobNavList(void);

void ExtProcesstJobVars(pentry entries[], int x)
{

	/*
	register int i;
	
	for(i=0;i<x;i++)
	{
	
	}
	*/

}//void ExtProcesstJobVars(pentry entries[], int x)


void ExttJobCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tJobTools"))
	{
		//ModuleFunctionProcess()

		//Default wizard like two step creation and deletion
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstJobVars(entries,x);
				//Check global conditions for new record here
        	                guMode=2000;
                	        tJob(LANG_NB_CONFIRMNEW);
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");  
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstJobVars(entries,x);
				//Check entries here
				uJob=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				NewtJob(0);
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");  
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstJobVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))  
			{
       	                	guMode=2001;
                        	tJob(LANG_NB_CONFIRMDEL);
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");  
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstJobVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))  
			{
                        	guMode=5;
                        	DeletetJob();
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");  
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstJobVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
                        	guMode=2002;
                        	tJob(LANG_NB_CONFIRMMOD);
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");  
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstJobVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
				uModBy=guLoginClient;
                        	ModtJob();
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");  
                }
                else if(!strcmp(gcCommand,"Purge"))
                {
			if(guPermLevel>9)
			{
				sprintf(gcQuery,"DELETE FROM tJob WHERE uOwner=%u",guCompany);
        			mysql_query(&gMysql,gcQuery);
        			if(mysql_errno(&gMysql))
                        		htmlPlainTextError(mysql_error(&gMysql));
	                        tJob("Job queue emptied");
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");  
		}
	}

}//void ExttJobCommands(pentry entries[], int x)


void ExttJobButtons(void)
{
	OpenFieldSet("tJob Aux Panel",100);
	
	if(guPermLevel>9)
		printf("<input class=largeButton title='Clear all job queue entries you control'"
				" type=submit name=gcCommand value='Purge'><br>\n");

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

	tJobNavList();
	
	CloseFieldSet();

}//void ExttJobButtons(void)


void ExttJobAuxTable(void)
{

}//void ExttJobAuxTable(void)


void ExttJobGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uJob"))
		{
			sscanf(gentries[i].val,"%u",&uJob);
			guMode=6;
		}
	}
	tJob("");

}//void ExttJobGetHook(entry gentries[], int x)


void ExttJobSelect(void)
{
	ExtSelect("tJob",VAR_LIST_tJob,0);

}//void ExttJobSelect(void)


void ExttJobSelectRow(void)
{
	ExtSelectRow("tJob",VAR_LIST_tJob,uJob);

}//void ExttJobSelectRow(void)


void ExttJobListSelect(void)
{
	char cCat[512];

	ExtListSelect("tJob",VAR_LIST_tJob);

	//Changes here must be reflected below in ExttJobListFilter()
        if(!strcmp(gcFilter,"uJob"))
        {
                sscanf(gcCommand,"%u",&uJob);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tJob.uJob=%u \
						ORDER BY uJob",
						uJob);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uJob");
        }

}//void ExttJobListSelect(void)


void ExttJobListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uJob"))
                printf("<option>uJob</option>");
        else
                printf("<option selected>uJob</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttJobListFilter(void)


void ExttJobNavBar(void)
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

}//void ExttJobNavBar(void)


void tJobNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tJob","tJob.uJob,tJob.cServer,tJob.cJobName,tJob.cJobData",0);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
	        printf("<p><u>tJobNavList</u><br>\n");
	        while((field=mysql_fetch_row(res)))
	                printf("<a class=darkLink href=unxsRadius.cgi?gcFunction=tJob"
				"&uJob=%s>%s.%s:%s</a><br>\n",field[0],field[1],field[2],field[3]);
	}
        mysql_free_result(res);

}//void tJobNavList(void)


