/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis for Unixservice.
 
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

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstJobVars(entries,x);
                        	guMode=2000;
	                        tJob(LANG_NB_CONFIRMNEW);
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstJobVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uJob=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtJob(0);
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstJobVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy) && uJob)
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
			if(uAllowDel(uOwner,uCreatedBy) && uJob)
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
			if(uAllowMod(uOwner,uCreatedBy) && uJob)
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
			if(uAllowMod(uOwner,uCreatedBy) && uJob)
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtJob();
			}
			else
				tJob("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttJobCommands(pentry entries[], int x)


void ExttJobButtons(void)
{
	OpenFieldSet("tJob Aux Panel",100);
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
			tJobNavList();
	}
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

	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tJob ORDER BY"
				" uJob",
				VAR_LIST_tJob);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tJob,tClient WHERE tJob.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uJob",
					VAR_LIST_tJob,uContactParentCompany,uContactParentCompany);
					

}//void ExttJobSelect(void)


void ExttJobSelectRow(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tJob WHERE uJob=%u",
			VAR_LIST_tJob,uJob);
	else
                sprintf(gcQuery,"SELECT %s FROM tJob,tClient"
                                " WHERE tJob.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tJob.uJob=%u",
                        		VAR_LIST_tJob
					,uContactParentCompany,uContactParentCompany
					,uJob);

}//void ExttJobSelectRow(void)


void ExttJobListSelect(void)
{
	char cCat[512];
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tJob",
				VAR_LIST_tJob);
	else
		sprintf(gcQuery,"SELECT %s FROM tJob,tClient"
				" WHERE tJob.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tJob
				,uContactParentCompany
				,uContactParentCompany);

	//Changes here must be reflected below in ExttJobListFilter()
        if(!strcmp(gcFilter,"uJob"))
        {
                sscanf(gcCommand,"%u",&uJob);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tJob.uJob=%u"
						" ORDER BY uJob",
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
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
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

	if(uAllowMod(uOwner,uCreatedBy) && !guListMode && uJob)
		printf(LANG_NBB_MODIFY);

	if(uAllowDel(uOwner,uCreatedBy) && !guListMode && uJob) 
		printf(LANG_NBB_DELETE);

	printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttJobNavBar(void)


void tJobNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	//Show jobs pending somehow. 3 done ok, 7 canceled.
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uJob,cLabel FROM tJob WHERE uJobStatus!=3 AND uJobStatus!=7 ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tJob.uJob,"
				" tJob.cLabel"
				" FROM tJob,tClient"
				" WHERE tJob.uOwner=tClient.uClient"
				" AND tJob.uJobStatus!=3 AND tJob.uJobStatus!=7"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
					guCompany,guLoginClient);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tJobNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tJobNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsRAD.cgi?gcFunction=tJob&uJob=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tJobNavList(void)


