/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2007 Gary Wallis.
 
*/

//ModuleFunctionProtos()


void tMonthNavList(void);

void ExtProcesstMonthVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstMonthVars(pentry entries[], int x)


void ExttMonthCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tMonthTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
	                        ProcesstMonthVars(entries,x);
                        	guMode=2000;
	                        tMonth(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
                        	ProcesstMonthVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uMonth=0;
				uCreatedBy=guLoginClient;
				uOwner=guLoginClient;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtMonth(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstMonthVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
	                        guMode=2001;
				tMonth(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstMonthVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=5;
				DeletetMonth();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstMonthVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2002;
				tMonth(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstMonthVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtMonth();
			}
                }
	}

}//void ExttMonthCommands(pentry entries[], int x)


void ExttMonthButtons(void)
{
	OpenFieldSet("tMonth Aux Panel",100);
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

			tMonthNavList();
	}
	CloseFieldSet();

}//void ExttMonthButtons(void)


void ExttMonthAuxTable(void)
{

}//void ExttMonthAuxTable(void)


void ExttMonthGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uMonth"))
		{
			sscanf(gentries[i].val,"%u",&uMonth);
			guMode=6;
		}
	}
	tMonth("");

}//void ExttMonthGetHook(entry gentries[], int x)


void ExttMonthSelect(void)
{
        //Set non search gcQuery here for tTableName()
	if(guPermLevel>=9)
	sprintf(gcQuery,"SELECT %s FROM tMonth ORDER BY\
					uMonth",
					VAR_LIST_tMonth);
	else
	sprintf(gcQuery,"SELECT %s FROM tMonth WHERE uOwner=%u ORDER BY\
					uMonth",
					VAR_LIST_tMonth,guLoginClient);

}//void ExttMonthSelect(void)


void ExttMonthSelectRow(void)
{
	if(guPermLevel<10)
                sprintf(gcQuery,"SELECT %s FROM tMonth,tClient \
                                WHERE tMonth.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)\
                                AND tMonth.uMonth=%u",
                        		VAR_LIST_tMonth,
					guLoginClient,guLoginClient,uMonth);
	else
                sprintf(gcQuery,"SELECT %s FROM tMonth WHERE uMonth=%u",
			VAR_LIST_tMonth,uMonth);

}//void ExttMonthSelectRow(void)


void ExttMonthListSelect(void)
{
	char cCat[512];

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT %s FROM tMonth,tClient \
		WHERE tMonth.uOwner=tClient.uClient \
		AND (tClient.uOwner=%u OR tClient.uClient=%u)",
				VAR_LIST_tMonth,
				guLoginClient,
				guLoginClient);
	else
                sprintf(gcQuery,"SELECT %s FROM tMonth",
				VAR_LIST_tMonth);

	//Changes here must be reflected below in ExttMonthListFilter()
        if(!strcmp(gcFilter,"uMonth"))
        {
                sscanf(gcCommand,"%u",&uMonth);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tMonth.uMonth=%u \
						ORDER BY uMonth",
						uMonth);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uMonth");
        }

}//void ExttMonthListSelect(void)


void ExttMonthListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uMonth"))
                printf("<option>uMonth</option>");
        else
                printf("<option selected>uMonth</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttMonthListFilter(void)


void ExttMonthNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=12 && !guListMode)
		printf(LANG_NBB_NEW);

			if( (guPermLevel>=12 && uOwner==guLoginClient)
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

}//void ExttMonthNavBar(void)


void tMonthNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT tMonth.uMonth\
				,tMonth.cLabel\
				FROM tMonth,tClient\
                                WHERE tMonth.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)",
					guLoginClient,guLoginClient);
	else
	        sprintf(gcQuery,"SELECT uMonth,cLabel FROM tMonth");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tMonthNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tMonthNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
printf("<a class=darkLink href=unxsBlog.cgi?gcFunction=tMonth\
&uMonth=%s>%s</a><br>\n",field[0],field[1]);
	        }
	}
        mysql_free_result(res);

}//void tMonthNavList(void)


