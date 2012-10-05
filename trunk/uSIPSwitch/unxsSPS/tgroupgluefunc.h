/*
FILE
	$Id: modulefunc.h 2116 2012-09-19 23:00:28Z Gary $
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2012 Gary Wallis for Unixservice, LLC.
TEMPLATE VARS AND FUNCTIONS
	ModuleFunctionProcess
	ModuleFunctionProtos
	cProject
	cTableKey
	cTableName
*/



void ExtProcesstGroupGlueVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstGroupGlueVars(pentry entries[], int x)


void ExttGroupGlueCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tGroupGlueTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstGroupGlueVars(entries,x);
                        	guMode=2000;
	                        tGroupGlue(LANG_NB_CONFIRMNEW);
			}
			else
				tGroupGlue("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstGroupGlueVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uGroupGlue=0;
				NewtGroupGlue(0);
			}
			else
				tGroupGlue("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstGroupGlueVars(entries,x);
			if(guPermLevel>=9)
			{
	                        guMode=2001;
				tGroupGlue(LANG_NB_CONFIRMDEL);
			}
			else
				tGroupGlue("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstGroupGlueVars(entries,x);
			if(guPermLevel>=9)
			{
				guMode=5;
				DeletetGroupGlue();
			}
			else
				tGroupGlue("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstGroupGlueVars(entries,x);
			if(guPermLevel>=9)
			{
				guMode=2002;
				tGroupGlue(LANG_NB_CONFIRMMOD);
			}
			else
				tGroupGlue("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstGroupGlueVars(entries,x);
			if(guPermLevel>=9)
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;
				ModtGroupGlue();
			}
			else
				tGroupGlue("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttGroupGlueCommands(pentry entries[], int x)


void ExttGroupGlueButtons(void)
{
	OpenFieldSet("tGroupGlue Aux Panel",100);
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
			if(uGroupGlue && uGroupType && uKey && uGroup)
			{
				char cGroupTable[33]={""};
				char cKeyTable[33]={""};
				char *cp;
        			MYSQL_RES *res;
        			MYSQL_ROW field;

				sprintf(cGroupTable,"%.64s",ForeignKey("tGroupType","cLabel",uGroupType));
				if((cp=strchr(cGroupTable,':'))) *cp=0;
				sprintf(cKeyTable,"%.32s",cp+1);

				sprintf(gcQuery,"SELECT cLabel FROM %s WHERE _rowid=%u",cGroupTable,uGroup);
        			mysql_query(&gMysql,gcQuery);
        			if(mysql_errno(&gMysql))
                			printf("%s",mysql_error(&gMysql));
				else
				{
        				res=mysql_store_result(&gMysql);
	        			if((field=mysql_fetch_row(res)))
						printf("%s(%u)=%s<br>\n",cGroupTable,uGroup,field[0]);
				}

				sprintf(gcQuery,"SELECT cLabel FROM %s WHERE _rowid=%u",cKeyTable,uKey);
        			mysql_query(&gMysql,gcQuery);
        			if(mysql_errno(&gMysql))
                			printf("%s",mysql_error(&gMysql));
				else
				{
        				res=mysql_store_result(&gMysql);
	        			if((field=mysql_fetch_row(res)))
						printf("%s(%u)=%s<br>\n",cKeyTable,uKey,field[0]);
				}
			}
	}
	CloseFieldSet();

}//void ExttGroupGlueButtons(void)


void ExttGroupGlueAuxTable(void)
{

}//void ExttGroupGlueAuxTable(void)


void ExttGroupGlueGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uGroupGlue"))
		{
			sscanf(gentries[i].val,"%u",&uGroupGlue);
			guMode=6;
		}
	}
	tGroupGlue("");

}//void ExttGroupGlueGetHook(entry gentries[], int x)


void ExttGroupGlueSelect(void)
{
	sprintf(gcQuery,"SELECT %s FROM tGroupGlue ORDER BY uGroupGlue",VAR_LIST_tGroupGlue);

}//void ExttGroupGlueSelect(void)


void ExttGroupGlueSelectRow(void)
{
	sprintf(gcQuery,"SELECT %s FROM tGroupGlue WHERE uGroupGlue=%u",VAR_LIST_tGroupGlue,uGroupGlue);

}//void ExttGroupGlueSelectRow(void)


void ExttGroupGlueListSelect(void)
{
	char cCat[512];
	sprintf(gcQuery,"SELECT %s FROM tGroupGlue",VAR_LIST_tGroupGlue);

	//Changes here must be reflected below in ExttGroupGlueListFilter()
        if(!strcmp(gcFilter,"uGroupGlue"))
        {
                sscanf(gcCommand,"%u",&uGroupGlue);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tGroupGlue.uGroupGlue=%u"
						" ORDER BY uGroupGlue",
						uGroupGlue);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uGroupGlue");
        }

}//void ExttGroupGlueListSelect(void)


void ExttGroupGlueListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uGroupGlue"))
                printf("<option>uGroupGlue</option>");
        else
                printf("<option selected>uGroupGlue</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttGroupGlueListFilter(void)


void ExttGroupGlueNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

	if(guPermLevel>=9)
		printf(LANG_NBB_MODIFY);

	if(guPermLevel>=9)
		printf(LANG_NBB_DELETE);

	if(guPermLevel>=9)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttGroupGlueNavBar(void)



