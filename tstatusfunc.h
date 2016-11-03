/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR/LEGAL
	(C) 2001-2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file.
*/

//ModuleFunctionProtos()


void tStatusNavList(void);

void ExtProcesstStatusVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstStatusVars(pentry entries[], int x)


void ExttStatusCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tStatusTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstStatusVars(entries,x);
                        	guMode=2000;
	                        tStatus(LANG_NB_CONFIRMNEW);
			}
			else
				tStatus("Operation denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstStatusVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uStatus=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtStatus(0);
			}
			else
				tStatus("Operation denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstStatusVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1)
			{
	                        guMode=2001;
				tStatus(LANG_NB_CONFIRMDEL);
			}
			else
				tStatus("Operation denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstStatusVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1)
			{
				guMode=5;
				DeletetStatus();
			}
			else
				tStatus("Operation denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstStatusVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tStatus(LANG_NB_CONFIRMMOD);
			}
			else
				tStatus("Operation denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstStatusVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtStatus();
			}
			else
				tStatus("Operation denied by permissions settings");
                }
	}

}//void ExttStatusCommands(pentry entries[], int x)


void ExttStatusButtons(void)
{
	OpenFieldSet("tStatus Aux Panel",100);
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
			tStatusNavList();
	}
	CloseFieldSet();

}//void ExttStatusButtons(void)


void ExttStatusAuxTable(void)
{

}//void ExttStatusAuxTable(void)


void ExttStatusGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uStatus"))
		{
			sscanf(gentries[i].val,"%u",&uStatus);
			guMode=6;
		}
	}
	tStatus("");

}//void ExttStatusGetHook(entry gentries[], int x)


void ExttStatusSelect(void)
{
        //Set non search gcQuery here for tTableName()
	ExtSelectPublic("tStatus",VAR_LIST_tStatus);

}//void ExttStatusSelect(void)


void ExttStatusSelectRow(void)
{
	ExtSelectRowPublic("tStatus",VAR_LIST_tStatus,uStatus);

}//void ExttStatusSelectRow(void)


void ExttStatusListSelect(void)
{
	char cCat[512];

	ExtListSelectPublic("tStatus",VAR_LIST_tStatus);
	
	//Changes here must be reflected below in ExttStatusListFilter()
        if(!strcmp(gcFilter,"uStatus"))
        {
                sscanf(gcCommand,"%u",&uStatus);
		sprintf(cCat," WHERE tStatus.uStatus=%u ORDER BY uStatus",uStatus);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uStatus");
        }

}//void ExttStatusListSelect(void)


void ExttStatusListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uStatus"))
                printf("<option>uStatus</option>");
        else
                printf("<option selected>uStatus</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttStatusListFilter(void)


void ExttStatusNavBar(void)
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

}//void ExttStatusNavBar(void)


void tStatusNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelectPublic("tStatus","tStatus.uStatus,tStatus.cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tStatusNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tStatusNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tStatus&uStatus=%s>"
				"%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tStatusNavList(void)
