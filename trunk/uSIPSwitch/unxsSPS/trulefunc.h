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


static unsigned uTimeInterval=0;
static char cuTimeIntervalPullDown[256]={""};
static unsigned uGateway=0;
static char cuGatewayPullDown[256]={""};

void tRuleNavList(void);
void tRuleGroupGlueNavList(void);

void ExtProcesstRuleVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uTimeInterval"))
			sscanf(entries[i].val,"%u",&uTimeInterval);
		else if(!strcmp(entries[i].name,"cuTimeIntervalPullDown"))
		{
			sprintf(cuTimeIntervalPullDown,"%.255s",entries[i].val);
			uTimeInterval=ReadPullDown("tTimeInterval","cLabel",cuTimeIntervalPullDown);
		}
		if(!strcmp(entries[i].name,"uGateway"))
			sscanf(entries[i].val,"%u",&uGateway);
		else if(!strcmp(entries[i].name,"cuGatewayPullDown"))
		{
			sprintf(cuGatewayPullDown,"%.255s",entries[i].val);
			uGateway=ReadPullDown("tGateway","cLabel",cuGatewayPullDown);
		}
	}
}//void ExtProcesstRuleVars(pentry entries[], int x)


void ExttRuleCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tRuleTools"))
	{
		
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstRuleVars(entries,x);
                        	guMode=2000;
	                        tRule(LANG_NB_CONFIRMNEW);
			}
			else
				tRule("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstRuleVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uRule=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtRule(0);
			}
			else
				tRule("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstRuleVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tRule(LANG_NB_CONFIRMDEL);
			}
			else
				tRule("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstRuleVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetRule();
			}
			else
				tRule("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstRuleVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tRule(LANG_NB_CONFIRMMOD);
			}
			else
				tRule("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstRuleVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;

				uModBy=guLoginClient;
				ModtRule();
			}
			else
				tRule("<blink>Error</blink>: Denied by permissions settings");
                }
		//Custom operations
		else if(!strcmp(gcCommand,"Add GW"))
                {
                        ProcesstRuleVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) && uRule)
			{
				if(uGateway)
				{
        				MYSQL_RES *res;

					sprintf(gcQuery,"SELECT uGroupGlue FROM tGroupGlue WHERE uKey=%u"
							" AND uGroupType=(SELECT uGroupType FROM tGroupType WHERE cLabel='tRule:tGateway' LIMIT 1)"
							" AND uGroup=%u",
								uGateway,uRule);
        				mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				tRule(gcQuery);
				        res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)>0)
						tRule("Gateway has already been added");
					sprintf(gcQuery,"INSERT INTO tGroupGlue SET uKey=%u,"
							"uGroupType=(SELECT uGroupType FROM tGroupType WHERE cLabel='tRule:tGateway' LIMIT 1),"
							"uGroup=%u",uGateway,uRule);
        				mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				tRule(gcQuery);
					tRule("Gateway added");
				}
				tRule("No key selected");
			}
			else
				tRule("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Del GW"))
                {
                        ProcesstRuleVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) && uRule)
			{
				if(uGateway)
				{
					sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uKey=%u AND uGroup=%u"
						" AND uGroupType=(SELECT uGroupType FROM tGroupType WHERE cLabel='tRule:tGateway' LIMIT 1)",
							uGateway,uRule);
        				mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				tRule(gcQuery);
					tRule("Gateway deleted");
				}
				tRule("No key selected");
			}
			else
				tRule("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Add TI"))
                {
                        ProcesstRuleVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) && uRule)
			{
				if(uTimeInterval)
				{
        				MYSQL_RES *res;

					sprintf(gcQuery,"SELECT uGroupGlue FROM tGroupGlue WHERE uKey=%u"
							" AND uGroupType=(SELECT uGroupType FROM tGroupType WHERE cLabel='tRule:tTimeInterval' LIMIT 1)"
							" AND uGroup=%u",
								uTimeInterval,uRule);
        				mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				tRule(gcQuery);
				        res=mysql_store_result(&gMysql);
					if(mysql_num_rows(res)>0)
						tRule("Time interval has already been added");
					sprintf(gcQuery,"INSERT INTO tGroupGlue SET uKey=%u,"
							"uGroupType=(SELECT uGroupType FROM tGroupType WHERE cLabel='tRule:tTimeInterval' LIMIT 1),"
							"uGroup=%u",uTimeInterval,uRule);
        				mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				tRule(gcQuery);
					tRule("Time interval added");
				}
				tRule("No key selected");
			}
			else
				tRule("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Del TI"))
                {
                        ProcesstRuleVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) && uRule)
			{
				if(uTimeInterval)
				{
					sprintf(gcQuery,"DELETE FROM tGroupGlue WHERE uKey=%u AND uGroup=%u"
						" AND uGroupType=(SELECT uGroupType FROM tGroupType WHERE cLabel='tRule:tTimeInterval' LIMIT 1)",
							uTimeInterval,uRule);
        				mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
                				tRule(gcQuery);
					tRule("Time interval deleted");
				}
				tRule("No key selected");
			}
			else
				tRule("<blink>Error</blink>: Denied by permissions settings");
		}
	}

}//void ExttRuleCommands(pentry entries[], int x)


void ExttRuleButtons(void)
{
	OpenFieldSet("tRule Aux Panel",100);
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
			printf("Build a rule by adding at least one gateway. The default rule should be called that.");
			if(uRule)
			{
				tRuleGroupGlueNavList();
				printf("<p><u><a class=darkLink href=unxsSPS.cgi?gcFunction=tTimeInterval>Add time interval to this rule</a></u><br>");
				tTablePullDown("tTimeInterval;cuTimeIntervalPullDown","cLabel","cLabel",uTimeInterval,1);
				printf("<br><input type=submit class=largeButton title='Add time interval record selected to current rule'"
					"name=gcCommand value='Add TI'>");
				printf("<br><input type=submit class=largeButton title='Del time interval record selected from current group'"
					"name=gcCommand value='Del TI'>");

				printf("<p><u>Add gateway to this rule</u><br>");
				tTablePullDown("tGateway;cuGatewayPullDown","cLabel","cLabel",uGateway,1);
				printf("<br><input type=submit class=largeButton title='Add time interval record selected to current rule'"
					"name=gcCommand value='Add GW'>");
				printf("<br><input type=submit class=largeButton title='Del time interval record selected from current group'"
					"name=gcCommand value='Del GW'>");
			}
			tRuleNavList();
	}
	CloseFieldSet();

}//void ExttRuleButtons(void)


void ExttRuleAuxTable(void)
{

}//void ExttRuleAuxTable(void)


void ExttRuleGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uRule"))
		{
			sscanf(gentries[i].val,"%u",&uRule);
			guMode=6;
		}
	}
	tRule("");

}//void ExttRuleGetHook(entry gentries[], int x)


void ExttRuleSelect(void)
{

#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tRule ORDER BY"
				" uRule",
				VAR_LIST_tRule);
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
		//select tTemplateSet.cLabel from tTemplateSet,tClient where tTemplateSet.uOwner=tClient.uClient and tClient.uOwner in (select uClient from tClient where uOwner=81 or uClient=51);
	sprintf(gcQuery,"SELECT %s FROM tRule,tClient WHERE tRule.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uRule",
					VAR_LIST_tRule,uContactParentCompany,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tRule ORDER BY uRule",VAR_LIST_tRule);
#endif
					

}//void ExttRuleSelect(void)


void ExttRuleSelectRow(void)
{
#ifdef StandardFields
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
                sprintf(gcQuery,"SELECT %s FROM tRule WHERE uRule=%u",
			VAR_LIST_tRule,uRule);
	else
                sprintf(gcQuery,"SELECT %s FROM tRule,tClient"
                                " WHERE tRule.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tRule.uRule=%u",
                        		VAR_LIST_tRule
					,uContactParentCompany,uContactParentCompany
					,uRule);
#else
	sprintf(gcQuery,"SELECT %s FROM tRule WHERE uRule=%u",VAR_LIST_tRule,uRule);
#endif

}//void ExttRuleSelectRow(void)


void ExttRuleListSelect(void)
{
	char cCat[512];
#ifdef StandardFields
	unsigned uContactParentCompany=0;
	
	GetClientOwner(guLoginClient,&uContactParentCompany);

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tRule",
				VAR_LIST_tRule);
	else
		sprintf(gcQuery,"SELECT %s FROM tRule,tClient"
				" WHERE tRule.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tRule
				,uContactParentCompany
				,uContactParentCompany);
#else
	sprintf(gcQuery,"SELECT %s FROM tRule",VAR_LIST_tRule);
#endif

	//Changes here must be reflected below in ExttRuleListFilter()
        if(!strcmp(gcFilter,"uRule"))
        {
                sscanf(gcCommand,"%u",&uRule);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tRule.uRule=%u"
						" ORDER BY uRule",
						uRule);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uRule");
        }

}//void ExttRuleListSelect(void)


void ExttRuleListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uRule"))
                printf("<option>uRule</option>");
        else
                printf("<option selected>uRule</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttRuleListFilter(void)


void ExttRuleNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

#ifdef StandardFields
	if(uAllowMod(uOwner,uCreatedBy))
#else
	if(guPermLevel>=9)
#endif
		printf(LANG_NBB_MODIFY);

#ifdef StandardFields
	if(uAllowDel(uOwner,uCreatedBy)) 
#else
	if(guPermLevel>=9)
#endif
		printf(LANG_NBB_DELETE);

#ifdef StandardFields
	if(uOwner)
#else
	if(guPermLevel>=9)
#endif
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttRuleNavBar(void)


void tRuleNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT uRule,cLabel,cPrefix FROM tRule ORDER BY cLabel");
	else
		sprintf(gcQuery,"SELECT tRule.uRule,"
				" tRule.cLabel,tRule.cPrefix"
				" FROM tRule,tClient"
				" WHERE tRule.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
					guCompany
					,guCompany);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tRuleNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tRuleNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tRule"
				"&uRule=%s>%s/%s</a><br>\n",
				field[0],field[1],field[2]);
	}
        mysql_free_result(res);

}//void tRuleNavList(void)


void tRuleGroupGlueNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_ROW field2;

	sprintf(gcQuery,"SELECT tTimeInterval.uTimeInterval,tTimeInterval.cLabel FROM tGroupGlue,tTimeInterval WHERE tGroupGlue.uGroup=%u"
			" AND tGroupGlue.uKey=tTimeInterval.uTimeInterval"
			" AND uGroupType=(SELECT uGroupType FROM tGroupType WHERE cLabel='tRule:tTimeInterval' LIMIT 1)",uRule);
        mysql_query(&gMysql,gcQuery);
        printf("<p><u>Intervals</u><br>\n");
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tTimeInterval"
				"&uTimeInterval=%s>%s</a><br>\n",
				field[0],field[1]);
	}
	else
	{
        	printf("No intervals. Rule will not be loaded!\n");
	}

	sprintf(gcQuery,"SELECT tGateway.uGateway,tGateway.cLabel FROM tGroupGlue,tGateway WHERE tGroupGlue.uGroup=%u"
			" AND tGroupGlue.uKey=tGateway.uGateway"
			" AND uGroupType=(SELECT uGroupType FROM tGroupType WHERE cLabel='tRule:tGateway' LIMIT 1)",uRule);
        mysql_query(&gMysql,gcQuery);
        printf("<p><u>Gateways</u><br>\n");
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
			printf("<a class=darkLink href=unxsSPS.cgi?gcFunction=tGateway"
				"&uGateway=%s>%s</a><br>\n",
				field[0],field[1]);

			sprintf(gcQuery,"SELECT uAddress,cIP,uPort FROM tAddress WHERE uGateway=%s",field[0]);
        		mysql_query(&gMysql,gcQuery);
        		if(mysql_errno(&gMysql))
        		{
                		printf("%s",mysql_error(&gMysql));
                		return;
        		}
        		res2=mysql_store_result(&gMysql);
	        	while((field2=mysql_fetch_row(res2)))
			{
				printf(" &nbsp; <a class=darkLink href=unxsSPS.cgi?gcFunction=tAddress"
				"&uAddress=%s>%s/%s</a><br>\n",
				field2[0],field2[1],field2[2]);
			}
		}
	}
	else
	{
        	printf("No gateways. Rule will not be loaded!\n");
	}
        mysql_free_result(res);

}//void tRuleGroupGlueNavList(void)

