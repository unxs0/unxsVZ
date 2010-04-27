/*
FILE
	$Id$
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR/LEGAL
	(C) 2001-2010 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file.
*/

//ModuleFunctionProtos()


void tTemplateTypeNavList(void);

void ExtProcesstTemplateTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstTemplateTypeVars(pentry entries[], int x)


void ExttTemplateTypeCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tTemplateTypeTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstTemplateTypeVars(entries,x);
                        	guMode=2000;
	                        tTemplateType(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstTemplateTypeVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uTemplateType=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtTemplateType(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstTemplateTypeVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1)
			{
	                        guMode=2001;
				tTemplateType(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstTemplateTypeVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1)
			{
				guMode=5;
				DeletetTemplateType();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstTemplateTypeVars(entries,x);
			if(guPermLevel>=12)
			{
				guMode=2002;
				tTemplateType(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstTemplateTypeVars(entries,x);
			if(guPermLevel>=12)
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtTemplateType();
			}
                }
	}

}//void ExttTemplateTypeCommands(pentry entries[], int x)


void ExttTemplateTypeButtons(void)
{
	OpenFieldSet("tTemplateType Aux Panel",100);
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

			tTemplateTypeNavList();
	}
	CloseFieldSet();

}//void ExttTemplateTypeButtons(void)


void ExttTemplateTypeAuxTable(void)
{

}//void ExttTemplateTypeAuxTable(void)


void ExttTemplateTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uTemplateType"))
		{
			sscanf(gentries[i].val,"%u",&uTemplateType);
			guMode=6;
		}
	}
	tTemplateType("");

}//void ExttTemplateTypeGetHook(entry gentries[], int x)


void ExttTemplateTypeSelect(void)
{
	ExtSelectPublic("tTemplateType",VAR_LIST_tTemplateType);

}//void ExttTemplateTypeSelect(void)


void ExttTemplateTypeSelectRow(void)
{
	ExtSelectRowPublic("tTemplateType",VAR_LIST_tTemplateType,uTemplateType);

}//void ExttTemplateTypeSelectRow(void)


void ExttTemplateTypeListSelect(void)
{
	char cCat[512];

	ExtListSelectPublic("tTemplateType",VAR_LIST_tTemplateType);
	
	//Changes here must be reflected below in ExttTemplateTypeListFilter()
        if(!strcmp(gcFilter,"uTemplateType"))
        {
                sscanf(gcCommand,"%u",&uTemplateType);
		sprintf(cCat," WHERE tTemplateType.uTemplateType=%u ORDER BY uTemplateType",
						uTemplateType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uTemplateType");
        }

}//void ExttTemplateTypeListSelect(void)


void ExttTemplateTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uTemplateType"))
                printf("<option>uTemplateType</option>");
        else
                printf("<option selected>uTemplateType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttTemplateTypeListFilter(void)


void ExttTemplateTypeNavBar(void)
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

}//void ExttTemplateTypeNavBar(void)


void tTemplateTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelectPublic("tTemplateType","tTemplateType.uTemplateType,tTemplateType.cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tTemplateTypeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tTemplateTypeNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tTemplateType&uTemplateType=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tTemplateTypeNavList(void)
