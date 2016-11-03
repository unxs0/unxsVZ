/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2010 Unixservice, LLC.
 
*/

void tSearchdomainNavList(void);

//file scoped var
static unsigned uDatacenter=0;
static char cuDatacenterPullDown[256]={""};


void ExtProcesstSearchdomainVars(pentry entries[], int x)
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
}//void ExtProcesstSearchdomainVars(pentry entries[], int x)


void ExttSearchdomainCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tSearchdomainTools"))
	{
        	MYSQL_RES *res;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstSearchdomainVars(entries,x);
                        	guMode=2000;
	                        tSearchdomain(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstSearchdomainVars(entries,x);

                        	guMode=2000;
				//Check entries here
				if(strlen(cLabel)<3)
					tSearchdomain("<blink>Error</blink>: cLabel too short!");
				sprintf(gcQuery,"SELECT uSearchdomain FROM tSearchdomain WHERE cLabel='%s'",
						cLabel);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tSearchdomain("<blink>Error</blink>: Searchdomain cLabel in use!");
				}
                        	guMode=0;

				uSearchdomain=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtSearchdomain(1);
				if(!uSearchdomain)
					tSearchdomain("<blink>Error</blink>: New tSearchdomain entry was not created!");

				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType="PROP_SEARCHDOMAIN
						",cName='cDatacenter',cValue='All Datacenters',uOwner=%u,uCreatedBy=%u"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,uSearchdomain,guCompany,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				tSearchdomain("New searchdomain created");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstSearchdomainVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=0;
				sprintf(gcQuery,"SELECT uSearchdomain FROM tContainer WHERE uSearchdomain=%u",
									uSearchdomain);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tSearchdomain("<blink>Error</blink>: Can't delete a searchdomain"
							" used by a container!");
				}
	                        guMode=2001;
				tSearchdomain(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstSearchdomainVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				sprintf(gcQuery,"SELECT uSearchdomain FROM tContainer WHERE uSearchdomain=%u",
									uSearchdomain);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tSearchdomain("<blink>Error</blink>: Can't delete a searchdomain"
							" used by a container!");
				}
	                        guMode=0;
				DeletetSearchdomain();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstSearchdomainVars(entries,x);
			{
				guMode=2002;
				tSearchdomain(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstSearchdomainVars(entries,x);
			{
                        	guMode=2002;
				//Check entries here
				if(strlen(cLabel)<3)
					tSearchdomain("<blink>Error</blink>: cLabel too short!");
                        	guMode=0;

				uModBy=guLoginClient;
				ModtSearchdomain();
			}
                }
                else if(!strcmp(gcCommand,"Enable"))
                {
                        ProcesstSearchdomainVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=6;
				if(!uDatacenter)
					sprintf(gcQuery,"INSERT tProperty SET cName='cDatacenter',cValue='All Datacenters',"
						"uType=%u,uKey=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uPROP_SEARCHDOMAIN,uSearchdomain,uOwner,guLoginClient);
				else
					sprintf(gcQuery,"INSERT tProperty SET cName='cDatacenter',cValue='%s',"
						"uType=%u,uKey=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						ForeignKey("tDatacenter","cLabel",uDatacenter),	
						uPROP_SEARCHDOMAIN,uSearchdomain,uOwner,guLoginClient);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
			}
			else
			{
				tSearchdomain("<blink>Error</blink>: Enable not allowed!");
			}
		}
                else if(!strcmp(gcCommand,"Disable"))
                {
                        ProcesstSearchdomainVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=6;
				if(!uDatacenter)
					sprintf(gcQuery,"DELETE FROM tProperty WHERE cName='cDatacenter' AND cValue='All Datacenters'"
						" AND uType=%u AND uKey=%u AND (uOwner=%u OR uCreatedBy=%u)",
						uPROP_SEARCHDOMAIN,uSearchdomain,uOwner,guLoginClient);
				else
					sprintf(gcQuery,"DELETE FROM tProperty WHERE cName='cDatacenter' AND cValue='%s'"
						" AND uType=%u AND uKey=%u AND (uOwner=%u OR uCreatedBy=%u)",
						ForeignKey("tDatacenter","cLabel",uDatacenter),
						uPROP_SEARCHDOMAIN,uSearchdomain,uOwner,guLoginClient);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
			}
			else
			{
				tSearchdomain("<blink>Error</blink>: Disable not allowed!");
			}
		}
	}

}//void ExttSearchdomainCommands(pentry entries[], int x)


void ExttSearchdomainButtons(void)
{
	OpenFieldSet("tSearchdomain Aux Panel",100);
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
			tSearchdomainNavList();
	}
	CloseFieldSet();

}//void ExttSearchdomainButtons(void)


void ExttSearchdomainAuxTable(void)
{
	if(!uSearchdomain || guMode==2000 )//uMODE_NEW
		return;

        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"tSearchdomain %s Property Panel",cLabel);
	OpenFieldSet(gcQuery,100);
	sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE uKey=%u AND uType="PROP_SEARCHDOMAIN
			" ORDER BY cName",uSearchdomain);

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
					"gcFunction=tProperty&uProperty=%s&cReturn=tSearchdomain_%u>"
					"%s</a></td><td>%s</td>\n",
						field[0],uSearchdomain,field[1],field[2]);
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

}//void ExttSearchdomainAuxTable(void)


void ExttSearchdomainGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uSearchdomain"))
		{
			sscanf(gentries[i].val,"%u",&uSearchdomain);
			guMode=6;
		}
	}
	tSearchdomain("");

}//void ExttSearchdomainGetHook(entry gentries[], int x)


void ExttSearchdomainSelect(void)
{
	ExtSelect("tSearchdomain",VAR_LIST_tSearchdomain);

}//void ExttSearchdomainSelect(void)


void ExttSearchdomainSelectRow(void)
{
	ExtSelectRow("tSearchdomain",VAR_LIST_tSearchdomain,uSearchdomain);

}//void ExttSearchdomainSelectRow(void)


void ExttSearchdomainListSelect(void)
{
	char cCat[512];

	ExtListSelect("tSearchdomain",VAR_LIST_tSearchdomain);
	
	//Changes here must be reflected below in ExttSearchdomainListFilter()
        if(!strcmp(gcFilter,"uSearchdomain"))
        {
                sscanf(gcCommand,"%u",&uSearchdomain);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tSearchdomain.uSearchdomain=%u ORDER BY uSearchdomain",
						uSearchdomain);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uSearchdomain");
        }

}//void ExttSearchdomainListSelect(void)


void ExttSearchdomainListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uSearchdomain"))
                printf("<option>uSearchdomain</option>");
        else
                printf("<option selected>uSearchdomain</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttSearchdomainListFilter(void)


void ExttSearchdomainNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10 && !guListMode)
		printf(LANG_NBB_NEW);

		printf(LANG_NBB_MODIFY);

		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttSearchdomainNavBar(void)


void tSearchdomainNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	ExtSelect("tSearchdomain","tSearchdomain.uSearchdomain,tSearchdomain.cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tSearchdomainNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tSearchdomainNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tSearchdomain&"
				"uSearchdomain=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tSearchdomainNavList(void)


