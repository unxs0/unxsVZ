/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Unixservice, LLC.
 
*/


void tOSTemplateNavList(void);
unsigned htmlOSTemplateContext(void);
unsigned LocalImportTemplateJob(unsigned uOSTemplate);

//file scoped var
static unsigned uDatacenter=0;
static char cuDatacenterPullDown[256]={""};

void ExtProcesstOSTemplateVars(pentry entries[], int x)
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
}//void ExtProcesstOSTemplateVars(pentry entries[], int x)


void ExttOSTemplateCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tOSTemplateTools"))
	{
        	MYSQL_RES *res;
		long unsigned luActualModDate=-1;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstOSTemplateVars(entries,x);
                        	guMode=2000;
	                        tOSTemplate(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstOSTemplateVars(entries,x);

                        	guMode=2000;
				//Check entries here
				if(strlen(cLabel)<3)
					tOSTemplate("<blink>Error</blink>: cLabel too short!");
				sprintf(gcQuery,"SELECT uOSTemplate FROM tOSTemplate WHERE cLabel='%s'",cLabel);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tOSTemplate("<blink>Error</blink>: tOSTemplate.cLabel already in use!");
				}
                        	guMode=0;

				uOSTemplate=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtOSTemplate(1);
				if(!uOSTemplate)
					tOSTemplate("<blink>Error</blink>: New tOSTemplate entry was not created!");

				sprintf(gcQuery,"INSERT INTO tProperty SET uKey=%u,uType="PROP_OSTEMPLATE
						",cName='cDatacenter',cValue='All Datacenters',uOwner=%u,uCreatedBy=%u"
						",uCreatedDate=UNIX_TIMESTAMP(NOW())"
							,uOSTemplate,guCompany,guLoginClient);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				tOSTemplate("New OS template created");
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstOSTemplateVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=0;
				sscanf(ForeignKey("tOSTemplate","uModDate",uOSTemplate),"%lu",&luActualModDate);
				if(uModDate!=luActualModDate)
					tOSTemplate("<blink>Error</blink>: This record was modified. Reload it!");
				sprintf(gcQuery,"SELECT uOSTemplate FROM tContainer WHERE uOSTemplate=%u",
						uOSTemplate);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tOSTemplate("<blink>Error</blink>: Can't delete a template used by a container!");
				}
	                        guMode=2001;
				tOSTemplate(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstOSTemplateVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				sscanf(ForeignKey("tOSTemplate","uModDate",uOSTemplate),"%lu",&luActualModDate);
				if(uModDate!=luActualModDate)
					tOSTemplate("<blink>Error</blink>: This record was modified. Reload it!");
				sprintf(gcQuery,"SELECT uOSTemplate FROM tContainer WHERE uOSTemplate=%u",
						uOSTemplate);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tOSTemplate("<blink>Error</blink>: Can't delete a template used by a container!");
				}
				DeletetOSTemplate();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstOSTemplateVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				sscanf(ForeignKey("tOSTemplate","uModDate",uOSTemplate),"%lu",&luActualModDate);
				if(uModDate!=luActualModDate)
					tOSTemplate("<blink>Error</blink>: This record was modified. Reload it!");
				tOSTemplate(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstOSTemplateVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				sprintf(gcQuery,"SELECT uOSTemplate FROM tContainer WHERE uOSTemplate=%u",
						uOSTemplate);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tOSTemplate("<blink>Error</blink>: Can't modify a template used by a container!");
				}
				mysql_free_result(res);
				sscanf(ForeignKey("tOSTemplate","uModDate",uOSTemplate),"%lu",&luActualModDate);
				if(uModDate!=luActualModDate)
					tOSTemplate("<blink>Error</blink>: This record was modified. Reload it!");
				if(strlen(cLabel)<3)
					tOSTemplate("<blink>Error</blink>: cLabel too short!");
                        	guMode=0;

				uModBy=guLoginClient;
				ModtOSTemplate();
			}
                }
		else if(!strcmp(gcCommand,"Local Import"))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstOSTemplateVars(entries,x);
				guMode=6;
				if(uOSTemplate<1)
					tOSTemplate("<blink>Error</blink>: Unexpected uOSTemplate&lt;1 error!");

				sprintf(gcQuery,"SELECT uOSTemplate FROM tContainer WHERE uOSTemplate=%u",
						uOSTemplate);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
        			res=mysql_store_result(&gMysql);
				if(mysql_num_rows(res))
				{
					mysql_free_result(res);
					tOSTemplate("<blink>Error</blink>: Can't import a template already in use!");
				}
				sscanf(ForeignKey("tOSTemplate","uModDate",uOSTemplate),"%lu",&luActualModDate);
				if(uModDate!=luActualModDate)
					tOSTemplate("<blink>Error</blink>: This record was modified. Reload it!");
				if(strlen(cLabel)<3)
					tOSTemplate("<blink>Error</blink>: cLabel too short!");
				
				if(LocalImportTemplateJob(uOSTemplate))
					tOSTemplate("Local Import job created");
				else
					tOSTemplate("<blink>Error</blink>: LocalImportTemplateJob() failed!");
			}
		}
                else if(!strcmp(gcCommand,"Enable"))
                {
                        ProcesstOSTemplateVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=6;
				sscanf(ForeignKey("tOSTemplate","uModDate",uOSTemplate),"%lu",&luActualModDate);
				if(uModDate!=luActualModDate)
					tOSTemplate("<blink>Error</blink>: This record was modified. Reload it.");
				if(!uDatacenter)
					sprintf(gcQuery,"INSERT tProperty SET cName='cDatacenter',cValue='All Datacenters',"
						"uType=%u,uKey=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						uPROP_OSTEMPLATE,uOSTemplate,uOwner,guLoginClient);
				else
					sprintf(gcQuery,"INSERT tProperty SET cName='cDatacenter',cValue='%s',"
						"uType=%u,uKey=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
						ForeignKey("tDatacenter","cLabel",uDatacenter),	
						uPROP_OSTEMPLATE,uOSTemplate,uOwner,guLoginClient);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
			}
			else
			{
				tOSTemplate("<blink>Error</blink>: Enable not allowed!");
			}
		}
                else if(!strcmp(gcCommand,"Disable"))
                {
                        ProcesstOSTemplateVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=6;
				sscanf(ForeignKey("tOSTemplate","uModDate",uOSTemplate),"%lu",&luActualModDate);
				if(uModDate!=luActualModDate)
					tOSTemplate("<blink>Error</blink>: This record was modified. Reload it.");
				if(!uDatacenter)
					//Delete all
					sprintf(gcQuery,"DELETE FROM tProperty WHERE cName='cDatacenter'"
						" AND uType=%u AND uKey=%u AND (uOwner=%u OR uCreatedBy=%u)",
						uPROP_OSTEMPLATE,uOSTemplate,guCompany,guLoginClient);
				else
					sprintf(gcQuery,"DELETE FROM tProperty WHERE cName='cDatacenter' AND cValue='%s'"
						" AND uType=%u AND uKey=%u AND (uOwner=%u OR uCreatedBy=%u)",
						ForeignKey("tDatacenter","cLabel",uDatacenter),
						uPROP_OSTEMPLATE,uOSTemplate,guCompany,guLoginClient);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
			}
			else
			{
				tOSTemplate("<blink>Error</blink>: Disable not allowed!");
			}
		}
                else if(!strcmp(gcCommand,"Clear All"))
                {
                        ProcesstOSTemplateVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=6;
				if(guCompany==1)
					sprintf(gcQuery,"DELETE FROM tProperty WHERE cName='cDatacenter'"
						" AND uType=%u",uPROP_OSTEMPLATE);
				else
					sprintf(gcQuery,"DELETE FROM tProperty WHERE cName='cDatacenter'"
						" AND uType=%u AND (uOwner=%u OR uCreatedBy=%u)",
						uPROP_OSTEMPLATE,guCompany,guLoginClient);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
				tOSTemplate("'Clear All' done");
			}
			else
			{
				tOSTemplate("<blink>Error</blink>: 'Clear All' not allowed!");
			}
		}
	}

}//void ExttOSTemplateCommands(pentry entries[], int x)


unsigned LocalImportTemplateJob(unsigned uOSTemplate)
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

	if(!uNode || !uDatacenter || !uOSTemplate)
		return(0);

	sprintf(gcQuery,"INSERT INTO tJob SET cLabel='LocalImportTemplateJob(%u)',cJobName='LocalImportTemplateJob'"
			",uDatacenter=%u,uNode=%u,uContainer=0"
			",uJobDate=UNIX_TIMESTAMP(NOW())"
			",uJobStatus=1"
			",cJobData='uOSTemplate=%u;'"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				uOSTemplate,
				uDatacenter,uNode,uOSTemplate,
				uOwner,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uCount=mysql_insert_id(&gMysql);
	unxsVZLog(uOSTemplate,"tOSTemplate","LocalImport");
	return(uCount);

}//unsigned LocalImportTemplateJob(...)


unsigned htmlOSTemplateContext(void)
{
	unsigned uRows=0;
        MYSQL_RES *res;

	sprintf(gcQuery,"SELECT uOSTemplate FROM tContainer WHERE uOSTemplate=%u",uOSTemplate);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	uRows=mysql_num_rows(res);
	printf("Used by %u containers.",uRows);
	mysql_free_result(res);

	return(uRows);

}//unsigned htmlOSTemplateContext(void)


void ExttOSTemplateButtons(void)
{
	unsigned uNum=0;

	OpenFieldSet("tOSTemplate Aux Panel",100);
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
			printf("Make sure that the cLabel entries (new or modified) have a corresponding "
				"/vz/template/cache/&lt;cLabel&gt;.tar.gz file on all unxsVZ controlled "
				"nodes. Please note the 32 character limitation on older versions of the "
				"software (limit is now 100 chars.)"
				"<p><u>Automated way to install an external template</u><br>"
				"1-. Transfer &lt;unused tOSTemplate.cLabel&gt;.tar.gz to a node with a local "
				"unxsVZ webmin and place in /vz/template/cache.<br>"
				"2-. Transfer &lt;unused tOSTemplate.cLabel&gt;.tar.gz.md5sum to a node with a local "
				"unxsVZ webmin and place in /vz/template/cache. This .md5sum file must correspond "
				"to to the file transferred in step 1-. This file must also have the expected md5sum "
				" command format with a fully qualified path to the tar.gz template file "
				"(see man md5sum for more info.)<br>"
				"3-. Run a [Local Import] job, after adding the new tOSTemplate.cLabel.<br>");
			printf("<p><u>Record Context Info</u><br>");
			uNum=htmlOSTemplateContext();
			tOSTemplateNavList();

			if(uOSTemplate>0 && uNum==0)
			{
				printf("<p><u>Extended table actions</u><br>");
                        	printf("<input class=largeButton title='Submit a job for the local import of the currently selected "
					"template' type=submit name=gcCommand value='Local Import'>");
			}
	}
	CloseFieldSet();

}//void ExttOSTemplateButtons(void)


void ExttOSTemplateAuxTable(void)
{
	if(!uOSTemplate || guMode==2000 )//uMODE_NEW
		return;

        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"tOSTemplate %s Property Panel",cLabel);
	OpenFieldSet(gcQuery,100);
	sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE uKey=%u AND uType="PROP_OSTEMPLATE
			" ORDER BY cName",uOSTemplate);

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
					"gcFunction=tProperty&uProperty=%s&cReturn=tOSTemplate_%u>"
					"%s</a></td><td>%s</td>\n",
						field[0],uOSTemplate,field[1],field[2]);
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
		" name=gcCommand value='Disable'><p>");
		printf("<input type=submit class=lwarnButton"
		" title='Clear all templates from tProperty association'"
		" name=gcCommand value='Clear All'></td>");
		printf("<td valign=top> Select a datacenter or none (---) for all ");
		tTablePullDown("tDatacenter;cuDatacenterPullDown","cLabel","cLabel",uDatacenter,1);
		printf("</td>");
		printf("</tr>\n");
	}
	
	printf("</table>");

	CloseFieldSet();

}//void ExttOSTemplateAuxTable(void)


void ExttOSTemplateGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uOSTemplate"))
		{
			sscanf(gentries[i].val,"%u",&uOSTemplate);
			guMode=6;
		}
	}
	tOSTemplate("");

}//void ExttOSTemplateGetHook(entry gentries[], int x)


void ExttOSTemplateSelect(void)
{
	ExtSelect("tOSTemplate",VAR_LIST_tOSTemplate);

}//void ExttOSTemplateSelect(void)


void ExttOSTemplateSelectRow(void)
{
	ExtSelectRow("tOSTemplate",VAR_LIST_tOSTemplate,uOSTemplate);

}//void ExttOSTemplateSelectRow(void)


void ExttOSTemplateListSelect(void)
{
	char cCat[512];

	ExtListSelect("tOSTemplate",VAR_LIST_tOSTemplate);
	
	//Changes here must be reflected below in ExttOSTemplateListFilter()
        if(!strcmp(gcFilter,"uOSTemplate"))
        {
                sscanf(gcCommand,"%u",&uOSTemplate);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tOSTemplate.uOSTemplate=%u ORDER BY uOSTemplate",uOSTemplate);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uOSTemplate");
        }

}//void ExttOSTemplateListSelect(void)


void ExttOSTemplateListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uOSTemplate"))
                printf("<option>uOSTemplate</option>");
        else
                printf("<option selected>uOSTemplate</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttOSTemplateListFilter(void)


void ExttOSTemplateNavBar(void)
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

}//void ExttOSTemplateNavBar(void)


void tOSTemplateNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;

	ExtSelect("tOSTemplate","tOSTemplate.uOSTemplate,tOSTemplate.cLabel");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tOSTemplateNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tOSTemplateNavList</u><br>\n");

		
	        while((field=mysql_fetch_row(res)))
		{
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tOSTemplate"
					"&uOSTemplate=%s>%s</a>",field[0],field[1]);
			sprintf(gcQuery,"SELECT uProperty FROM tProperty WHERE uKey=%s AND uType="PROP_OSTEMPLATE"",field[0]);
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

}//void tOSTemplateNavList(void)


