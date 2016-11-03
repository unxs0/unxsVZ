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

//ModuleFunctionProtos()
static char cSearch[64]={""};

//Extern
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);
//Local
void tAccessNavList(void);
void BasicAccessCheck(void);
void tAccessContextInfo(void);

void ExtProcesstAccessVars(pentry entries[], int x)
{
	
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",entries[i].val);
	}
	
}//void ExtProcesstAccessVars(pentry entries[], int x)


void ExttAccessCommands(pentry entries[], int x)
{
	char cServerGroup[100]={""};
	char cJobData[100]={""};
	
	if(!strcmp(gcFunction,"tAccessTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstAccessVars(entries,x);
				uExpireDate=0;
				uSource=1;
                        	guMode=2000;
	                        tAccess(LANG_NB_CONFIRMNEW);
			}
			else
				tAccess("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstAccessVars(entries,x);

                        	guMode=2000;
				//Check entries here
				BasicAccessCheck();
                        	guMode=0;

				uAccess=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtAccess(1);

				if(!uAccess)
					htmlPlainTextError("Unexpected NewtAlias() error");
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uAccess=%u;\ncDomainIP=%s;\ncRelayAttr=%s;\n",
						uAccess,cDomainIP,cRelayAttr);
				SubmitJob("NewAccess",cDomainIP,cRelayAttr,cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				tAccess("New tAccess record created. Job(s) created");
				
			}
			else
				tAccess("<blink>Error</blink>: Denied by permissions settings");
			
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstAccessVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tAccess(LANG_NB_CONFIRMDEL);
			}
			else
				tAccess("<blink>Error</blink>: Denied by permissions settings");
			
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstAccessVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uAccess=%u;\ncDomainIP=%s;\ncRelayAttr=%s;\n",
						uAccess,cDomainIP,cRelayAttr);
				SubmitJob("DelAccess",cDomainIP,cRelayAttr,cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				DeletetAccess();
			}
			else
				tAccess("<blink>Error</blink>: Denied by permissions settings");
			
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstAccessVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tAccess(LANG_NB_CONFIRMMOD);
			}
			else
				tAccess("<blink>Error</blink>: Denied by permissions settings");
			
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstAccessVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				BasicAccessCheck();
                        	guMode=0;

				uModBy=guLoginClient;

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uAccess=%u;\ncDomainIP=%s;\ncRelayAttr=%s;\n",
						uAccess,cDomainIP,cRelayAttr);
				SubmitJob("ModAccess",cDomainIP,cRelayAttr,cServerGroup,cJobData,0,0,
								guCompany,guLoginClient);
				ModtAccess();
			}
			else
				tAccess("<blink>Error</blink>: Denied by permissions settings");
			
                }
	}

}//void ExttAccessCommands(pentry entries[], int x)


void ExttAccessButtons(void)
{
	OpenFieldSet("tAccess Aux Panel",100);
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
			if(uAccess)
			{
				printf("<p><u>Record Context Info</u><br>");
				tAccessContextInfo();
				printf("<p><input title='Enter a mySQL search pattern for cDomainIP' type=text name=cSearch value='%s' maxlength=63 size=20> cSearch \n",cSearch);
				tAccessNavList();
			}
	}
	CloseFieldSet();

}//void ExttAccessButtons(void)


void ExttAccessAuxTable(void)
{

}//void ExttAccessAuxTable(void)


void ExttAccessGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uAccess"))
		{
			sscanf(gentries[i].val,"%u",&uAccess);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.63s",gentries[i].val);
		}
	}
	tAccess("");

}//void ExttAccessGetHook(entry gentries[], int x)


void ExttAccessSelect(void)
{
	if(cSearch[0])
		ExtSelectSearch("tAccess",VAR_LIST_tAccess,"cDomainIP",cSearch,NULL,101);
	else
		ExtSelect("tAccess",VAR_LIST_tAccess,0);

}//void ExttAccessSelect(void)


void ExttAccessSelectRow(void)
{
	ExtSelectRow("tAccess",VAR_LIST_tAccess,uAccess);

}//void ExttAccessSelectRow(void)


void ExttAccessListSelect(void)
{
	char cCat[512];

	ExtListSelect("tAccess",VAR_LIST_tAccess);
	
	//Changes here must be reflected below in ExttAccessListFilter()
        if(!strcmp(gcFilter,"uAccess"))
        {
                sscanf(gcCommand,"%u",&uAccess);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tAccess.uAccess=%u \
						ORDER BY uAccess",
						uAccess);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uAccess");
        }

}//void ExttAccessListSelect(void)


void ExttAccessListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uAccess"))
                printf("<option>uAccess</option>");
        else
                printf("<option selected>uAccess</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttAccessListFilter(void)


void ExttAccessNavBar(void)
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

}//void ExttAccessNavBar(void)


void tAccessNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;

	printf("<p><u>tAccessNavList</u><br>\n");
	
	if(!cSearch[0])
	{
        	printf("Must restrict via cSearch\n");
		return;
	}


	ExtSelectSearch("tAccess","uAccess,cDomainIP","cDomainIP",cSearch,NULL,101);
	
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
printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tAccess&uAccess=%s&cSearch=%s>%s</a><br>\n",field[0],cURLEncode(cSearch),field[1]);
			if(uCount>=100)
			{
				printf("More than 100 records: You must refine your search further<br>\n");
				break;
			}
	        }
	}
        mysql_free_result(res);

}//void tAccessNavList(void)


void BasicAccessCheck(void)
{
	register int i;
	unsigned a= -1,b= -1,c= -1,d= -1;
	char *cp;
	

	if(!uServerGroup)
		tAccess("<blink>Error: </blink>Must select uServerGroup");

	if(strlen(cDomainIP)<4)
		tAccess("<blink>Error: </blink>Must specify a valid cDomainIP");
	if(strlen(cRelayAttr)<5)
		tAccess("<blink>Error: </blink>Must specify a valid cRelayAttr");
	
	if(!uServerGroup)
		tAccess("<blink>Error: </blink>Must select uServerGroup");
	
	if(cDomainIP[strlen(cDomainIP)-1]=='.')
		tAccess("cDomainIP incorrect format, ends with '.'");


	sscanf(cDomainIP,"%u.%u.%u.%u",&a,&b,&c,&d);
	if(a>=0 && a<255 && b>=0 && b<255 && c>=0 && c<255 )
	{
		if(d == -1 )
			sprintf(cDomainIP,"%u.%u.%u",a,b,c);
		else if(d>=0 && d<255)
			sprintf(cDomainIP,"%u.%u.%u.%u",a,b,c,d);
		else if(1)
			tAccess("cDomainIP looks like an IP but is formatted wrong.");
	}
	else
	{
		if((cp=strchr(cDomainIP,'.')))
		{
			cp++;
			if(strlen(cp)<2)
				tAccess("cDomainIP after '.' part is too short");
		}
		else
		{
			tAccess("cDomainIP must have '.' TLD part");
		}
	}
	
	for(i=0;cDomainIP[i] && i<99;i++)
		if(isupper(cDomainIP[i]))
			cDomainIP[i]=tolower(cDomainIP[i]);
	
}//void BasicAccessCheck(void)


void tAccessContextInfo(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	printf("<u>Jobs</u><br>\n");
	sprintf(gcQuery,"SELECT tJob.uJob,tJob.cJobName,tJob.cServer FROM tJob WHERE tJob.cLabel LIKE '%%Access%%%s%%' AND tJob.uJobStatus!=3",
						cDomainIP);
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

}//void tAccessContextInfo(void)

