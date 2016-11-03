/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
 
*/

//ModuleFunctionProtos()


void tMonthLogNavList(void);

void ExtProcesstMonthLogVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstMonthLogVars(pentry entries[], int x)


void ExttMonthLogCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tMonthLogTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=20)
			{
	                        ProcesstMonthLogVars(entries,x);
                        	guMode=2000;
	                        tMonthLog(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=20)
			{
                        	ProcesstMonthLogVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uMonthLog=0;
				uCreatedBy=guLoginClient;
				uOwner=guLoginClient;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtMonthLog(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstMonthLogVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=20 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
	                        guMode=2001;
				tMonthLog(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstMonthLogVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=20 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=5;
				DeletetMonthLog();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstMonthLogVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=20 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2002;
				tMonthLog(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstMonthLogVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=20 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtMonthLog();
			}
                }
	}

}//void ExttMonthLogCommands(pentry entries[], int x)


void ExttMonthLogButtons(void)
{
	OpenFieldSet("tMonthLog Aux Panel",100);
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
			tMonthLogNavList();
	}
	CloseFieldSet();

}//void ExttMonthLogButtons(void)


void ExttMonthLogAuxTable(void)
{

}//void ExttMonthLogAuxTable(void)


void ExttMonthLogGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uMonthLog"))
		{
			sscanf(gentries[i].val,"%u",&uMonthLog);
			guMode=6;
		}
	}
	tMonthLog("");

}//void ExttMonthLogGetHook(entry gentries[], int x)


void ExttMonthLogSelect(void)
{
        //Set non search gcQuery here for tTableName()
	if(guPermLevel>=9)
	sprintf(gcQuery,"SELECT %s FROM tMonthLog ORDER BY\
					uMonthLog",
					VAR_LIST_tMonthLog);
	else
	sprintf(gcQuery,"SELECT %s FROM tMonthLog WHERE uOwner=%u ORDER BY\
					uMonthLog",
					VAR_LIST_tMonthLog,guLoginClient);

}//void ExttMonthLogSelect(void)


void ExttMonthLogSelectRow(void)
{
	if(guPermLevel<10)
                sprintf(gcQuery,"SELECT %s FROM tMonthLog,tClient \
                                WHERE tMonthLog.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)\
                                AND tMonthLog.uMonthLog=%u",
                        		VAR_LIST_tMonthLog,
					guLoginClient,guLoginClient,uMonthLog);
	else
                sprintf(gcQuery,"SELECT %s FROM tMonthLog WHERE uMonthLog=%u",
			VAR_LIST_tMonthLog,uMonthLog);

}//void ExttMonthLogSelectRow(void)


void ExttMonthLogListSelect(void)
{
	char cCat[512];

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT %s FROM tMonthLog,tClient \
		WHERE tMonthLog.uOwner=tClient.uClient \
		AND (tClient.uOwner=%u OR tClient.uClient=%u)",
				VAR_LIST_tMonthLog,
				guLoginClient,
				guLoginClient);
	else
                sprintf(gcQuery,"SELECT %s FROM tMonthLog",
				VAR_LIST_tMonthLog);

	//Changes here must be reflected below in ExttMonthLogListFilter()
        if(!strcmp(gcFilter,"uMonthLog"))
        {
                sscanf(gcCommand,"%u",&uMonthLog);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tMonthLog.uMonthLog=%u \
						ORDER BY uMonthLog",
						uMonthLog);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uMonthLog");
        }

}//void ExttMonthLogListSelect(void)


void ExttMonthLogListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uMonthLog"))
                printf("<option>uMonthLog</option>");
        else
                printf("<option selected>uMonthLog</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttMonthLogListFilter(void)


void ExttMonthLogNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=20 && !guListMode)
		printf(LANG_NBB_NEW);

			if( (guPermLevel>=20 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_MODIFY);

			if( (guPermLevel>=20 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttMonthLogNavBar(void)


void tMonthLogNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT tMonthLog.uMonthLog\
				,tMonthLog.cLabel\
				FROM tMonthLog,tClient\
                                WHERE tMonthLog.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)",
					guLoginClient,guLoginClient);
	else
	        sprintf(gcQuery,"SELECT uMonthLog,cLabel FROM tMonthLog");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tMonthLogNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tMonthLogNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
printf("<a class=darkLink href=unxsRadacct.cgi?gcFunction=tMonthLog\
&uMonthLog=%s>%s</a><br>\n",field[0],field[1]);
	        }
	}
        mysql_free_result(res);

}//void tMonthLogNavList(void)


