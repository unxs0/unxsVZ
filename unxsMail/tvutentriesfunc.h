/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
 
*/

static char cSearch[64]={""};

//ModuleFunctionProtos()


void tVUTEntriesNavList(void);
void AddVirtualEmail(unsigned uVUTExt);
void BasicVUTEntriesCheck(void);
void tVUTEntriesContextInfo(void);
void SetuClient(void);

//Extern
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);

void ExtProcesstVUTEntriesVars(pentry entries[], int x)
{
	
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",entries[i].val);
	}
	
}//void ExtProcesstVUTEntriesVars(pentry entries[], int x)


void ExttVUTEntriesCommands(pentry entries[], int x)
{
	char cServerGroup[100]={""};
	char cDomain[100]={""};
	char cJobData[100]={""};
	unsigned uServerGroup=0;
	
	if(!strcmp(gcFunction,"tVUTEntriesTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstVUTEntriesVars(entries,x);
                        	guMode=2000;
	                        tVUTEntries(LANG_NB_CONFIRMNEW);
			}
			else
				tVUTEntries("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstVUTEntriesVars(entries,x);

                        	guMode=2000;
				//Check entries here
				BasicVUTEntriesCheck();
                        	guMode=0;

				uVUTEntries=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				SetuClient();

				NewtVUTEntries(1);
				//htmlPlainTextError("d1");
				if(!uVUTEntries)
					 htmlPlainTextError("Unexpected NewtVUTEntries() error");
				
				sscanf(ForeignKey("tVUT","uServerGroup",uVUT),"%u",&uServerGroup);
				sprintf(cDomain,ForeignKey("tVUT","cDomain",uVUT));
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uVUTEntries=%u;\ncVirtualEmail=%s;\ncTargetEmail=%s;\ncDomain=%s;\n",
						uVUTEntries,cVirtualEmail,cTargetEmail,cDomain);
				SubmitJob("NewVUT",cDomain,cVirtualEmail,cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				tVUTEntries("New tVUTEntries record created. Job(s) created");
			}
			else
				tVUTEntries("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstVUTEntriesVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tVUTEntries(LANG_NB_CONFIRMDEL);
			}
			else
				tVUTEntries("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstVUTEntriesVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				sscanf(ForeignKey("tVUT","uServerGroup",uVUT),"%u",&uServerGroup);
				sprintf(cDomain,ForeignKey("tVUT","cDomain",uVUT));
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uVUTEntries=%u;\ncVirtualEmail=%s;\ncTargetEmail=%s;\ncDomain=%s;\n",
						uVUTEntries,cVirtualEmail,cTargetEmail,cDomain);
				SubmitJob("DelVUT",cDomain,cVirtualEmail,cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				
				DeletetVUTEntries();
			}
			else
				tVUTEntries("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstVUTEntriesVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tVUTEntries(LANG_NB_CONFIRMMOD);
			}
			else
				tVUTEntries("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstVUTEntriesVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				BasicVUTEntriesCheck();
                        	guMode=0;

				uModBy=guLoginClient;
				SetuClient();

				sscanf(ForeignKey("tVUT","uServerGroup",uVUT),"%u",&uServerGroup);
				sprintf(cDomain,ForeignKey("tVUT","cDomain",uVUT));
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uVUTEntries=%u;\ncVirtualEmail=%s;\ncTargetEmail=%s;\ncDomain=%s;\n"
						,uVUTEntries,cVirtualEmail,cTargetEmail,cDomain);
				SubmitJob("ModVUT",cDomain,cVirtualEmail,cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				ModtVUTEntries();
			}
			else
				tVUTEntries("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttVUTEntriesCommands(pentry entries[], int x)


void ExttVUTEntriesButtons(void)
{
	OpenFieldSet("tVUTEntries Aux Panel",100);
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
			if(uVUTEntries)
			{
				printf("<p><u>Record Context Info</u><br>");
				tVUTEntriesContextInfo();
				printf("<p><input title='Enter a mySQL search pattern for cVirtualEmail' "
					"type=text name=cSearch value='%s' maxlength=63 size=20>\n",cSearch);
				tVUTEntriesNavList();

				if(uVUT)
					printf("<p><a class=darkLink href=unxsMail.cgi?gcFunction=tVUT&uVUT=%u>Quick link to tVUT</a><br>\n",uVUT);
			}
	}
	CloseFieldSet();

}//void ExttVUTEntriesButtons(void)


void ExttVUTEntriesAuxTable(void)
{

}//void ExttVUTEntriesAuxTable(void)


void ExttVUTEntriesGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uVUTEntries"))
		{
			sscanf(gentries[i].val,"%u",&uVUTEntries);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",gentries[i].val);
	}
	tVUTEntries("");

}//void ExttVUTEntriesGetHook(entry gentries[], int x)


void ExttVUTEntriesSelect(void)
{
	if(cSearch[0])
		ExtSelectSearch("tVUTEntries",VAR_LIST_tVUTEntries,"cVirtualEmail",cSearch,NULL,101);
	else
		ExtSelect("tVUTEntries",VAR_LIST_tVUTEntries,0);
	
			
}//void ExttVUTEntriesSelect(void)


void ExttVUTEntriesSelectRow(void)
{
	ExtSelectRow("tVUTEntries",VAR_LIST_tVUTEntries,uVUTEntries);

}//void ExttVUTEntriesSelectRow(void)


void ExttVUTEntriesListSelect(void)
{
	char cCat[512];

	ExtListSelect("tVUTEntries",VAR_LIST_tVUTEntries);
	
	//Changes here must be reflected below in ExttVUTEntriesListFilter()
        if(!strcmp(gcFilter,"uVUTEntries"))
        {
                sscanf(gcCommand,"%u",&uVUTEntries);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tVUTEntries.uVUTEntries=%u \
						ORDER BY uVUTEntries",
						uVUTEntries);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uVUTEntries");
        }

}//void ExttVUTEntriesListSelect(void)


void ExttVUTEntriesListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uVUTEntries"))
                printf("<option>uVUTEntries</option>");
        else
                printf("<option selected>uVUTEntries</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttVUTEntriesListFilter(void)


void ExttVUTEntriesNavBar(void)
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

}//void ExttVUTEntriesNavBar(void)


void tVUTEntriesNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;
	
	printf("<p><u>tVUTEntriesNavList</u><br>\n");
	
	if(!cSearch[0])
	{
        	printf("Must restrict via cSearch\n");
		return;
	}
			
	ExtSelectSearch("tVUTEntries","uVUTEntries,cVirtualEmail,cTargetEmail,(SELECT tVUT.cDomain FROM tVUT WHERE tVUTEntries.uVUT=tVUT.uVUT)",
			"cVirtualEmail",cSearch,NULL,101);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tVUTEntriesNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tVUTEntriesNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
			uCount++;
printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tVUTEntries&uVUTEntries=%s&cSearch=%s>%s@%s:%s</a><br>\n",
		field[0],cURLEncode(cSearch),field[1],field[3],field[2]);
			if(uCount>=100)
			{
				printf("More than 100 records: You must refine your search further<br>\n");
				break;
			}
	        }
	}
        mysql_free_result(res);

}//void tVUTEntriesNavList(void)


void AddVirtualEmail(unsigned uVUTExt)
{
	uVUT=uVUTExt;
	if(guPermLevel>=7)
	{
		guMode=2000;
		tVUTEntries(LANG_NB_CONFIRMNEW);
	}

}//void AddVirtualEmail(unsigned uVUTExt)


void BasicVUTEntriesCheck(void)
{
	MYSQL_RES *res;
	char cDomain[100];
	char *cp;
	char *cp2;
	unsigned uServerGroup=0;

	if(strchr(cVirtualEmail,'@'))
		tVUTEntries("cVirtualEmail must not have domain");
	if(!cVirtualEmail[0])
		tVUTEntries("Must specify cVirtualEmail");
	if(!cTargetEmail[0])
		tVUTEntries("Must specify cTargetEmail");
	if(strchr(cTargetEmail,','))
		tVUTEntries("cTargetEmail must not have ','");
	
	sprintf(gcQuery,"SELECT uVUTEntries FROM tVUTEntries WHERE cVirtualEmail='%s' AND uVUT=%u AND uVUTEntries!=%u",cVirtualEmail,uVUT,uVUTEntries);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
		tVUTEntries("<blink>Error: </blink>cVirtualEmail already in use for this domain");

	sscanf(ForeignKey("tVUT","uServerGroup",uVUT),"%u",&uServerGroup);

	if(!(cp=strchr(cTargetEmail,'@')))
	{
		//Allow special error:nouser lines for DEFAULT only
		if(!strcmp(cVirtualEmail,"DEFAULT") && !strncmp(cTargetEmail,"error:nouser",12))
			return;

		//Check tAliases
		sprintf(gcQuery,"SELECT uAlias FROM tAlias WHERE cUser='%s' AND uServerGroup=%u",cTargetEmail,uServerGroup);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
        	res=mysql_store_result(&gMysql);
		if(!mysql_num_rows(res))
		{

			//Check tUser
			sprintf(gcQuery,"SELECT uUser FROM tUser WHERE cLogin='%s' AND uServerGroup=%u",cTargetEmail,uServerGroup);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			if(!mysql_num_rows(res))
				tVUTEntries("Local cTargetEmail not in tAlias or tUser.");
		}
	}
	else
	{
		if(strlen(cTargetEmail)<6)
			tVUTEntries("cTargetEmail is too short to be a valid email");
		cp++;
		if(*cp==0)
			tVUTEntries("cTargetEmail must not end with @");
		if(!(cp2=strchr(cp,'.')))
			tVUTEntries("cTargetEmail must have a '.' after the '@'");
		if(strlen(cp)<4)
			tVUTEntries("cTargetEmail after '@' part is too short");
		cp2++;
		if(*cp2==0)
			tVUTEntries("cTargetEmail must not end with .");
		if(strlen(cp2)<2)
			tVUTEntries("cTargetEmail after '@' and '.' part is too short");
	}

	if((cp=strchr(cTargetEmail,'@')))
	{
		strncpy(cDomain,ForeignKey("tVUT","cDomain",uVUT),99);
		cDomain[99]=0;
		if(!strcasecmp(cp+1,cDomain))
			tVUTEntries("cTargetEmail domain must not be tVUT.cDomain");
	}
        else
        {
                sprintf(gcQuery,"SELECT uUser from tUser WHERE cLogin='%s' AND uServerGroup=%u",cTargetEmail,uServerGroup);
                mysql_query(&gMysql,gcQuery);
                if(mysql_errno(&gMysql))
                        htmlPlainTextError(mysql_error(&gMysql));
                res=mysql_store_result(&gMysql);
                if(!mysql_num_rows(res))
                {

                        sprintf(gcQuery,"SELECT uAlias from tAlias WHERE cUser='%s' AND uServerGroup=%u",cTargetEmail,uServerGroup);
                        mysql_query(&gMysql,gcQuery);
                        if(mysql_errno(&gMysql))
                                htmlPlainTextError(mysql_error(&gMysql));
                        res=mysql_store_result(&gMysql);
                        if(!mysql_num_rows(res))
                        {
                                mysql_free_result(res);
                                tVUTEntries("Unexpected error: Missing '@' in cTargetEmail. Or cTargetEmail not in tUser or tAlias");
                        }
                }
                mysql_free_result(res);
        }


}//void BasicVUTEntriesCheck(void)


void tVUTEntriesContextInfo(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	printf("<u>Users/Aliases</u><br>\n");
	sprintf(gcQuery,"SELECT tUser.uUser,tUser.cLogin,tServerGroup.cLabel FROM tUser,tServerGroup WHERE "
			"tUser.cLogin='%s' AND tUser.uServerGroup=tServerGroup.uServerGroup",cTargetEmail);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(!mysql_num_rows(res))
	{
		sprintf(gcQuery,"SELECT uAlias,cUser,tServerGroup.cLabel from tAlias,tServerGroup "
				"WHERE tAlias.cUser='%s' AND tAlias.uServerGroup=tServerGroup.uServerGroup",cTargetEmail);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);

		if(mysql_num_rows(res))
			while((field=mysql_fetch_row(res)))
				printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tAlias&uAlias=%s>%s/%s</a><br>\n",
						field[0],field[1],field[2]);
		else
			printf("No users<br>\n");
	}
	else
	{
		while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tUser&uUser=%s>%s/%s</a><br>\n",
					field[0],field[1],field[2]);
	}

	printf("<u>Jobs</u><br>\n");
	sprintf(gcQuery,"SELECT tJob.uJob,tJob.cJobName,tJob.cServer FROM tJob WHERE tJob.cLabel LIKE '%%VUTEntries%%%s%%' AND tJob.uJobStatus!=3",
						cVirtualEmail);
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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tJob&uJob=%s>%s/%s</a><br>\n",
						field[0],field[1],field[2]);
	}
	else
	{
		printf("No pending jobs<br>\n");
	}
        mysql_free_result(res);

}//void tVUTEntriesContextInfo(void)


void SetuClient(void)
{
	//This function will set the uClient field value based on which tUser/tAlias record the tVUTEntries record points to
	//It wil be called upon new and mod.
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uClient FROM tUser WHERE cLogin='%s'",cTargetEmail);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uClient);
	else
	{
		//Will look via tAlias
		mysql_free_result(res);
		sprintf(gcQuery,"SELECT tUser.uClient FROM tAlias,tUser WHERE tUser.cLogin=tAlias.cTargetEmail AND tAlias.cUser='%s'",cTargetEmail);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);

		if((field=mysql_fetch_row(res)))
			sscanf(field[0],"%u",&uClient);
	}

	mysql_free_result(res);

}//void SetuClient(void)

