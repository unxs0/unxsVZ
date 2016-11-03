/*
FILE
	svn ID removed
PURPOSE
AUTHOR/LEGAL
	(C) 2001-2016 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file.
*/

//ModuleFunctionProtos()


void tFWStatusNavList(void);

void ExtProcesstFWStatusVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstFWStatusVars(pentry entries[], int x)


void ExttFWStatusCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tFWStatusTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstFWStatusVars(entries,x);
                        	guMode=2000;
	                        tFWStatus(LANG_NB_CONFIRMNEW);
			}
			else
				tFWStatus("Operation denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstFWStatusVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uFWStatus=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtFWStatus(0);
			}
			else
				tFWStatus("Operation denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstFWStatusVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1)
			{
	                        guMode=2001;
				tFWStatus(LANG_NB_CONFIRMDEL);
			}
			else
				tFWStatus("Operation denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstFWStatusVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1)
			{
				guMode=5;
				DeletetFWStatus();
			}
			else
				tFWStatus("Operation denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstFWStatusVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tFWStatus(LANG_NB_CONFIRMMOD);
			}
			else
				tFWStatus("Operation denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstFWStatusVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtFWStatus();
			}
			else
				tFWStatus("Operation denied by permissions settings");
                }
	}

}//void ExttFWStatusCommands(pentry entries[], int x)


void ExttFWStatusButtons(void)
{
	OpenFieldSet("tFWStatus Aux Panel",100);
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
			tFWStatusNavList();
	}
	CloseFieldSet();

}//void ExttFWStatusButtons(void)


void ExttFWStatusAuxTable(void)
{

}//void ExttFWStatusAuxTable(void)


void ExttFWStatusGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uFWStatus"))
		{
			sscanf(gentries[i].val,"%u",&uFWStatus);
			guMode=6;
		}
	}
	tFWStatus("");

}//void ExttFWStatusGetHook(entry gentries[], int x)


void ExttFWStatusSelect(void)
{
        //Set non search gcQuery here for tTableName()
	ExtSelectPublic("tFWStatus",VAR_LIST_tFWStatus);

}//void ExttFWStatusSelect(void)


void ExttFWStatusSelectRow(void)
{
	ExtSelectRowPublic("tFWStatus",VAR_LIST_tFWStatus,uFWStatus);

}//void ExttFWStatusSelectRow(void)


void ExttFWStatusListSelect(void)
{
	char cCat[512];

	ExtListSelectPublic("tFWStatus",VAR_LIST_tFWStatus);
	
	//Changes here must be reflected below in ExttFWStatusListFilter()
        if(!strcmp(gcFilter,"uFWStatus"))
        {
                sscanf(gcCommand,"%u",&uFWStatus);
		sprintf(cCat," WHERE tFWStatus.uFWStatus=%u ORDER BY uFWStatus",uFWStatus);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uFWStatus");
        }

}//void ExttFWStatusListSelect(void)


void ExttFWStatusListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uFWStatus"))
                printf("<option>uFWStatus</option>");
        else
                printf("<option selected>uFWStatus</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttFWStatusListFilter(void)


void ExttFWStatusNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=12 && !guListMode)
		printf(LANG_NBB_NEW);

	if(uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);

	if(guPermLevel>=12 && guLoginClient==1)
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttFWStatusNavBar(void)


void tFWStatusNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelectPublic("tFWStatus","tFWStatus.uFWStatus,tFWStatus.cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tFWStatusNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tFWStatusNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tFWStatus&uFWStatus=%s>"
				"%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tFWStatusNavList(void)
