/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author: (c) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details
 
*/


void tConfigurationNavList(void);

void ExtProcesstConfigurationVars(pentry entries[], int x)
{

	/*
	register int i;
	
	for(i=0;i<x;i++)
	{
	
	}
	*/

}//void ExtProcesstConfigurationVars(pentry entries[], int x)


void ExttConfigurationCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tConfigurationTools"))
	{
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
				ProcesstConfigurationVars(entries,x);
				//Check global conditions for new record here
				guMode=2000;
				tConfiguration(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
				ProcesstConfigurationVars(entries,x);
				//Check entries here
				uConfiguration=0;
				uCreatedBy=guLoginClient;
				uOwner=guLoginClient;
				uModBy=0;//Never modified
				NewtConfiguration(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstConfigurationVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2001;
				tConfiguration(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstConfigurationVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=5;
				DeletetConfiguration();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstConfigurationVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12)
				|| (guPermLevel>9 && uOwner!=1)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2002;
				tConfiguration(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstConfigurationVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=12)
				|| (guPermLevel>9 && uOwner!=1)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				uModBy=guLoginClient;
				ModtConfiguration();
			}
                }
	}

}//void ExttConfigurationCommands(pentry entries[], int x)


void ExttConfigurationButtons(void)
{
	OpenFieldSet("tConfiguration Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("Enter required data<br>");
                        printf(LANG_NBB_CONFIRMNEW);
			printf("<br>\n");
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
			printf("<br>\n");
                break;

                case 2002:
			printf("Review record data<br>");
                        printf(LANG_NBB_CONFIRMMOD);
			printf("<br>\n");
                break;

	}

        printf("<p><u>tConfigurationNavList</u><br>\n");
	tConfigurationNavList();

        printf("</td></tr>\n");
        printf("</table>\n");

}//void ExttConfigurationButtons(void)


void ExttConfigurationAuxTable(void)
{

}//void ExttConfigurationAuxTable(void)


void ExttConfigurationGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uConfiguration"))
		{
			sscanf(gentries[i].val,"%u",&uConfiguration);
			guMode=6;
		}
	}
	tConfiguration("");

}//void ExttConfigurationGetHook(entry gentries[], int x)


void ExttConfigurationSelect(void)
{
        //Set non search query here for tTableName()
	if(guPermLevel>=9)
	sprintf(gcQuery,"SELECT %s FROM tConfiguration ORDER BY\
					uConfiguration",
					VAR_LIST_tConfiguration);
	else
	sprintf(gcQuery,"SELECT %s FROM tConfiguration WHERE uOwner=%u ORDER BY\
					uConfiguration",
					VAR_LIST_tConfiguration,guLoginClient);

}//void ExttConfigurationSelect(void)


void ExttConfigurationSelectRow(void)
{
	if(guPermLevel<10)
                sprintf(gcQuery,"SELECT %s FROM tConfiguration,tClient \
                                WHERE tConfiguration.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)\
                                AND tConfiguration.uConfiguration=%u",
                        		VAR_LIST_tConfiguration,
					guLoginClient,guLoginClient,uConfiguration);
	else
                sprintf(gcQuery,"SELECT %s FROM tConfiguration WHERE uConfiguration=%u",
			VAR_LIST_tConfiguration,uConfiguration);

}//void ExttConfigurationSelectRow(void)


void ExttConfigurationListSelect(void)
{
	char cCat[512];

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT %s FROM tConfiguration,tClient \
		WHERE tConfiguration.uOwner=tClient.uClient \
		AND (tClient.uOwner=%u OR tClient.uClient=%u)",
				VAR_LIST_tConfiguration,
				guLoginClient,
				guLoginClient);
	else
                sprintf(gcQuery,"SELECT %s FROM tConfiguration",
				VAR_LIST_tConfiguration);

	//Changes here must be reflected below in ExttConfigurationListFilter()
        if(!strcmp(gcFilter,"uConfiguration"))
        {
                sscanf(gcCommand,"%u",&uConfiguration);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tConfiguration.uConfiguration=%u \
						ORDER BY uConfiguration",
						uConfiguration);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uConfiguration");
        }

}//void ExttConfigurationListSelect(void)


void ExttConfigurationListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uConfiguration"))
                printf("<option>uConfiguration</option>");
        else
                printf("<option selected>uConfiguration</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttConfigurationListFilter(void)


void ExttConfigurationNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10)
		printf(LANG_NBB_NEW);

	if( (guPermLevel>=12)
			|| (guPermLevel>9 && uOwner!=1)
			|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_MODIFY);

	if( (guPermLevel>=12 && uOwner==guLoginClient)
			|| (guPermLevel>9 && uOwner!=1)
			|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_DELETE);

	printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttConfigurationNavBar(void)


void tConfigurationNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT tConfiguration.uConfiguration\
				,tConfiguration.cLabel\
				FROM tConfiguration,tClient\
                                WHERE tConfiguration.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)",
					guLoginClient,guLoginClient);
	else
	        sprintf(gcQuery,"SELECT uConfiguration,cLabel FROM tConfiguration");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
        while((field=mysql_fetch_row(res)))
        {
                printf("<a class=darkLink href=unxsRadius.cgi?gcFunction=tConfiguration\
&uConfiguration=%s>%s</a><br>\n",field[0],field[1]);
        }
        mysql_free_result(res);

}//void tConfigurationNavList(void)


