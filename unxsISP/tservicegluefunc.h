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


void tServiceGlueNavList(void);

void ExtProcesstServiceGlueVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstServiceGlueVars(pentry entries[], int x)


void ExttServiceGlueCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tServiceGlueTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstServiceGlueVars(entries,x);
                        	guMode=2000;
	                        tServiceGlue(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstServiceGlueVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uServiceGlue=0;
				NewtServiceGlue(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstServiceGlueVars(entries,x);
			if(guPermLevel>=10)
			{
	                        guMode=2001;
				tServiceGlue(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstServiceGlueVars(entries,x);
			if(guPermLevel>=10)
			{
				guMode=5;
				DeletetServiceGlue();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstServiceGlueVars(entries,x);
			if(guPermLevel>=10)
			{
				guMode=2002;
				tServiceGlue(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstServiceGlueVars(entries,x);
			if(guPermLevel>=10)
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				ModtServiceGlue();
			}
                }
	}

}//void ExttServiceGlueCommands(pentry entries[], int x)


void ExttServiceGlueButtons(void)
{
	OpenFieldSet("tServiceGlue Aux Panel",100);
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
			//tServiceGlueNavList();
	}
	CloseFieldSet();

}//void ExttServiceGlueButtons(void)


void ExttServiceGlueAuxTable(void)
{

}//void ExttServiceGlueAuxTable(void)


void ExttServiceGlueGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uServiceGlue"))
		{
			sscanf(gentries[i].val,"%u",&uServiceGlue);
			guMode=6;
		}
	}
	tServiceGlue("");

}//void ExttServiceGlueGetHook(entry gentries[], int x)


void ExttServiceGlueSelect(void)
{
	sprintf(gcQuery,"SELECT "VAR_LIST_tServiceGlue" FROM tServiceGlue");	
	
}//void ExttServiceGlueSelect(void)


void ExttServiceGlueSelectRow(void)
{
	sprintf(gcQuery,"SELECT "VAR_LIST_tServiceGlue" FROM tServiceGlue WHERE uServiceGlue=%u",uServiceGlue);

}//void ExttServiceGlueSelectRow(void)


void ExttServiceGlueListSelect(void)
{
	char cCat[512];

	sprintf(gcQuery,"SELECT "VAR_LIST_tServiceGlue" FROM tServiceGlue");

	//Changes here must be reflected below in ExttServiceGlueListFilter()
        if(!strcmp(gcFilter,"uServiceGlue"))
        {
                sscanf(gcCommand,"%u",&uServiceGlue);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tServiceGlue.uServiceGlue=%u \
						ORDER BY uServiceGlue",
						uServiceGlue);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uServiceGlue");
        }

}//void ExttServiceGlueListSelect(void)


void ExttServiceGlueListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uServiceGlue"))
                printf("<option>uServiceGlue</option>");
        else
                printf("<option selected>uServiceGlue</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttServiceGlueListFilter(void)


void ExttServiceGlueNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	/*if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

	if(uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);

	if(uAllowDel(uOwner,uCreatedBy)) 
		printf(LANG_NBB_DELETE);

	if(uOwner)*/
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttServiceGlueNavBar(void)


void tServiceGlueNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tServiceGlue","tServiceGlue.uServiceGlue,tServiceGlue.cLabel",0);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tServiceGlueNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tServiceGlueNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tServiceGlue"
				"&uServiceGlue=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tServiceGlueNavList(void)


