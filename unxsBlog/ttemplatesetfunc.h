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


void tTemplateSetNavList(void);

void ExtProcesstTemplateSetVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstTemplateSetVars(pentry entries[], int x)


void ExttTemplateSetCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tTemplateSetTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstTemplateSetVars(entries,x);
                        	guMode=2000;
	                        tTemplateSet(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstTemplateSetVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uTemplateSet=0;
				uCreatedBy=guLoginClient;
				uOwner=guLoginClient;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtTemplateSet(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstTemplateSetVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
	                        guMode=2001;
				tTemplateSet(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstTemplateSetVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=5;
				DeletetTemplateSet();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstTemplateSetVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2002;
				tTemplateSet(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstTemplateSetVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtTemplateSet();
			}
                }
	}

}//void ExttTemplateSetCommands(pentry entries[], int x)


void ExttTemplateSetButtons(void)
{
	OpenFieldSet("tTemplateSet Aux Panel",100);
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

			tTemplateSetNavList();
	}
	CloseFieldSet();

}//void ExttTemplateSetButtons(void)


void ExttTemplateSetAuxTable(void)
{

}//void ExttTemplateSetAuxTable(void)


void ExttTemplateSetGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uTemplateSet"))
		{
			sscanf(gentries[i].val,"%u",&uTemplateSet);
			guMode=6;
		}
	}
	tTemplateSet("");

}//void ExttTemplateSetGetHook(entry gentries[], int x)


void ExttTemplateSetSelect(void)
{
        //Set non search gcQuery here for tTableName()
	if(guPermLevel>=9)
	sprintf(gcQuery,"SELECT %s FROM tTemplateSet ORDER BY\
					uTemplateSet",
					VAR_LIST_tTemplateSet);
	else
	sprintf(gcQuery,"SELECT %s FROM tTemplateSet WHERE uOwner=%u ORDER BY\
					uTemplateSet",
					VAR_LIST_tTemplateSet,guLoginClient);

}//void ExttTemplateSetSelect(void)


void ExttTemplateSetSelectRow(void)
{
	if(guPermLevel<10)
                sprintf(gcQuery,"SELECT %s FROM tTemplateSet,tClient \
                                WHERE tTemplateSet.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)\
                                AND tTemplateSet.uTemplateSet=%u",
                        		VAR_LIST_tTemplateSet,
					guLoginClient,guLoginClient,uTemplateSet);
	else
                sprintf(gcQuery,"SELECT %s FROM tTemplateSet WHERE uTemplateSet=%u",
			VAR_LIST_tTemplateSet,uTemplateSet);

}//void ExttTemplateSetSelectRow(void)


void ExttTemplateSetListSelect(void)
{
	char cCat[512];

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT %s FROM tTemplateSet,tClient \
		WHERE tTemplateSet.uOwner=tClient.uClient \
		AND (tClient.uOwner=%u OR tClient.uClient=%u)",
				VAR_LIST_tTemplateSet,
				guLoginClient,
				guLoginClient);
	else
                sprintf(gcQuery,"SELECT %s FROM tTemplateSet",
				VAR_LIST_tTemplateSet);

	//Changes here must be reflected below in ExttTemplateSetListFilter()
        if(!strcmp(gcFilter,"uTemplateSet"))
        {
                sscanf(gcCommand,"%u",&uTemplateSet);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tTemplateSet.uTemplateSet=%u \
						ORDER BY uTemplateSet",
						uTemplateSet);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uTemplateSet");
        }

}//void ExttTemplateSetListSelect(void)


void ExttTemplateSetListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uTemplateSet"))
                printf("<option>uTemplateSet</option>");
        else
                printf("<option selected>uTemplateSet</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttTemplateSetListFilter(void)


void ExttTemplateSetNavBar(void)
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

			if( (guPermLevel>=10 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttTemplateSetNavBar(void)


void tTemplateSetNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT tTemplateSet.uTemplateSet\
				,tTemplateSet.cLabel\
				FROM tTemplateSet,tClient\
                                WHERE tTemplateSet.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)",
					guLoginClient,guLoginClient);
	else
	        sprintf(gcQuery,"SELECT uTemplateSet,cLabel FROM tTemplateSet");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tTemplateSetNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tTemplateSetNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
printf("<a class=darkLink href=unxsBlog.cgi?gcFunction=tTemplateSet&uTemplateSet=%s>%s</a><br>\n",field[0],field[1]);
	        }
	}
        mysql_free_result(res);

}//void tTemplateSetNavList(void)

