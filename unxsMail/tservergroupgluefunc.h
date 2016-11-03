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


void tServerGroupGlueNavList(void);

void ExtProcesstServerGroupGlueVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstServerGroupGlueVars(pentry entries[], int x)


void ExttServerGroupGlueCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tServerGroupGlueTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstServerGroupGlueVars(entries,x);
                        	guMode=2000;
	                        tServerGroupGlue(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstServerGroupGlueVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uServerGroupGlue=0;
				NewtServerGroupGlue(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstServerGroupGlueVars(entries,x);
			if( guPermLevel>=12 )
			{
	                        guMode=2001;
				tServerGroupGlue(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstServerGroupGlueVars(entries,x);
			if( guPermLevel>=12 )
			{
				guMode=5;
				DeletetServerGroupGlue();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstServerGroupGlueVars(entries,x);
			if( guPermLevel>=10 )
			{
				guMode=2002;
				tServerGroupGlue(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstServerGroupGlueVars(entries,x);
			if( guPermLevel>=10 )
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				ModtServerGroupGlue();
			}
                }
	}

}//void ExttServerGroupGlueCommands(pentry entries[], int x)


void ExttServerGroupGlueButtons(void)
{
	OpenFieldSet("tServerGroupGlue Aux Panel",100);
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
			tServerGroupGlueNavList();
	}
	CloseFieldSet();

}//void ExttServerGroupGlueButtons(void)


void ExttServerGroupGlueAuxTable(void)
{

}//void ExttServerGroupGlueAuxTable(void)


void ExttServerGroupGlueGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uServerGroupGlue"))
		{
			sscanf(gentries[i].val,"%u",&uServerGroupGlue);
			guMode=6;
		}
	}
	tServerGroupGlue("");

}//void ExttServerGroupGlueGetHook(entry gentries[], int x)


void ExttServerGroupGlueSelect(void)
{
        //Set non search gcQuery here for tTableName()
	if(guPermLevel>=9)
	sprintf(gcQuery,"SELECT %s FROM tServerGroupGlue ORDER BY uServerGroupGlue",
					VAR_LIST_tServerGroupGlue);

}//void ExttServerGroupGlueSelect(void)


void ExttServerGroupGlueSelectRow(void)
{
	sprintf(gcQuery,"SELECT %s FROM tServerGroupGlue WHERE uServerGroupGlue=%u",
			VAR_LIST_tServerGroupGlue,uServerGroupGlue);

}//void ExttServerGroupGlueSelectRow(void)


void ExttServerGroupGlueListSelect(void)
{
	char cCat[512];

	sprintf(gcQuery,"SELECT %s FROM tServerGroupGlue",VAR_LIST_tServerGroupGlue);

	//Changes here must be reflected below in ExttServerGroupGlueListFilter()
        if(!strcmp(gcFilter,"uServerGroupGlue"))
        {
                sscanf(gcCommand,"%u",&uServerGroupGlue);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tServerGroupGlue.uServerGroupGlue=%u \
						ORDER BY uServerGroupGlue",
						uServerGroupGlue);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uServerGroupGlue");
        }

}//void ExttServerGroupGlueListSelect(void)


void ExttServerGroupGlueListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uServerGroupGlue"))
                printf("<option>uServerGroupGlue</option>");
        else
                printf("<option selected>uServerGroupGlue</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttServerGroupGlueListFilter(void)


void ExttServerGroupGlueNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10 && !guListMode)
		printf(LANG_NBB_NEW);

	if( guPermLevel>=10 )
		printf(LANG_NBB_MODIFY);

	if( guPermLevel>=12 )
		printf(LANG_NBB_DELETE);

	printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttServerGroupGlueNavBar(void)


void tServerGroupGlueNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tServerGroupGlue.uServerGroupGlue,tServerGroup.cLabel,tServer.cLabel FROM tServerGroupGlue,tServerGroup,tServer WHERE tServerGroupGlue.uServerGroup=tServerGroup.uServerGroup AND tServerGroupGlue.uServer=tServer.uServer ORDER BY tServerGroup.cLabel,tServer.cLabel LIMIT 101");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tServerGroupGlueNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tServerGroupGlueNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tServerGroupGlue&uServerGroupGlue=%s>%s/%s</a><br>\n",field[0],field[1],field[2]);
	        }
	}
        mysql_free_result(res);

}//void tServerGroupGlueNavList(void)


