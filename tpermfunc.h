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


void tPermNavList(void);

void ExtProcesstPermVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstPermVars(pentry entries[], int x)


void ExttPermCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tPermTools"))
	{
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstPermVars(entries,x);
                        	guMode=2000;
	                        tPerm(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstPermVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uPerm=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtPerm(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstPermVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1)
			{
	                        guMode=2001;
				tPerm(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstPermVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1)
			{
				guMode=5;
				DeletetPerm();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstPermVars(entries,x);
			if(guPermLevel>=12)
			{
				guMode=2002;
				tPerm(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstPermVars(entries,x);
			if(guPermLevel>=12)
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtPerm();
			}
                }
	}

}//void ExttPermCommands(pentry entries[], int x)


void ExttPermButtons(void)
{
	OpenFieldSet("tPerm Aux Panel",100);
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
			printf("Granular control of access to tables, columns and interface named data items."
				" Linked to tAuthorize.uPerm usually.");
			printf("<p><u>Record Context Info</u><br>");
			tPermNavList();
	}
	CloseFieldSet();

}//void ExttPermButtons(void)


void ExttPermAuxTable(void)
{

}//void ExttPermAuxTable(void)


void ExttPermGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uPerm"))
		{
			sscanf(gentries[i].val,"%u",&uPerm);
			guMode=6;
		}
	}
	tPerm("");

}//void ExttPermGetHook(entry gentries[], int x)


void ExttPermSelect(void)
{
	ExtSelectPublic("tPerm",VAR_LIST_tPerm);

}//void ExttPermSelect(void)


void ExttPermSelectRow(void)
{
	ExtSelectRowPublic("tPerm",VAR_LIST_tPerm,uPerm);

}//void ExttPermSelectRow(void)


void ExttPermListSelect(void)
{
	char cCat[512];

	ExtListSelectPublic("tPerm",VAR_LIST_tPerm);
	
	//Changes here must be reflected below in ExttPermListFilter()
        if(!strcmp(gcFilter,"uPerm"))
        {
                sscanf(gcCommand,"%u",&uPerm);
		sprintf(cCat," WHERE tPerm.uPerm=%u ORDER BY uPerm",
						uPerm);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uPerm");
        }

}//void ExttPermListSelect(void)


void ExttPermListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uPerm"))
                printf("<option>uPerm</option>");
        else
                printf("<option selected>uPerm</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttPermListFilter(void)


void ExttPermNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=12 && !guListMode)
		printf(LANG_NBB_NEW);

	if(guPermLevel>=12)
		printf(LANG_NBB_MODIFY);

	if(guPermLevel>=12 && guLoginClient==1)
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttPermNavBar(void)


void tPermNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelectPublicOrder("tPerm","uPerm,cLabel","cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tPermNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tPermNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tPerm&uPerm=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tPermNavList(void)


