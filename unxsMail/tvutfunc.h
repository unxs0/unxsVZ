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

static char cSearch[64]={""};

void tVUTNavList(void);
void tVUTEntriesAuxList(void);
void BasicVUTCheck(void);
void tVUTContextInfo(void);

//Extern
void AddVirtualEmail(unsigned uVUTExt);//tvutentriesfunc.h
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);

void ExtProcesstVUTVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",entries[i].val);
	}
	
}//void ExtProcesstVUTVars(pentry entries[], int x)


void ExttVUTCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tVUTTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstVUTVars(entries,x);
                        	guMode=2000;
	                        tVUT(LANG_NB_CONFIRMNEW);
			}
			else
				tVUT("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstVUTVars(entries,x);

                        	guMode=2000;
				//Check entries here
				BasicVUTCheck();
                        	guMode=0;

				uVUT=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtVUT(0);
			}
			else
				tVUT("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstVUTVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tVUT(LANG_NB_CONFIRMDEL);
			}
			else
				tVUT("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstVUTVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				char cServerGroup[100]={""};
				char cJobData[100]={""};
				guMode=5;
				sprintf(gcQuery,"DELETE FROM tVUTEntries WHERE uVUT=%u",uVUT);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				SubmitJob("DelVUT",cDomain,"all entries",cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				DeletetVUT();
			}
			else
				tVUT("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstVUTVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tVUT(LANG_NB_CONFIRMMOD);
			}
			else
				tVUT("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstVUTVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				BasicVUTCheck();
                        	guMode=0;

				uModBy=guLoginClient;
				ModtVUT();
			}
			else
				tVUT("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,"Add Virtual Email"))
		{
			ProcesstVUTVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				AddVirtualEmail(uVUT);
			}
			else
				tVUT("<blink>Error</blink>: Denied by permissions settings");
			
		}

	}

}//void ExttVUTCommands(pentry entries[], int x)


void ExttVUTButtons(void)
{
	OpenFieldSet("tVUT Aux Panel",100);
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
			printf("At this table you manage sendmail virtual user table (VUT). Here you can see the per domain records. "
				"If you load a domain it will list the VUT entries at the left panel. You can use the [Add Virtual Email] button "
				"for adding new VUT entries.<br>"
				"Virtual emails give you the possibility to match any email address to an arbitrary mailbox existent at the system. E.g.:<br>"
				"<i>sales@unixservice.com</i> emails are sent to the qwerty mailbox.");

			if(uVUT)
			{
				printf("<p><u>Record Context Info</u><br>");
				tVUTContextInfo();
				printf("<p><input title='Enter a mySQL search pattern for cDomain' type=text "
					"name=cSearch value='%s' maxlength=63 size=20>\n",cSearch);
				tVUTNavList();
				tVUTEntriesAuxList();
			}

			if(uServerGroup && cDomain[0] && uVUT)
				printf("<p><input class=largeButton title='Add a virtual user table entry for "
					"this domain and server group' type=submit name=gcCommand value='Add Virtual Email'>");
	}
	CloseFieldSet();

}//void ExttVUTButtons(void)


void ExttVUTAuxTable(void)
{

}//void ExttVUTAuxTable(void)


void ExttVUTGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uVUT"))
		{
			sscanf(gentries[i].val,"%u",&uVUT);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.63s",gentries[i].val);
		}
	}
	tVUT("");

}//void ExttVUTGetHook(entry gentries[], int x)


void ExttVUTSelect(void)
{
        //Set non search gcQuery here for tTableName()
	if(cSearch[0])
		ExtSelectSearch("tVUT",VAR_LIST_tVUT,"cDomain",cSearch,NULL,0);
	else
		ExtSelect("tVUT",VAR_LIST_tVUT,0);
	
}//void ExttVUTSelect(void)


void ExttVUTSelectRow(void)
{
	ExtSelectRow("tVUT",VAR_LIST_tVUT,uVUT);
	
}//void ExttVUTSelectRow(void)


void ExttVUTListSelect(void)
{
	char cCat[512];

	ExtListSelect("tVUT",VAR_LIST_tVUT);
	
	//Changes here must be reflected below in ExttVUTListFilter()
        if(!strcmp(gcFilter,"uVUT"))
        {
                sscanf(gcCommand,"%u",&uVUT);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tVUT.uVUT=%u \
						ORDER BY uVUT",
						uVUT);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uVUT");
        }

}//void ExttVUTListSelect(void)


void ExttVUTListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uVUT"))
                printf("<option>uVUT</option>");
        else
                printf("<option selected>uVUT</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttVUTListFilter(void)


void ExttVUTNavBar(void)
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

}//void ExttVUTNavBar(void)


void tVUTNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;

	printf("<p><u>tVUTNavList</u><br>\n");

	if(!cSearch[0])
	{
        	printf("Must restrict via cSearch\n");
		return;
	}

	ExtSelectSearch("tVUT"," uVUT,cDomain","cDomain",cSearch,NULL,101);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
	        while((field=mysql_fetch_row(res)))
		{
			uCount++;
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tVUT&uVUT=%s&cSearch=%s>%s</a><br>\n",field[0],cURLEncode(cSearch),field[1]);
			if(uCount>=100)
			{
				printf("More than 100 records: You must refine your search further<br>\n");
				break;
			}
	        }
	}
	else
		printf("No records found<br>");
	
        mysql_free_result(res);

}//void tVUTNavList(void)


void tVUTEntriesAuxList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;
	
	sprintf(gcQuery,"SELECT uVUTEntries,cVirtualEmail,cTargetEmail FROM tVUTEntries WHERE uVUT=%u",uVUT);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
        if(mysql_num_rows(res))
	{
        	printf("<p><u>tVUTEntriesNavList</u><br>\n");

        	while((field=mysql_fetch_row(res)))
        	{
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tVUTEntries&uVUTEntries=%s>%s@%s:%s</a><br>\n",
				field[0],field[1],cDomain,field[2]);
			uCount++;
			if(uCount>=100)
			{
				printf("Only the first 100 tVUTEntries records displayed<br>");
				break;
			}
        	}
	}
        mysql_free_result(res);

}//void tVUTEntriesNavList(void)


void BasicVUTCheck(void)
{
	char *cp;

	if(!cDomain[0])
		tVUT("<blink>Error:</blink> cDomain can't be empty");

	if(strlen(cDomain)<4)
		tVUT("<blink>Error</blink>: Must provide valid cDomain");

	if((cp=strchr(cDomain,'.')))
	{
		cp++;
		if(strlen(cp)<2)
			tVUT("<blink>Error</blink>: cDomain after '.' part is too short");
	}
	else
	{
		tVUT("<blink>Error</blink>: cDomain must have '.' TLD part");
	}

	if(!uServerGroup)
		tVUT("<blink>Error:</blink> Must select uServerGroup");
	
}//void BasicVUTCheck(void)


void tVUTContextInfo(void)
{
}//void tVUTContextInfo(void)


