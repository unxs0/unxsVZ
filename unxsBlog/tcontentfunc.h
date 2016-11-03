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


void tContentNavList(void);

void ExtProcesstContentVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstContentVars(pentry entries[], int x)


void ExttContentCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tContentTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=7)
			{
	                        ProcesstContentVars(entries,x);
                        	guMode=2000;
	                        tContent(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=7)
			{
                        	ProcesstContentVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uContent=0;
				uCreatedBy=guLoginClient;
				uOwner=guLoginClient;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtContent(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstContentVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=7 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
	                        guMode=2001;
				tContent(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstContentVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=7 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=5;
				DeletetContent();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstContentVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=7 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2002;
				tContent(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstContentVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=7 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1 && uOwner!=0)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtContent();
			}
                }
	}

}//void ExttContentCommands(pentry entries[], int x)


void ExttContentButtons(void)
{
	OpenFieldSet("tContent Aux Panel",100);
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
			tContentNavList();
	}
	CloseFieldSet();

}//void ExttContentButtons(void)


void ExttContentAuxTable(void)
{

}//void ExttContentAuxTable(void)


void ExttContentGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uContent"))
		{
			sscanf(gentries[i].val,"%u",&uContent);
			guMode=6;
		}
	}
	tContent("");

}//void ExttContentGetHook(entry gentries[], int x)


void ExttContentSelect(void)
{
        //Set non search gcQuery here for tTableName()
	if(guPermLevel>=9)
	sprintf(gcQuery,"SELECT %s FROM tContent ORDER BY\
					uContent",
					VAR_LIST_tContent);
	else
	sprintf(gcQuery,"SELECT %s FROM tContent WHERE uOwner=%u ORDER BY\
					uContent",
					VAR_LIST_tContent,guLoginClient);

}//void ExttContentSelect(void)


void ExttContentSelectRow(void)
{
	if(guPermLevel<10)
                sprintf(gcQuery,"SELECT %s FROM tContent,tClient \
                                WHERE tContent.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)\
                                AND tContent.uContent=%u",
                        		VAR_LIST_tContent,
					guLoginClient,guLoginClient,uContent);
	else
                sprintf(gcQuery,"SELECT %s FROM tContent WHERE uContent=%u",
			VAR_LIST_tContent,uContent);

}//void ExttContentSelectRow(void)


void ExttContentListSelect(void)
{
	char cCat[512];

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT %s FROM tContent,tClient \
		WHERE tContent.uOwner=tClient.uClient \
		AND (tClient.uOwner=%u OR tClient.uClient=%u)",
				VAR_LIST_tContent,
				guLoginClient,
				guLoginClient);
	else
                sprintf(gcQuery,"SELECT %s FROM tContent",
				VAR_LIST_tContent);

	//Changes here must be reflected below in ExttContentListFilter()
        if(!strcmp(gcFilter,"uContent"))
        {
                sscanf(gcCommand,"%u",&uContent);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tContent.uContent=%u \
						ORDER BY uContent",
						uContent);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uContent");
        }

}//void ExttContentListSelect(void)


void ExttContentListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uContent"))
                printf("<option>uContent</option>");
        else
                printf("<option selected>uContent</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttContentListFilter(void)


void ExttContentNavBar(void)
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

}//void ExttContentNavBar(void)


void tContentNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT tContent.uContent\
				,tContent.cTitle\
				FROM tContent,tClient\
                                WHERE tContent.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)",
					guLoginClient,guLoginClient);
	else
	        sprintf(gcQuery,"SELECT uContent,cTitle FROM tContent");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tContentNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tContentNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
printf("<a class=darkLink href=unxsBlog.cgi?gcFunction=tContent&uContent=%s>%s</a><br>\n",
				field[0],field[1]);
	        }
	}
        mysql_free_result(res);

}//void tContentNavList(void)


