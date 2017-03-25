/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2010 Unixservice, LLC.
 
*/

void tConfigNavList(void);
unsigned htmlConfigContext(void);
unsigned LocalImportConfigJob(unsigned uConfig);

//file scoped var
static unsigned uDatacenter=0;
static char cuDatacenterPullDown[256]={""};


void ExtProcesstConfigVars(pentry entries[], int x)
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
}//void ExtProcesstConfigVars(pentry entries[], int x)


void ExttConfigCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tConfigTools"))
	{
        	MYSQL_RES *res;
		time_t uActualModDate= -1;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstConfigVars(entries,x);
                        	guMode=2000;
	                        tConfig(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstConfigVars(entries,x);

                        	guMode=2000;
				//Check entries here
				if(strlen(cLabel)<3)
					tConfig("<blink>Error</blink>: cLabel too short!");
				sprintf(gcQuery,"SELECT uConfig FROM tConfig WHERE cLabel='%s'",
						cLabel);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tConfig("<blink>Error</blink>: tConfig.cLabel is already used!");
				}
                        	guMode=0;

				uConfig=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtConfig(1);
				if(!uConfig)
					tConfig("<blink>Error</blink>: New tConfig entry was not created!");

				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType="PROP_CONFIG
						",cName='cDatacenter',cValue='All Datacenters',uOwner=%u,uCreatedBy=%u"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,uConfig,guCompany,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				tConfig("New config created");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstConfigVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=0;
				sprintf(gcQuery,"SELECT uConfig FROM tContainer WHERE uConfig=%u",uConfig);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tConfig("<blink>Error</blink>: Can't delete a config used by a container!");
				}
	                        guMode=2001;
				tConfig(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstConfigVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				sscanf(ForeignKey("tConfig","uModDate",uConfig),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tConfig("<blink>Error</blink>: This record was modified. Reload it.");
				sprintf(gcQuery,"SELECT uConfig FROM tContainer WHERE uConfig=%u",uConfig);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tConfig("<blink>Error</blink>: Can't delete a config used by a container!");
				}
				guMode=5;
				DeletetConfig();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tConfig(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				sscanf(ForeignKey("tConfig","uModDate",uConfig),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tConfig("<blink>Error</blink>: This record was modified. Reload it.");
				if(strlen(cLabel)<3)
					tConfig("<blink>Error</blink>: cLabel too short!");
				sprintf(gcQuery,"SELECT uConfig FROM tContainer WHERE uConfig=%u",uConfig);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tConfig("<blink>Error</blink>: Can't modify a config used by a container!");
				}
                        	guMode=0;

				uModBy=guLoginClient;
				ModtConfig();
			}
                }
		else if(!strcmp(gcCommand,"Local Import"))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstConfigVars(entries,x);
				guMode=6;
				if(uConfig<1)
					tConfig("<blink>Error</blink>: Unexpected uConfig&lt;1 error!");
				sscanf(ForeignKey("tConfig","uModDate",uConfig),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tConfig("<blink>Error</blink>: This record was modified. Reload it.");
				sprintf(gcQuery,"SELECT uConfig FROM tContainer WHERE uConfig=%u",
						uConfig);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tConfig("<blink>Error</blink>: Can't import a config already in use!");
				}
				if(strlen(cLabel)<3)
					tConfig("<blink>Error</blink>: cLabel too short!");
				
				if(LocalImportConfigJob(uConfig))
					tConfig("Local Import job created");
				else
					tConfig("<blink>Error</blink>: LocalImportTemplateJob() failed!");
			}
		}
                else if(!strcmp(gcCommand,"Enable"))
                {
                        ProcesstConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=6;
				sscanf(ForeignKey("tConfig","uModDate",uConfig),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tConfig("<blink>Error</blink>: This record was modified. Reload it.");
				if(!uDatacenter)
					sprintf(gcQuery,"INSERT tProperty SET cName='cDatacenter',cValue='All Datacenters',"
						"uType=%u,uKey=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uPROP_CONFIG,uConfig,uOwner,guLoginClient);
				else
					sprintf(gcQuery,"INSERT tProperty SET cName='cDatacenter',cValue='%s',"
						"uType=%u,uKey=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						ForeignKey("tDatacenter","cLabel",uDatacenter),	
						uPROP_CONFIG,uConfig,uOwner,guLoginClient);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
			}
			else
			{
				tConfig("<blink>Error</blink>: Enable not allowed!");
			}
		}
                else if(!strcmp(gcCommand,"Disable"))
                {
                        ProcesstConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=6;
				sscanf(ForeignKey("tConfig","uModDate",uConfig),"%lu",&uActualModDate);
				if(uModDate!=uActualModDate)
					tConfig("<blink>Error</blink>: This record was modified. Reload it.");
				if(!uDatacenter)
					sprintf(gcQuery,"DELETE FROM tProperty WHERE cName='cDatacenter' AND cValue='All Datacenters'"
						" AND uType=%u AND uKey=%u AND (uOwner=%u OR uCreatedBy=%u)",
						uPROP_CONFIG,uConfig,uOwner,guLoginClient);
				else
					sprintf(gcQuery,"DELETE FROM tProperty WHERE cName='cDatacenter' AND cValue='%s'"
						" AND uType=%u AND uKey=%u AND (uOwner=%u OR uCreatedBy=%u)",
						ForeignKey("tDatacenter","cLabel",uDatacenter),
						uPROP_CONFIG,uConfig,uOwner,guLoginClient);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
			}
			else
			{
				tConfig("<blink>Error</blink>: Disable not allowed!");
			}
		}
	}

}//void ExttConfigCommands(pentry entries[], int x)


void ExttConfigButtons(void)
{
	unsigned uNum=0;

	OpenFieldSet("tConfig Aux Panel",100);
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
			//ve-dns2.conf-sample
			printf("<u>Table Tips (%s)</u><br>",cGitVersion);
			printf("Make sure that the cLabel entries (new or modified) have a corresponding "
				"/etc/vz/conf//ve-&lt;cLabel&gt;.conf-sample file on all unxsVZ controlled "
				"nodes. Please note the 32 character limitation."
				"<p><u>Automated way to install an external config file</u><br>"
				"1-. Transfer ve-&lt;unused tConfig.cLabel&gt;.conf-sample to a node with a local "
				"unxsVZ webmin and place in /etc/vz/conf/.<br>"
				"2-. Run a [Local Import] job, after adding the new tConfig.cLabel.<br>");
			printf("<p><u>Record Context Info</u><br>");
			uNum=htmlConfigContext();
			tConfigNavList();

			if(uConfig>0 && uNum==0)
			{
				printf("<p><u>Extended table actions</u><br>");
                        	printf("<input class=largeButton title='Submit a job for the local import of the currently selected "
					"config' type=submit name=gcCommand value='Local Import'>");
			}
	}
	CloseFieldSet();

}//void ExttConfigButtons(void)


void ExttConfigAuxTable(void)
{
	if(!uConfig || guMode==2000 )//uMODE_NEW
		return;

        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"tConfig %s Property Panel",cLabel);
	OpenFieldSet(gcQuery,100);
	sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE uKey=%u AND uType="PROP_CONFIG
			" ORDER BY cName",uConfig);

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
					"gcFunction=tProperty&uProperty=%s&cReturn=tConfig_%u>"
					"%s</a></td><td>%s</td>\n",
						field[0],uConfig,field[1],field[2]);
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

}//void ExttConfigAuxTable(void)


void ExttConfigGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uConfig"))
		{
			sscanf(gentries[i].val,"%u",&uConfig);
			guMode=6;
		}
	}
	tConfig("");

}//void ExttConfigGetHook(entry gentries[], int x)


void ExttConfigSelect(void)
{
	ExtSelect("tConfig",VAR_LIST_tConfig);

}//void ExttConfigSelect(void)


void ExttConfigSelectRow(void)
{
	ExtSelectRow("tConfig",VAR_LIST_tConfig,uConfig);

}//void ExttConfigSelectRow(void)


void ExttConfigListSelect(void)
{
	char cCat[512];

	ExtListSelect("tConfig",VAR_LIST_tConfig);

	//Changes here must be reflected below in ExttConfigListFilter()
        if(!strcmp(gcFilter,"uConfig"))
        {
                sscanf(gcCommand,"%u",&uConfig);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tConfig.uConfig=%u ORDER BY uConfig",uConfig);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uConfig");
        }

}//void ExttConfigListSelect(void)


void ExttConfigListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uConfig"))
                printf("<option>uConfig</option>");
        else
                printf("<option selected>uConfig</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttConfigListFilter(void)


void ExttConfigNavBar(void)
{
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

}//void ExttConfigNavBar(void)


void tConfigNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;

	ExtSelect("tConfig","tConfig.uConfig,tConfig.cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tConfigNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tConfigNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
		{
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tConfig&"
					"uConfig=%s>%s</a>",field[0],field[1]);
			sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uKey=%s AND uType="PROP_CONFIG"",field[0]);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				printf("%s",mysql_error(&gMysql));
	        	res2=mysql_store_result(&gMysql);
			if(mysql_num_rows(res2))
				printf("*");
			printf("<br>\n");
		        	mysql_free_result(res2);
		}
	}
        mysql_free_result(res);

}//void tConfigNavList(void)


unsigned htmlConfigContext(void)
{
	unsigned uRows=0;
        MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uConfig FROM tContainer WHERE uConfig=%u",uConfig);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	uRows=mysql_num_rows(res);
	printf("Used by %u containers.",uRows);
	mysql_free_result(res);

	return(uRows);

}//unsigned htmlConfigContext(void)


unsigned LocalImportConfigJob(unsigned uConfig)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uCount=0;
	unsigned uNode=0;
	unsigned uDatacenter=0;
	char cHostname[100];

	if(gethostname(cHostname,99)!=0)
		return(0);

	sprintf(gcQuery,"SELECT uNode,uDatacenter FROM tNode WHERE cLabel='%.99s'",cHostname);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		sscanf(field[0],"%u",&uNode);
		sscanf(field[1],"%u",&uDatacenter);
	}
	mysql_free_result(res);

	if(!uNode || !uDatacenter || !uConfig)
		return(0);

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='LocalImportConfigJob(%u)',cJobName='LocalImportConfigJob'"
			",uDatacenter=%u,uNode=%u,uContainer=0"
			",uJobDate=UNIX_TIMESTAMP(NOW())"
			",uJobStatus=1"
			",cJobData='uConfig=%u;'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uConfig,
				uDatacenter,uNode,uConfig,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uConfig,"tConfig","LocalConfigImport");
	return(uCount);

}//unsigned LocalImportConfigJob(...)
