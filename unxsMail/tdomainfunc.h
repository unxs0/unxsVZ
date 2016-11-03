/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
 
*/

#include <ctype.h>
static char cSearch[64]={""};


//Extern
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);
void tDomainNavList(void);
void BasicDomainCheck(void);
void tDomainContextInfo(void);
unsigned DomainIsInUse(unsigned uDomain, char *cDomain);
void AlreadyUsedHere(void);
void htmlRelayCheck(void);
void SelectuServerGroup(char *cInputName, unsigned uSelected);
void AddExtraRecords(void);
void UpdateDependencies(void);


static unsigned uAddtLocal=0;
static unsigned uAddtRelay=0;
static unsigned uAddtVUT=0;
static unsigned uLocalServerGroup=0;
static unsigned uRelayServerGroup=0;
static char cTransport[64]={""};

void ExtProcesstDomainVars(pentry entries[], int x)
{
	
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",entries[i].val);
		else if(!strcmp(entries[i].name,"uAddtLocal"))
			sscanf(entries[i].val,"%u",&uAddtLocal);
		else if(!strcmp(entries[i].name,"uAddtRelay"))
			sscanf(entries[i].val,"%u",&uAddtRelay);
		else if(!strcmp(entries[i].name,"uAddtVUT"))
			sscanf(entries[i].val,"%u",&uAddtVUT);
		else if(!strcmp(entries[i].name,"uLocalServerGroup"))
			sscanf(entries[i].val,"%u",&uLocalServerGroup);
		else if(!strcmp(entries[i].name,"uRelayServerGroup"))
			sscanf(entries[i].val,"%u",&uRelayServerGroup);
		else if(!strcmp(entries[i].name,"cTransport"))
			sprintf(cTransport,"%.63s",entries[i].val);
			
	}
	
}//void ExtProcesstDomainVars(pentry entries[], int x)


void ExttDomainCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tDomainTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstDomainVars(entries,x);
                        	guMode=2000;
	                        tDomain(LANG_NB_CONFIRMNEW);
			}
			else
				tDomain("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstDomainVars(entries,x);

                        	guMode=2000;
				//Check entries here
				BasicDomainCheck();
				AlreadyUsedHere();
                        	guMode=0;

				uDomain=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				AddExtraRecords();
				NewtDomain(0);
			}
			else
				tDomain("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstDomainVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				if(!uDomain)
					tDomain("<blink>Error</blink>: No uDomain!");
				if(DomainIsInUse(uDomain,cDomain))
					tDomain("<blink>Error</blink>: Domain is in use!");
	                        guMode=2001;
				tDomain(LANG_NB_CONFIRMDEL);
			}
			else
				tDomain("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstDomainVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				//Checks
	                        guMode=2001;
				BasicDomainCheck();
				if(DomainIsInUse(uDomain,cDomain))
					tDomain("<blink>Error</blink>: Domain is in use!");

				guMode=5;
				DeletetDomain();
			}
			else
				tDomain("<blink>Error</blink>: Denied by permissions settings");

                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstDomainVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tDomain(LANG_NB_CONFIRMMOD);
			}
			else
				tDomain("<blink>Error</blink>: Denied by permissions settings");
			
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstDomainVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				BasicDomainCheck();
                        	guMode=0;

				uModBy=guLoginClient;
				UpdateDependencies();
				ModtDomain();
			}
			else
				tDomain("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,"tRelay Check"))
			htmlRelayCheck();
	}

}//void ExttDomainCommands(pentry entries[], int x)


void ExttDomainButtons(void)
{
	OpenFieldSet("tDomain Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter/mod data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
			printf("<br><br><u>tLocal entry</u> <input type=checkbox name=uAddtLocal value=1><br>\n");
			printf("uServerGroup \n");
			SelectuServerGroup("uLocalServerGroup",uLocalServerGroup);
			
			printf("<br><br><u>tRelay entry</u> <input type=checkbox name=uAddtRelay value=1><br>\n");
			printf("uServerGroup \n");
			SelectuServerGroup("uRelayServerGroup",uRelayServerGroup);
			printf("<br>cTransport <input type=text name=cTransport size=20>\n");

			printf("<br><br><u>tVUT entry</u> <input type=checkbox name=uAddtVUT value=1>\n");

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
			printf("All fully qualified domain names (FQDN) used in this system must exist in this table. "
				"This accomplished several purposes, quick overview of mail domains, rule checking and "
				"easy entry where it makes sense.");
			if(uDomain)
			{
				printf("<p><u>Record Context Info</u><br>");
				tDomainContextInfo();
				printf("<p><input title='Enter a mySQL search pattern for cDomain' type=text name=cSearch "
					"value='%s' maxlength=63 size=20> cSearch\n",cSearch);
				tDomainNavList();

				printf("<br><br><input class=largeButton type=submit name=gcCommand value='tRelay Check'>\n");
			}
	}
	CloseFieldSet();

}//void ExttDomainButtons(void)


void ExttDomainAuxTable(void)
{

}//void ExttDomainAuxTable(void)


void ExttDomainGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uDomain"))
		{
			sscanf(gentries[i].val,"%u",&uDomain);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.63s",gentries[i].val);
		}
	}
	tDomain("");

}//void ExttDomainGetHook(entry gentries[], int x)


void ExttDomainSelect(void)
{
	if(cSearch[0])
		ExtSelectSearch("tDomain",VAR_LIST_tDomain,"cDomain",cSearch,NULL,0);
	else
		ExtSelect("tDomain",VAR_LIST_tDomain,0);

}//void ExttDomainSelect(void)


void ExttDomainSelectRow(void)
{
	ExtSelectRow("tDomain",VAR_LIST_tDomain,uDomain);

}//void ExttDomainSelectRow(void)


void ExttDomainListSelect(void)
{
	char cCat[512];

	ExtListSelect("tDomain",VAR_LIST_tDomain);
	
	//Changes here must be reflected below in ExttDomainListFilter()
        if(!strcmp(gcFilter,"uDomain"))
        {
                sscanf(gcCommand,"%u",&uDomain);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tDomain.uDomain=%u \
						ORDER BY uDomain",
						uDomain);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uDomain");
        }

}//void ExttDomainListSelect(void)


void ExttDomainListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uDomain"))
                printf("<option>uDomain</option>");
        else
                printf("<option selected>uDomain</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttDomainListFilter(void)


void ExttDomainNavBar(void)
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

}//void ExttDomainNavBar(void)


void tDomainNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	unsigned uCount=0;
	printf("<p><u>tDomainvList</u><br>\n");
	
	if(!cSearch[0])
	{
        	printf("Must restrict via cSearch\n");
		return;
	}

	ExtSelectSearch("tDomain","uDomain,cDomain","cDomain",cSearch,NULL,101);

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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tDomain&uDomain=%s&cSearch=%s>%s</a><br>\n",
					field[0]
					,cURLEncode(cSearch)
					,field[1]);
			if(uCount>=100)
			{
				printf("More than 100 records: You must refine your search further<br>\n");
				break;
			}
	        }
	}
        mysql_free_result(res);

}//void tDomainNavList(void)


unsigned DomainIsInUse(unsigned uDomain, char *cDomain)
{
	MYSQL_RES *res;
	unsigned uRetVal=0;

	sprintf(gcQuery,"SELECT uUser FROM tUser WHERE uDomain=%u",uDomain);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
		uRetVal=1;
	mysql_free_result(res);
	if(uRetVal) return(uRetVal);

	sprintf(gcQuery,"SELECT uAccess FROM tAccess WHERE cDomainIP='%s'",cDomain);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
		uRetVal=1;
	mysql_free_result(res);
	if(uRetVal) return(uRetVal);

	sprintf(gcQuery,"SELECT uVUT FROM tVUT WHERE cDomain='%s'",cDomain);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
		uRetVal=1;
	mysql_free_result(res);
	if(uRetVal) return(uRetVal);

	sprintf(gcQuery,"SELECT uRelay FROM tRelay WHERE cDomain='%s'",cDomain);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
		uRetVal=1;
	mysql_free_result(res);
	if(uRetVal) return(uRetVal);

	sprintf(gcQuery,"SELECT uLocal FROM tLocal WHERE cDomain='%s'",cDomain);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res)>0)
		uRetVal=1;
	mysql_free_result(res);

	return(uRetVal);

}//unsigned DomainIsInUse()


void BasicDomainCheck(void)
{
	char *cp;

	if(!cDomain[0])
		tDomain("<blink>Error</blink>: cDomain can't be empty");

	if(strlen(cDomain)<4)
		tDomain("<blink>Error</blink>: Must provide valid cDomain");

	if((cp=strchr(cDomain,'.')))
	{
		cp++;
		if(strlen(cp)<2)
			tDomain("<blink>Error</blink>: cDomain after '.' part is too short");
	}
	else
	{
		tDomain("<blink>Error</blink>: cDomain must have '.' TLD part");
	}

	
}//void BasicDomainCheck(void)


void tDomainContextInfo(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(!cDomain[0])
		return;

	printf("<u>tUser</u><br>\n");
	sprintf(gcQuery,"SELECT uUser FROM tUser WHERE uDomain=%u",uDomain);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		printf("At least one tUser entry<br>\n");
	}
	else
	{
		printf("No tUser entries<br>\n");
	}
        mysql_free_result(res);

	printf("<u>tVUT</u><br>\n");
	sprintf(gcQuery,"SELECT uVUT,cDomain,cLabel FROM tVUT,tServerGroup WHERE tVUT.uServerGroup=tServerGroup.uServerGroup AND cDomain='%s'",cDomain);
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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tVUT&uVUT=%s>%s/%s</a><br>\n",field[0],field[1],field[2]);
	}
	else
	{
		printf("No tVUT entries<br>\n");
	}
        mysql_free_result(res);

	printf("<u>tAccess</u><br>\n");
	sprintf(gcQuery,"SELECT uAccess,cDomainIP,cLabel FROM tAccess,tServerGroup WHERE tAccess.uServerGroup=tServerGroup.uServerGroup AND cDomainIP='%s'",cDomain);
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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tAccess&uAccess=%s>%s/%s</a><br>\n",field[0],field[1],field[2]);
	}
	else
	{
		printf("No tAccess entries<br>\n");
	}
        mysql_free_result(res);

	printf("<u>tLocal</u><br>\n");
	sprintf(gcQuery,"SELECT uLocal,cDomain,cLabel FROM tLocal,tServerGroup WHERE tLocal.uServerGroup=tServerGroup.uServerGroup AND cDomain='%s'",cDomain);
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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tLocal&uLocal=%s>%s/%s</a><br>\n",field[0],field[1],field[2]);
	}
	else
	{
		printf("No tLocal entries<br>\n");
	}
        mysql_free_result(res);

	printf("<u>tRelay</u><br>\n");
	sprintf(gcQuery,"SELECT uRelay,cDomain,cLabel FROM tRelay,tServerGroup WHERE tRelay.uServerGroup=tServerGroup.uServerGroup AND cDomain='%s'",cDomain);
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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tRelay&uRelay=%s>%s/%s</a><br>\n",field[0],field[1],field[2]);
	}
	else
	{
		printf("No tRelay entries<br>\n");
	}
        mysql_free_result(res);

}//void tDomainContextInfo(void)


void AlreadyUsedHere(void)
{
	MYSQL_RES *res;
	sprintf(gcQuery,"SELECT uDomain FROM tDomain WHERE cDomain='%s'",cDomain);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
		tDomain("<blink>Error: </blink>Domain already used here!");
	mysql_free_result(res);

}//void AlreadyUsedHere(void)


void htmlRelayCheck(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	MYSQL_RES *res2;

	ExtSelect("tDomain","cDomain",0);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	printf("Content-type: text/plain\n\n");

	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"SELECT uRelay FROM tRelay WHERE cDomain='%s'",field[0]);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res2=mysql_store_result(&gMysql);

		if(!mysql_num_rows(res2))
			printf("%s has no tRelay entry\n",field[0]);

		mysql_free_result(res2);
	}

	exit(0);

}//void htmlRelayCheck(void)


void SelectuServerGroup(char *cInputName, unsigned uSelected)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uRcdServerGroup;
	
	ExtSelect("tServerGroup","tServerGroup.uServerGroup,tServerGroup.cLabel",0);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	
	printf("<select name=%s>\n",cInputName);
	printf("<option value=0>---</option>\n");
	
	while((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uRcdServerGroup);
		printf("<option value=%s ",field[0]);
		if(uSelected==uRcdServerGroup)
			printf("selected");
		printf(">%s</option>\n",field[1]);
	}

	printf("</select>\n");
	
}//void SelectuServerGroup(char *cInputName)


void AddExtraRecords(void)
{
	char cServerGroup[100]={""};
	char cJobData[100]={""};
	
	if(uAddtLocal)
	{
		unsigned uLocal=0;
		if(!uLocalServerGroup)
		{
			guMode=2000;
			tDomain("<blink>Error:</blink> You must select a server group for the tLocal record");
		}
		sprintf(gcQuery,"INSERT INTO tLocal SET cDomain='%s',uServerGroup=%u,uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",
				cDomain
				,uLocalServerGroup
				,guCompany
				,guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		
		uLocal=mysql_insert_id(&gMysql);
		sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uLocalServerGroup));
		sprintf(cJobData,"uLocal=%u;\ncDomain=%s;\n",
				uLocal,cDomain);
		SubmitJob("NewLocal",cDomain,"",cServerGroup,cJobData,0,0,
				guCompany,guLoginClient);
	}

	if(uAddtRelay)
	{
		unsigned uRelay=0;
		if(!uRelayServerGroup)
		{
			guMode=2000;
			tDomain("<blink>Error:</blink> You must select a server group for the tRelay record");
		}
		if(!cTransport[0])
		{
			guMode=2000;
			tDomain("<blink>Error:</blink> You must enter a transport server for the tRelay record");
		}
		sprintf(gcQuery,"INSERT INTO tRelay SET cDomain='%s',cTransport='%s',uServerGroup=%u,uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",
				cDomain
				,cTransport
				,uRelayServerGroup
				,guCompany
				,guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		uRelay=mysql_insert_id(&gMysql);
		
		sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uRelayServerGroup));
		sprintf(cJobData,"uRelay=%u;\ncDomain=%s;\ncTransport=%s;\n",
				uRelay,cDomain,cTransport);
		SubmitJob("NewRelay",cDomain,"",cServerGroup,cJobData,0,0,
				guCompany,guLoginClient);
	}

	if(uAddtVUT)
	{
		//We use same uServerGroup than for tLocal. Which is logic ;)
		sprintf(gcQuery,"INSERT INTO tVUT SET cDomain='%s',uServerGroup=%u,uOwner=%u,uCreatedBy=%u,"
				"uCreatedDate=UNIX_TIMESTAMP(NOW())",
				cDomain
				,uLocalServerGroup
				,guCompany
				,guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
			
}//void AddExtraRecords(void)


void UpdateDependencies(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	char cOldDomain[100]={""};
	char cJobData[100]={""};
	char cServerGroup[100]={""};

	unsigned uServerGroup=0;

	sprintf(cOldDomain,"%.99s",ForeignKey("tDomain","cDomain",uDomain));

	if(!cOldDomain[0]) return;
	
	//Update tLocal, tRelay and tVUT records.
	//Please note that a job has to be submitted for all these three tables!

	sprintf(gcQuery,"SELECT uLocal,uServerGroup FROM tLocal WHERE cDomain='%s'",cOldDomain);
	macro_mySQLRunAndStore(res);
	if((field=mysql_fetch_row(res)))
	{
		unsigned uLocal=0;
		unsigned uServerGroup=0;
		char cJobData[100]={""};
		char cServerGroup[100]={""};

		sscanf(field[0],"%u",&uLocal);
		sscanf(field[1],"%u",&uServerGroup);

		sprintf(gcQuery,"UPDATE tLocal SET cDomain='%s' WHERE cDomain='%s'",
			TextAreaSave(cDomain),cOldDomain);
		macro_mySQLQueryHTMLError;
		//Submit job ModLocal
		sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
		sprintf(cJobData,"uLocal=%u;\ncDomain=%s;\n",
				uLocal,cDomain);
		SubmitJob("ModLocal",cDomain,"",cServerGroup,cJobData,0,0,
				guCompany,guLoginClient);
	}
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT uRelay,uServerGroup FROM tRelay WHERE cDomain='%s'",cOldDomain);
	macro_mySQLRunAndStore(res);
	if((field=mysql_fetch_row(res)))
	{
		unsigned uRelay=0;
		
		sscanf(field[0],"%u",&uRelay);
		sscanf(field[1],"%u",&uServerGroup);

		//Update tRelay
		sprintf(gcQuery,"UPDATE tRelay SET cDomain='%s' WHERE cDomain='%s'",
			TextAreaSave(cDomain),cOldDomain);
		macro_mySQLQueryHTMLError;
		//Submit job ModRelay
		sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
		sprintf(cJobData,"uRelay=%u;\ncDomain=%s;\ncTransport=%s;\n",
				uRelay,cDomain,cTransport);
		SubmitJob("ModRelay",cDomain,"",cServerGroup,cJobData,0,0,
				guCompany,guLoginClient);
	}
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT uVUT FROM tVUT WHERE cDomain='%s'",cOldDomain);
	macro_mySQLRunAndStore(res);
	if((field=mysql_fetch_row(res)))
	{
		unsigned uVUT=0;
		
		sscanf(field[0],"%u",&uVUT);
		sscanf(field[1],"%u",&uServerGroup);

		//Update tVUT
		sprintf(gcQuery,"UPDATE tVUT SET cDomain='%s' WHERE cDomain='%s'",
			TextAreaSave(cDomain),cOldDomain);
		macro_mySQLQueryHTMLError;
		//Submit job ModLocal
		sscanf(ForeignKey("tVUT","uServerGroup",uVUT),"%u",&uServerGroup);
		sprintf(cDomain,ForeignKey("tVUT","cDomain",uVUT));
		sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
		sprintf(cJobData,"uVUTEntries=0;\ncVirtualEmail=tDomain dep change;\ncTargetEmail=;\ncDomain=%s;\n",
				cDomain);
		SubmitJob("ModVUT",cDomain,"tDomain dep change",cServerGroup,cJobData,0,0,
					guCompany,guLoginClient);
	}
	mysql_free_result(res);

}//void UpdateDependencies(void)

