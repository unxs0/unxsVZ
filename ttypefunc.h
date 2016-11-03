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


void tTypeNavList(void);

void ExtProcesstTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstTypeVars(pentry entries[], int x)


void ExttTypeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tTypeTools"))
	{
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstTypeVars(entries,x);
                        	guMode=2000;
	                        tType(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstTypeVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uType=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtType(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstTypeVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1)
			{
	                        guMode=2001;
				tType(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstTypeVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1)
			{
				guMode=5;
				DeletetType();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstTypeVars(entries,x);
			if(guPermLevel>=12)
			{
				guMode=2002;
				tType(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstTypeVars(entries,x);
			if(guPermLevel>=12)
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtType();
			}
                }
	}

}//void ExttTypeCommands(pentry entries[], int x)


void ExttTypeButtons(void)
{
	OpenFieldSet("tType Aux Panel",100);
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
			printf("These are usually fixed uType value entries used internally for our <i>big table</i>"
				" tProperty.");
			printf("<p><u>Record Context Info</u><br>");
			tTypeNavList();
	}
	CloseFieldSet();

}//void ExttTypeButtons(void)


void ExttTypeAuxTable(void)
{

}//void ExttTypeAuxTable(void)


void ExttTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uType"))
		{
			sscanf(gentries[i].val,"%u",&uType);
			guMode=6;
		}
	}
	tType("");

}//void ExttTypeGetHook(entry gentries[], int x)


void ExttTypeSelect(void)
{
	ExtSelectPublic("tType",VAR_LIST_tType);

}//void ExttTypeSelect(void)


void ExttTypeSelectRow(void)
{
	ExtSelectRowPublic("tType",VAR_LIST_tType,uType);

}//void ExttTypeSelectRow(void)


void ExttTypeListSelect(void)
{
	char cCat[512];

	ExtListSelectPublic("tType",VAR_LIST_tType);
	
	//Changes here must be reflected below in ExttTypeListFilter()
        if(!strcmp(gcFilter,"uType"))
        {
                sscanf(gcCommand,"%u",&uType);
		sprintf(cCat," WHERE tType.uType=%u ORDER BY uType",
						uType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uType");
        }

}//void ExttTypeListSelect(void)


void ExttTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uType"))
                printf("<option>uType</option>");
        else
                printf("<option selected>uType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttTypeListFilter(void)


void ExttTypeNavBar(void)
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

}//void ExttTypeNavBar(void)


void tTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelectPublicOrder("tType","uType,cLabel","cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tTypeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tTypeNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tType&uType=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tTypeNavList(void)


