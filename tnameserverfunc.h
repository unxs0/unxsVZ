/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2010 Unixservice, LLC.
 
*/

void tNameserverNavList(void);

//file scoped var
static unsigned uDatacenter=0;
static char cuDatacenterPullDown[256]={""};


void ExtProcesstNameserverVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uDatacenter"))
			sscanf(entries[i].val,"%u",&uDatacenter);
		else if(!strcmp(entries[i].name,"cuDatacenterPullDown"))
		{
			sprintf(cuDatacenterPullDown,"%.255s",entries[i].val);
			uDatacenter=ReadPullDown("tDatacenter","cLabel",cuDatacenterPullDown);
		}
	}
}//void ExtProcesstNameserverVars(pentry entries[], int x)


void ExttNameserverCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tNameserverTools"))
	{
        	MYSQL_RES *res;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstNameserverVars(entries,x);
                        	guMode=2000;
	                        tNameserver(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstNameserverVars(entries,x);

                        	guMode=2000;
				//Check entries here
				if(strlen(cLabel)<3)
					tNameserver("<blink>Error</blink>: cLabel too short!");
				sprintf(gcQuery,"SELECT uNameserver FROM tNameserver WHERE cLabel='%s'",
						cLabel);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tNameserver("<blink>Error</blink>: Nameserver cLabel in use!");
				}
                        	guMode=0;

				uNameserver=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtNameserver(1);
				if(!uNameserver)
					tNameserver("<blink>Error</blink>: New tNameserver entry was not created!");

				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType="PROP_NAMESERVER
						",cName='cDatacenter',cValue='All Datacenters',uOwner=%u,uCreatedBy=%u"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,uNameserver,guCompany,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				tNameserver("New nameserver created");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstNameserverVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=0;
				sprintf(gcQuery,"SELECT uNameserver FROM tContainer WHERE uNameserver=%u",
									uNameserver);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tNameserver("<blink>Error</blink>: Can't delete a nameserver used by a container!");
				}
	                        guMode=2001;
				tNameserver(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstNameserverVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				sprintf(gcQuery,"SELECT uNameserver FROM tContainer WHERE uNameserver=%u",
									uNameserver);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tNameserver("<blink>Error</blink>: Can't delete a nameserver used by a container!");
				}
				DeletetNameserver();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstNameserverVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tNameserver(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstNameserverVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				if(strlen(cLabel)<3)
					tNameserver("<blink>Error</blink>: cLabel too short!");
                        	guMode=0;

				uModBy=guLoginClient;
				ModtNameserver();
			}
                }
                else if(!strcmp(gcCommand,"Enable"))
                {
                        ProcesstNameserverVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=6;
				if(!uDatacenter)
					sprintf(gcQuery,"INSERT tProperty SET cName='cDatacenter',cValue='All Datacenters',"
						"uType=%u,uKey=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uPROP_NAMESERVER,uNameserver,uOwner,guLoginClient);
				else
					sprintf(gcQuery,"INSERT tProperty SET cName='cDatacenter',cValue='%s',"
						"uType=%u,uKey=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						ForeignKey("tDatacenter","cLabel",uDatacenter),	
						uPROP_NAMESERVER,uNameserver,uOwner,guLoginClient);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
			}
			else
			{
				tNameserver("<blink>Error</blink>: Enable not allowed!");
			}
		}
                else if(!strcmp(gcCommand,"Disable"))
                {
                        ProcesstNameserverVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=6;
				if(!uDatacenter)
					sprintf(gcQuery,"DELETE FROM tProperty WHERE cName='cDatacenter' AND cValue='All Datacenters'"
						" AND uType=%u AND uKey=%u AND (uOwner=%u OR uCreatedBy=%u)",
						uPROP_NAMESERVER,uNameserver,uOwner,guLoginClient);
				else
					sprintf(gcQuery,"DELETE FROM tProperty WHERE cName='cDatacenter' AND cValue='%s'"
						" AND uType=%u AND uKey=%u AND (uOwner=%u OR uCreatedBy=%u)",
						ForeignKey("tDatacenter","cLabel",uDatacenter),
						uPROP_NAMESERVER,uNameserver,uOwner,guLoginClient);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
			}
			else
			{
				tNameserver("<blink>Error</blink>: Disable not allowed!");
			}
		}
	}

}//void ExttNameserverCommands(pentry entries[], int x)


void ExttNameserverButtons(void)
{
	OpenFieldSet("tNameserver Aux Panel",100);
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
			printf("<u>Table Tips (%s)</u><br>",cGitVersion);
			printf("<p><u>Record Context Info</u><br>");
			tNameserverNavList();
	}
	CloseFieldSet();

}//void ExttNameserverButtons(void)


void ExttNameserverAuxTable(void)
{
	if(!uNameserver || guMode==2000 )//uMODE_NEW
		return;

        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"tNameserver %s Property Panel",cLabel);
	OpenFieldSet(gcQuery,100);
	sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE uKey=%u AND uType="PROP_NAMESERVER
			" ORDER BY cName",uNameserver);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
	printf("<table cols=2>");
	if(mysql_num_rows(res))
	{
		while((field=mysql_fetch_row(res)))
		{
			printf("<tr>\n");
			printf("<td width=200 valign=top><a class=darkLink href=unxsVZ.cgi?"
					"gcFunction=tProperty&uProperty=%s&cReturn=tNameserver_%u>"
					"%s</a></td><td>%s</td>\n",
						field[0],uNameserver,field[1],field[2]);
			printf("</tr>\n");
		}
	}

	//Simple interface to add to tConfiguration table
	if(uAllowMod(uOwner,uCreatedBy))
	{
		printf("<tr>");
		printf("<td width=200 valign=top><input type=submit class=largeButton"
		" title='Enable for one or more datacenters; for new container creation'"
		" name=gcCommand value='Enable'><p>");
		printf("<input type=submit class=largeButton"
		" title='Disable for one or more datacenters; for new container creation'"
		" name=gcCommand value='Disable'</td>");
		printf("<td valign=top> Select a datacenter or none (---) for all ");
		tTablePullDown("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,1);
		printf("</td>");
		printf("</tr>\n");
	}
	
	printf("</table>");

	CloseFieldSet();

}//void ExttNameserverAuxTable(void)


void ExttNameserverGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uNameserver"))
		{
			sscanf(gentries[i].val,"%u",&uNameserver);
			guMode=6;
		}
	}
	tNameserver("");

}//void ExttNameserverGetHook(entry gentries[], int x)


void ExttNameserverSelect(void)
{
	ExtSelect("tNameserver",VAR_LIST_tNameserver);

}//void ExttNameserverSelect(void)


void ExttNameserverSelectRow(void)
{
	ExtSelectRow("tNameserver",VAR_LIST_tNameserver,uNameserver);

}//void ExttNameserverSelectRow(void)


void ExttNameserverListSelect(void)
{
	char cCat[512];

	ExtListSelect("tNameserver",VAR_LIST_tNameserver);
	
	//Changes here must be reflected below in ExttNameserverListFilter()
        if(!strcmp(gcFilter,"uNameserver"))
        {
                sscanf(gcCommand,"%u",&uNameserver);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tNameserver.uNameserver=%u ORDER BY uNameserver",uNameserver);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uNameserver");
        }

}//void ExttNameserverListSelect(void)


void ExttNameserverListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uNameserver"))
                printf("<option>uNameserver</option>");
        else
                printf("<option selected>uNameserver</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttNameserverListFilter(void)


void ExttNameserverNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10 && !guListMode)
		printf(LANG_NBB_NEW);

	if(uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);

	if(uAllowDel(uOwner,uCreatedBy))
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttNameserverNavBar(void)


void tNameserverNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tNameserver","tNameserver.uNameserver,tNameserver.cLabel");
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tNameserverNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tNameserverNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tNameserver&"
				"uNameserver=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tNameserverNavList(void)


