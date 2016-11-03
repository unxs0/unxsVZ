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

//Local
void tAliasNavList(void);
void BasicAliasCheck(void);
void tAliasContextInfo(void);


void ExtProcesstAliasVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",entries[i].val);
	}
}//void ExtProcesstAliasVars(pentry entries[], int x)


void ExttAliasCommands(pentry entries[], int x)
{
	char cServerGroup[100]={""};
	char cJobData[100]={""};

	if(!strcmp(gcFunction,"tAliasTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstAliasVars(entries,x);
                        	guMode=2000;
	                        tAlias(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstAliasVars(entries,x);

                        	guMode=2000;
				//Check entries here
				BasicAliasCheck();
                        	guMode=0;

				uAlias=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtAlias(1);
				if(!uAlias)
					htmlPlainTextError("Unexpected NewtAlias() error");
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uAlias=%u;\ncUser=%s;\ncTargetEmail=%s;\n",
						uAlias,cUser,cTargetEmail);
				SubmitJob("NewAlias","",cUser,cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				tAlias("New tAlias record created. Job(s) created");
			}
			else
				tAlias("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstAliasVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tAlias(LANG_NB_CONFIRMDEL);
			}
			else
				tAlias("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstAliasVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				BasicAliasCheck();

				guMode=5;
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uAlias=%u;\ncUser=%s;\ncTargetEmail=%s;\n",
						uAlias,cUser,cTargetEmail);
				SubmitJob("DelAlias","",cUser,cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				DeletetAlias();
			}
			else
				tAlias("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstAliasVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tAlias(LANG_NB_CONFIRMMOD);
			}
			else
				tAlias("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstAliasVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				BasicAliasCheck();
                        	guMode=0;

				uModBy=guLoginClient;
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uAlias=%u;\ncUser=%s;\ncTargetEmail=%s;\n",
						uAlias,cUser,cTargetEmail);
				SubmitJob("ModAlias","",cUser,cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				ModtAlias();
			}
			else
				tAlias("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttAliasCommands(pentry entries[], int x)


void ExttAliasButtons(void)
{
	OpenFieldSet("tAlias Aux Panel",100);
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
			 printf("This is the infamous sendmail alias or other similar configuration file for most ESMTP software. "
				"Use the cSearch box to generate a list of links. This table is by nature multi-domain capable, "
				"each alias file instance is per server via the server group.");
			if(uAlias)
			{
				printf("<p><u>Record Context Info</u><br>");
				tAliasContextInfo();

				printf("<p><input title='Enter a mySQL search pattern for cUser or cTargetEmail' "
					"type=text name=cSearch value='%s' maxlength=63 size=20> cSearch\n",
					cSearch);
				tAliasNavList();
			}
	}
	CloseFieldSet();

}//void ExttAliasButtons(void)


void ExttAliasAuxTable(void)
{

}//void ExttAliasAuxTable(void)


void ExttAliasGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uAlias"))
		{
			sscanf(gentries[i].val,"%u",&uAlias);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.63s",gentries[i].val);
		}
	}
	tAlias("");

}//void ExttAliasGetHook(entry gentries[], int x)


void ExttAliasSelect(void)
{
	if(cSearch[0])
		ExtSelectSearch("tAlias",VAR_LIST_tAlias,"cUser",cSearch,NULL,0);
	else
		ExtSelect("tAlias",VAR_LIST_tAlias,0);

}//void ExttAliasSelect(void)


void ExttAliasSelectRow(void)
{
	ExtSelectRow("tAlias",VAR_LIST_tAlias,uAlias);

}//void ExttAliasSelectRow(void)


void ExttAliasListSelect(void)
{
	char cCat[512];

	ExtListSelect("tAlias",VAR_LIST_tAlias);
	
	//Changes here must be reflected below in ExttAliasListFilter()
        if(!strcmp(gcFilter,"uAlias"))
        {
                sscanf(gcCommand,"%u",&uAlias);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tAlias.uAlias=%u \
						ORDER BY uAlias",
						uAlias);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uAlias");
        }

}//void ExttAliasListSelect(void)


void ExttAliasListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uAlias"))
                printf("<option>uAlias</option>");
        else
                printf("<option selected>uAlias</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttAliasListFilter(void)


void ExttAliasNavBar(void)
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

}//void ExttAliasNavBar(void)


void tAliasNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;

	printf("<p><u>tAliasNavList</u><br>\n");
	
	if(!cSearch[0])
	{
        	printf("Must restrict via cSearch\n");
		return;
	}


	sprintf(gcQuery,"SELECT uAlias,cUser FROM tAlias WHERE cUser LIKE '%s%%' OR cTargetEmail LIKE '%s%%' ORDER BY cUser LIMIT 101",cSearch,cSearch);

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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tAlias&uAlias=%s&cSearch=%s>%s</a><br>\n",
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

}//void tAliasNavList(void)


void BasicAliasCheck(void)
{
	if(!cTargetEmail[0])
		tAlias("<blink>Error: </blink>cTargetEmail is required");
	if(!isalpha(cTargetEmail[0]))
		tAlias("<blink>Error: </blink>cTargetEmail must not start with a number");
	if(strlen(cTargetEmail)<3)
		tAlias("<blink>Error: </blink>cTargetEmail must be at least 3 chars");
	if(strstr(cTargetEmail,".."))
		tAlias("<blink>Error: </blink>cTargetEmail may not have two consecutive periods");

	if(!cUser[0])
		tAlias("<blink>cUser is required");
	if(!isalpha(cUser[0]))
		tAlias("<blink>Error: </blink>cUser must not start with a number");
	if(strlen(cUser)<3)
		tAlias("<blink>Error: </blink>cUser must be at least 3 chars");
	if(strstr(cUser,".."))
		tAlias("<blink>Error: </blink>cUser may not have two consecutive periods");

	if(!uServerGroup)
		tAlias("<blink>Error: </blink>uServerGroup is required");
}//void BasicAliasCheck(void)


void tAliasContextInfo(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(!cUser[0])
		return;

	printf("<u>Users</u><br>\n");
	sprintf(gcQuery,"SELECT tUser.uUser,tUser.cLogin,tServerGroup.cLabel FROM tUser,tServerGroup WHERE tUser.uServerGroup=tServerGroup.uServerGroup AND tUser.uServerGroup=%u AND (tUser.cLogin='%s' OR '%s' LIKE CONCAT('%%',tUser.cLogin,'%%'))",uServerGroup,cUser,cTargetEmail);
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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tUser&uUser=%s>%s/%s</a><br>\n",
						field[0],field[1],field[2]);
	}
	else
	{
		printf("No users<br>\n");
	}
        mysql_free_result(res);

	printf("<u>Jobs</u><br>\n");
	sprintf(gcQuery,"SELECT tJob.uJob,tJob.cJobName,tJob.cServer FROM tJob WHERE tJob.cLabel LIKE '%%Alias%%%s%%' AND tJob.uJobStatus!=3",
						cUser);
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

}//void tAliasContextInfo(void)

