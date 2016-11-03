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


void tContentTypeNavList(void);

void ExtProcesstContentTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstContentTypeVars(pentry entries[], int x)


void ExttContentTypeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tContentTypeTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=7)
			{
	                        ProcesstContentTypeVars(entries,x);
                        	guMode=2000;
	                        tContentType(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=7)
			{
                        	ProcesstContentTypeVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uContentType=0;
				uCreatedBy=guLoginClient;
				uOwner=guLoginClient;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtContentType(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstContentTypeVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=7 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
	                        guMode=2001;
				tContentType(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstContentTypeVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=7 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=5;
				DeletetContentType();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstContentTypeVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=7 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2002;
				tContentType(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstContentTypeVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=7 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtContentType();
			}
                }
	}

}//void ExttContentTypeCommands(pentry entries[], int x)


void ExttContentTypeButtons(void)
{
	OpenFieldSet("tContentType Aux Panel",100);
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
			tContentTypeNavList();
	}
	CloseFieldSet();

}//void ExttContentTypeButtons(void)


void ExttContentTypeAuxTable(void)
{

}//void ExttContentTypeAuxTable(void)


void ExttContentTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uContentType"))
		{
			sscanf(gentries[i].val,"%u",&uContentType);
			guMode=6;
		}
	}
	tContentType("");

}//void ExttContentTypeGetHook(entry gentries[], int x)


void ExttContentTypeSelect(void)
{
        //Set non search gcQuery here for tTableName()
	if(guPermLevel>=9)
	sprintf(gcQuery,"SELECT %s FROM tContentType ORDER BY\
					uContentType",
					VAR_LIST_tContentType);
	else
	sprintf(gcQuery,"SELECT %s FROM tContentType WHERE uOwner=%u ORDER BY\
					uContentType",
					VAR_LIST_tContentType,guLoginClient);

}//void ExttContentTypeSelect(void)


void ExttContentTypeSelectRow(void)
{
	if(guPermLevel<10)
                sprintf(gcQuery,"SELECT %s FROM tContentType,tClient \
                                WHERE tContentType.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)\
                                AND tContentType.uContentType=%u",
                        		VAR_LIST_tContentType,
					guLoginClient,guLoginClient,uContentType);
	else
                sprintf(gcQuery,"SELECT %s FROM tContentType WHERE uContentType=%u",
			VAR_LIST_tContentType,uContentType);

}//void ExttContentTypeSelectRow(void)


void ExttContentTypeListSelect(void)
{
	char cCat[512];

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT %s FROM tContentType,tClient \
		WHERE tContentType.uOwner=tClient.uClient \
		AND (tClient.uOwner=%u OR tClient.uClient=%u)",
				VAR_LIST_tContentType,
				guLoginClient,
				guLoginClient);
	else
                sprintf(gcQuery,"SELECT %s FROM tContentType",
				VAR_LIST_tContentType);

	//Changes here must be reflected below in ExttContentTypeListFilter()
        if(!strcmp(gcFilter,"uContentType"))
        {
                sscanf(gcCommand,"%u",&uContentType);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tContentType.uContentType=%u \
						ORDER BY uContentType",
						uContentType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uContentType");
        }

}//void ExttContentTypeListSelect(void)


void ExttContentTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uContentType"))
                printf("<option>uContentType</option>");
        else
                printf("<option selected>uContentType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttContentTypeListFilter(void)


void ExttContentTypeNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

			if( (guPermLevel>=7 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_MODIFY);

			if( (guPermLevel>=7 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttContentTypeNavBar(void)


void tContentTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT tContentType.uContentType\
				,tContentType.cLabel\
				FROM tContentType,tClient\
                                WHERE tContentType.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)",
					guLoginClient,guLoginClient);
	else
	        sprintf(gcQuery,"SELECT uContentType,cLabel FROM tContentType");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tContentTypeNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tContentTypeNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
printf("<a class=darkLink href=unxsBlog.cgi?gcFunction=tContentType\
&uContentType=%s>%s</a><br>\n",field[0],field[1]);
	        }
	}
        mysql_free_result(res);

}//void tContentTypeNavList(void)


