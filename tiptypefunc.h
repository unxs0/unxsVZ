/*
FILE
	tiptypefunc.h
PURPOSE
	tIpType aux functions
AUTHOR
	(C) 2001-2017 Gary Wallis for Unixservice, LLC.
*/

//ModuleFunctionProtos()


void tIPTypeNavList(void);

void ExtProcesstIPTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstIPTypeVars(pentry entries[], int x)


void ExttIPTypeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tIPTypeTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstIPTypeVars(entries,x);
                        	guMode=2000;
	                        tIPType(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstIPTypeVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uIPType=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtIPType(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstIPTypeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tIPType(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstIPTypeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetIPType();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstIPTypeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tIPType(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstIPTypeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtIPType();
			}
                }
	}

}//void ExttIPTypeCommands(pentry entries[], int x)


void ExttIPTypeButtons(void)
{
	OpenFieldSet("tIPType Aux Panel",100);
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

			tIPTypeNavList();
	}
	CloseFieldSet();

}//void ExttIPTypeButtons(void)


void ExttIPTypeAuxTable(void)
{

}//void ExttIPTypeAuxTable(void)


void ExttIPTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uIPType"))
		{
			sscanf(gentries[i].val,"%u",&uIPType);
			guMode=6;
		}
	}
	tIPType("");

}//void ExttIPTypeGetHook(entry gentries[], int x)


void ExttIPTypeSelect(void)
{
	ExtSelect("tIPType",VAR_LIST_tIPType);

}//void ExttIPTypeSelect(void)


void ExttIPTypeSelectRow(void)
{
	ExtSelectRow("tIPType",VAR_LIST_tIPType,uIPType);

}//void ExttIPTypeSelectRow(void)


void ExttIPTypeListSelect(void)
{
	char cCat[512];

	ExtListSelect("tIPType",VAR_LIST_tIPType);

	//Changes here must be reflected below in ExttIPTypeListFilter()
        if(!strcmp(gcFilter,"uIPType"))
        {
                sscanf(gcCommand,"%u",&uIPType);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tIPType.uIPType=%u \
						ORDER BY uIPType",
						uIPType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uIPType");
        }

}//void ExttIPTypeListSelect(void)


void ExttIPTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uIPType"))
                printf("<option>uIPType</option>");
        else
                printf("<option selected>uIPType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttIPTypeListFilter(void)


void ExttIPTypeNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

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

}//void ExttIPTypeNavBar(void)


void tIPTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tIPType","tIPType.uIPType,tIPType.cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tIPTypeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tIPTypeNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tIPType&uIPType=%s>%s</a><br>",
					field[0],field[1]);
	}
        mysql_free_result(res);

}//void tIPTypeNavList(void)


