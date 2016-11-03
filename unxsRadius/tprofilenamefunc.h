/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	File (C) 2001-2009 Gary Wallis and Hugo Urquiza.

	GPL License applies, see www.fsf.org for details.
	LICENSE included in distribution.
*/

//ModuleFunctionProtos()


void tProfileNameNavList(void);

//Aux drop/pull downs
static char cForCompanyPullDown[256]={"---"};
static unsigned uForCompany=0;

void tTablePullDownCompanies(unsigned uSelector);//tprofilefunc.h
unsigned uProfileNameUsed(const unsigned uProfileName,const unsigned uOwner);//tprofilefunc.h

void BasictProfileNameCheck(unsigned uMod);


void ExtProcesstProfileNameVars(pentry entries[], int x)
{

	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cForCompanyPullDown"))
		{
			strcpy(cForCompanyPullDown,entries[i].val);
			uForCompany=ReadPullDown(TCLIENT,"cLabel",cForCompanyPullDown);
		}
	}

}//void ExtProcesstProfileNameVars(pentry entries[], int x)


void ExttProfileNameCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tProfileNameTools"))
	{
		//ModuleFunctionProcess()

		//Default wizard like two step creation and deletion
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			ProcesstProfileNameVars(entries,x);
			if(guPermLevel>=10)
			{
				//Check global conditions for new record here
        	                guMode=2000;
               	        	tProfileName(LANG_NB_CONFIRMNEW);
			}
			else
				tProfileName("<blink>Error</blink>: Denied by permissions settings");  
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			ProcesstProfileNameVars(entries,x);
			if(guPermLevel>=10)
			{
				//Check entries here
				BasictProfileNameCheck(0);
				uProfileName=0;
				uCreatedBy=guLoginClient;
				if(uForCompany)
					uOwner=uForCompany;
				else
					uOwner=guCompany;
				uModBy=0;
				uModDate=0;
				guMode=0;
				NewtProfileName(0);
			}
			else
				tProfileName("<blink>Error</blink>: Denied by permissions settings");  
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstProfileNameVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))  
			{
                        	guMode=2001;
                        	tProfileName(LANG_NB_CONFIRMDEL);
			}
			else
				tProfileName("<blink>Error</blink>: Denied by permissions settings");  
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstProfileNameVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))  
			{
                        	guMode=2001;
				if(uProfileNameUsed(uProfileName,uOwner))
					tProfileName("<blink>Error</blink>: Can't delete, profile is being used.");
				guMode=0;
                        	DeletetProfileName();
			}
			else
				tProfileName("<blink>Error</blink>: Denied by permissions settings");  
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstProfileNameVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
                        	guMode=2002;
                        	tProfileName(LANG_NB_CONFIRMMOD);
			}
			else
				tProfileName("<blink>Error</blink>: Denied by permissions settings");  
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstProfileNameVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))  
			{
                        	guMode=2002;
				BasictProfileNameCheck(1);
				if(uForCompany)
				{
					if(uProfileNameUsed(uProfileName,uOwner))
						tProfileName("<blink>Error</blink>: Can't change owner, profile name"
								" is being used.");
					uOwner=uForCompany;
				}
				else
					uOwner=guCompany;
                        	guMode=5;
				uModBy=guLoginClient;
                        	ModtProfileName();
			}
			else
				tProfileName("<blink>Error</blink>: Denied by permissions settings");  
                }
	}

}//void ExttProfileNameCommands(pentry entries[], int x)


void ExttProfileNameButtons(void)
{
	OpenFieldSet("tProfile Aux Panel",100);
	
	switch(guMode)
        {
                case 2000:
			printf("Enter required data<br>");
			if(guPermLevel>7)
					tTablePullDownCompanies(guCompany);
                        printf(LANG_NBB_CONFIRMNEW);
			printf("<br>\n");
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
			printf("<br>\n");
                break;

                case 2002:
			printf("Review record data<br>");
			if(guPermLevel>7)
					tTablePullDownCompanies(guCompany);
                        printf(LANG_NBB_CONFIRMMOD);
			printf("<br>\n");
                break;

	}

        printf("<p><u>tProfileNameNavList</u><br>\n");
	tProfileNameNavList();

	CloseFieldSet();
	
}//void ExttProfileNameButtons(void)


void ExttProfileNameAuxTable(void)
{

}//void ExttProfileNameAuxTable(void)


void ExttProfileNameGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uProfileName"))
		{
			sscanf(gentries[i].val,"%u",&uProfileName);
			guMode=6;
		}
	}
	tProfileName("");

}//void ExttProfileNameGetHook(entry gentries[], int x)


void ExttProfileNameSelect(void)
{
        //Set non search query here for tTableName()
	ExtSelect("tProfileName",VAR_LIST_tProfileName,0);
	
}//void ExttProfileNameSelect(void)


void ExttProfileNameSelectRow(void)
{
	ExtSelectRow("tProfileName",VAR_LIST_tProfileName,uProfileName);

}//void ExttProfileNameSelectRow(void)


void ExttProfileNameListSelect(void)
{
	char cCat[512];

	ExtListSelect("tProfileName",VAR_LIST_tProfileName);
	
	//Changes here must be reflected below in ExttProfileNameListFilter()
        if(!strcmp(gcFilter,"uProfileName"))
        {
                sscanf(gcCommand,"%u",&uProfileName);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tProfileName.uProfileName=%u \
						ORDER BY uProfileName",
						uProfileName);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uProfileName");
        }

}//void ExttProfileNameListSelect(void)


void ExttProfileNameListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uProfileName"))
                printf("<option>uProfileName</option>");
        else
                printf("<option selected>uProfileName</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttProfileNameListFilter(void)


void ExttProfileNameNavBar(void)
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

}//void ExttProfileNameNavBar(void)


void tProfileNameNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tProfileName","tProfileName.uProfileName,tProfileName.cLabel",0);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
                printf("<a class=darkLink href=unxsRadius.cgi?gcFunction=tProfileName"
			"&uProfileName=%s>%s</a><br>\n",field[0],field[1]);
        mysql_free_result(res);

}//void tProfileNameNavList(void)


void BasictProfileNameCheck(unsigned uMod)
{
	//
	//Set guMode in case we find and issue
	if(uMod)
		guMode=2002;
	else
		guMode=2000;

	if(!cLabel[0])
		tProfileName("<blink>Error: </blink>cLabel is required");
	
	if(!uMod)
	{
		MYSQL_RES *res;
		sprintf(gcQuery,"SELECT uProfileName FROM tProfileName WHERE cLabel='%s'",TextAreaSave(cLabel));
		macro_mySQLRunAndStore(res);
		if(mysql_num_rows(res))
		{
			tProfileName("<blink>Error: </blink>cLabel already in use");
		}
		mysql_free_result(res);
	}

}//void BasictProfileNameCheck(unsigned uMod)

