/*
FILE
	svn ID removed
PURPOSE
AUTHOR/LEGAL
	(C) 2001-2016 Gary Wallis for Unixservice, LLC.
	GPLv2 license applies. See LICENSE file.
*/

//ModuleFunctionProtos()


void tContainerTypeNavList(void);

void ExtProcesstContainerTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstContainerTypeVars(pentry entries[], int x)


void ExttContainerTypeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tContainerTypeTools"))
	{
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstContainerTypeVars(entries,x);
                        	guMode=2000;
	                        tContainerType(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstContainerTypeVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uContainerType=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtContainerType(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstContainerTypeVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1)
			{
	                        guMode=2001;
				tContainerType(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstContainerTypeVars(entries,x);
			if(guPermLevel>=12 && guLoginClient==1)
			{
				guMode=5;
				DeletetContainerType();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstContainerTypeVars(entries,x);
			if(guPermLevel>=12)
			{
				guMode=2002;
				tContainerType(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstContainerTypeVars(entries,x);
			if(guPermLevel>=12)
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtContainerType();
			}
                }
	}

}//void ExttContainerTypeCommands(pentry entries[], int x)


void ExttContainerTypeButtons(void)
{
	OpenFieldSet("tContainerType Aux Panel",100);
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
			printf("<u>Table Tips (%s)</u><br>",cGitVersion);
			printf("These are usually fixed uContainerType value entries used internally.");
			printf("<p><u>Record Context Info</u><br>");
			tContainerTypeNavList();
	}
	CloseFieldSet();

}//void ExttContainerTypeButtons(void)


void ExttContainerTypeAuxTable(void)
{

}//void ExttContainerTypeAuxTable(void)


void ExttContainerTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uContainerType"))
		{
			sscanf(gentries[i].val,"%u",&uContainerType);
			guMode=6;
		}
	}
	tContainerType("");

}//void ExttContainerTypeGetHook(entry gentries[], int x)


void ExttContainerTypeSelect(void)
{
	ExtSelectPublic("tContainerType",VAR_LIST_tContainerType);

}//void ExttContainerTypeSelect(void)


void ExttContainerTypeSelectRow(void)
{
	ExtSelectRowPublic("tContainerType",VAR_LIST_tContainerType,uContainerType);

}//void ExttContainerTypeSelectRow(void)


void ExttContainerTypeListSelect(void)
{
	char cCat[512];

	ExtListSelectPublic("tContainerType",VAR_LIST_tContainerType);
	
	//Changes here must be reflected below in ExttContainerTypeListFilter()
        if(!strcmp(gcFilter,"uContainerType"))
        {
                sscanf(gcCommand,"%u",&uContainerType);
		sprintf(cCat," WHERE tContainerType.uContainerType=%u ORDER BY uContainerType",
						uContainerType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uContainerType");
        }

}//void ExttContainerTypeListSelect(void)


void ExttContainerTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uContainerType"))
                printf("<option>uContainerType</option>");
        else
                printf("<option selected>uContainerType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttContainerTypeListFilter(void)


void ExttContainerTypeNavBar(void)
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

}//void ExttContainerTypeNavBar(void)


void tContainerTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelectPublicOrder("tContainerType","uContainerType,cLabel","cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tContainerTypeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tContainerTypeNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tContainerType&uContainerType=%s>%s</a><br>\n",
				field[0],field[1]);
	}
        mysql_free_result(res);

}//void tContainerTypeNavList(void)


