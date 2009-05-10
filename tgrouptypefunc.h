/*
FILE
	$Id: tgrouptypefunc.h 2468 2009-02-17 22:43:51Z Gary $
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2007 Gary Wallis.
 
*/

//ModuleFunctionProtos()


void tGroupTypeNavList(void);

void ExtProcesstGroupTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstGroupTypeVars(pentry entries[], int x)


void ExttGroupTypeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tGroupTypeTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstGroupTypeVars(entries,x);
                        	guMode=2000;
	                        tGroupType(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstGroupTypeVars(entries,x);

                        	guMode=2000;
				//Check entries
                        	guMode=0;

				uGroupType=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtGroupType(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstGroupTypeVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
	                        guMode=2001;
				tGroupType(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstGroupTypeVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=5;
				DeletetGroupType();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstGroupTypeVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2002;
				tGroupType(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstGroupTypeVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtGroupType();
			}
                }
	}

}//void ExttGroupTypeCommands(pentry entries[], int x)


void ExttGroupTypeButtons(void)
{
	OpenFieldSet("tGroupType Aux Panel",100);
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
			printf("tGroupType (and it's related tables <a href=?gcFunction=tGroupGlue>"
				"tGroupGlue</a> and <a href=?gcFunction=tGroup>tGroup</a>)"
				" provide a way to create"
				" container and node groups of different types."
				" These groups can then be used for organizational and/or autonomic purposes.");
			tGroupTypeNavList();
	}
	CloseFieldSet();

}//void ExttGroupTypeButtons(void)


void ExttGroupTypeAuxTable(void)
{

}//void ExttGroupTypeAuxTable(void)


void ExttGroupTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uGroupType"))
		{
			sscanf(gentries[i].val,"%u",&uGroupType);
			guMode=6;
		}
	}
	tGroupType("");

}//void ExttGroupTypeGetHook(entry gentries[], int x)


void ExttGroupTypeSelect(void)
{
	ExtSelect("tGroupType",VAR_LIST_tGroupType);

}//void ExttGroupTypeSelect(void)


void ExttGroupTypeSelectRow(void)
{
	ExtSelectRow("tGroupType",VAR_LIST_tGroupType,uGroupType);

}//void ExttGroupTypeSelectRow(void)


void ExttGroupTypeListSelect(void)
{
	char cCat[512];

	ExtListSelect("tGroupType",VAR_LIST_tGroupType);
	
	//Changes here must be reflected below in ExttGroupTypeListFilter()
        if(!strcmp(gcFilter,"uGroupType"))
        {
                sscanf(gcCommand,"%u",&uGroupType);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tGroupType.uGroupType=%u \
						ORDER BY uGroupType",
						uGroupType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uGroupType");
        }

}//void ExttGroupTypeListSelect(void)


void ExttGroupTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uGroupType"))
                printf("<option>uGroupType</option>");
        else
                printf("<option selected>uGroupType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttGroupTypeListFilter(void)


void ExttGroupTypeNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10 && !guListMode)
		printf(LANG_NBB_NEW);

			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_MODIFY);

			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttGroupTypeNavBar(void)


void tGroupTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tGroupType","tGroupType.uGroupType,tGroupType.cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tGroupTypeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tGroupTypeNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tGroupType\
&uGroupType=%s>%s</a><br>\n",field[0],field[1]);
	        }
	}
        mysql_free_result(res);

}//void tGroupTypeNavList(void)


