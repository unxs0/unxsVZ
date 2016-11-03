/*
FILE
	svn ID removed
PURPOSE
	Non-schema dependent tconfiguration.c expansion.
AUTHOR/LEGAL
	(C) 2001-2010 Gary Wallis for Unixservice, LLC.
	GPL License applies, see www.fsf.org for details
	See LICENSE file in this distribution
*/

void tConfigurationNavList(void);
unsigned CreateConfigurationFileJob(unsigned uConfiguration,unsigned uDatacenter,unsigned uNode,unsigned uContainer);
void htmlGlossaryLink(char *cLabel);
void UpdateVZKey(char *cComment);

void ExtProcesstConfigurationVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstConfigurationVars(pentry entries[], int x)


void ExttConfigurationCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tConfigurationTools"))
	{
		time_t uActualModDate;

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstConfigurationVars(entries,x);
                        	guMode=2000;
	                        tConfiguration(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstConfigurationVars(entries,x);

                        	guMode=2000;
				//Check entries here
                        	guMode=0;

				uConfiguration=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtConfiguration(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstConfigurationVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tConfiguration("Make sure this is the record you would like to delete");
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstConfigurationVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetConfiguration();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstConfigurationVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tConfiguration(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstConfigurationVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
                        	guMode=0;


				//Setup security with private key
				if(strstr(cComment,"#unxsVZKey="))
					UpdateVZKey(cComment);

				uModBy=guLoginClient;
				ModtConfiguration();
			}
                }
                else if(!strcmp(gcCommand,"Create File Job"))
                {
                        ProcesstConfigurationVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy) && guPermLevel>11 && guLoginClient==1)
			{
				unsigned uCount=0;

				if(uConfiguration==0)
					tConfiguration("<blink>Error</blink>: uConfiguration==0!");
					
				if(uDatacenter==0 && uNode!=0)
					tConfiguration("<blink>Error</blink>: uDatacenter==0!");
					

				sscanf(ForeignKey("tConfiguration","uModDate",uConfiguration),"%lu",&uActualModDate);

				if(uModDate==uActualModDate)
				{
					uCount=CreateConfigurationFileJob(uConfiguration,uDatacenter,
							uNode,uContainer);
					if(uCount)
						tConfiguration("CreateConfigurationFileJob() Done");
					else
						tConfiguration("<blink>Error</blink>: No jobs created! Too specific?");
				}
				else
				{
					tConfiguration("This record was modified. Reload it.");
				}
			}
		}
	}

}//void ExttConfigurationCommands(pentry entries[], int x)


void ExttConfigurationButtons(void)
{

	OpenFieldSet("tConfiguration Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<p>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("This table is used for configuration settings used by the "
				"system that are of a more general nature than tProperty values. "
				"Like tProperty if a tConfiguration.cLabel has an entry in tGlossary "
				"then a link will be provided.<p>\n");
			tConfigurationNavList();
			printf("<p>");
			htmlGlossaryLink(cLabel);

	}
	CloseFieldSet();



}//void ExttConfigurationButtons(void)


void ExttConfigurationAuxTable(void)
{

}//void ExttConfigurationAuxTable(void)


void ExttConfigurationGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uConfiguration"))
		{
			sscanf(gentries[i].val,"%u",&uConfiguration);
			guMode=6;
		}
	}
	tConfiguration("");

}//void ExttConfigurationGetHook(entry gentries[], int x)


void ExttConfigurationSelect(void)
{
	if(guPermLevel>9)
		ExtSelectPublic("tConfiguration",VAR_LIST_tConfiguration);
	else
		ExtSelect("tConfiguration",VAR_LIST_tConfiguration);

}//void ExttConfigurationSelect(void)


void ExttConfigurationSelectRow(void)
{
	if(guPermLevel>9)
		ExtSelectRowPublic("tConfiguration",VAR_LIST_tConfiguration,uConfiguration);
	else
		ExtSelectRow("tConfiguration",VAR_LIST_tConfiguration,uConfiguration);

}//void ExttConfigurationSelectRow(void)


void ExttConfigurationListSelect(void)
{
	char cCat[512];

	ExtListSelect("tConfiguration",VAR_LIST_tConfiguration);

	//Changes here must be reflected below in ExttConfigurationListFilter()
        if(!strcmp(gcFilter,"uConfiguration"))
        {
                sscanf(gcCommand,"%u",&uConfiguration);
		if(guLoginClient!=1)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tConfiguration.uConfiguration=%u ORDER BY uConfiguration",uConfiguration);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cLabel"))
        {
		if(guLoginClient!=1)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tConfiguration.cLabel LIKE '%s' ORDER BY cLabel,uConfiguration",gcCommand);
		strcat(gcQuery,cCat);
	}
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uConfiguration");
        }

	//tConfiguration(gcQuery);

}//void ExttConfigurationListSelect(void)


void ExttConfigurationListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp; Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uConfiguration"))
                printf("<option>uConfiguration</option>");
        else
                printf("<option selected>uConfiguration</option>");
        if(strcmp(gcFilter,"cLabel"))
                printf("<option>cLabel</option>");
        else
                printf("<option selected>cLabel</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttConfigurationListFilter(void)


void ExttConfigurationNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=10)
		printf(LANG_NBB_NEW);

	if(uAllowMod(uOwner,uCreatedBy))
		printf(LANG_NBB_MODIFY);

	if(uAllowDel(uOwner,uCreatedBy))
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttConfigurationNavBar(void)


void tConfigurationNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT tConfiguration.uConfiguration,tConfiguration.cLabel,tConfiguration.cValue,"
			" IFNULL(tDatacenter.cLabel,'AllDatacenters'),IFNULL(tNode.cLabel,'AllNodes')"
			" FROM tConfiguration"
			" LEFT JOIN tDatacenter ON tConfiguration.uDatacenter=tDatacenter.uDatacenter"
			" LEFT JOIN tNode ON tConfiguration.uNode=tNode.uNode"
			" ORDER BY tConfiguration.cLabel,tConfiguration.uDatacenter,tConfiguration.uNode");
	mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tConfigurationNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
        	printf("<p><u>tConfigurationNavList</u><br>\n");

	        while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tConfiguration&"
					"uConfiguration=%s>%s/%s/%s (%.32s)</a><br>\n",field[0],field[1],field[3],field[4],field[2]);
	}
        mysql_free_result(res);

}//void tConfigurationNavList(void)


unsigned CreateConfigurationFileJob(unsigned uConfiguration,unsigned uDatacenter,unsigned uNode,unsigned uContainer)
{
	unsigned uCount=0;	
        MYSQL_RES *res;
        MYSQL_ROW field;

	//All datacenters and all nodes, but no uContainers
	if(uDatacenter==0 && uNode==0 && uContainer==0)
		sprintf(gcQuery,"SELECT tNode.uDatacenter,tNode.uNode,0 FROM tNode");
	//Single selected datacenter and every node of that datacenter, but no uContainers
	else if(uDatacenter!=0 && uNode==0 && uContainer==0)
		sprintf(gcQuery,"SELECT tNode.uDatacenter,tNode.uNode,0 FROM tNode WHERE"
			" tNode.uDatacenter=%u",uDatacenter);
	//Single selected datacenter node, no container
	else if(uDatacenter!=0 && uNode!=0 && uContainer==0)
		sprintf(gcQuery,"SELECT tNode.uDatacenter,tNode.uNode,0 FROM tNode WHERE"
			" tNode.uDatacenter=%u AND tNode.uNode=%u",uDatacenter,uNode);

	//Single containers
	//Completely specified container. No migration ever happened. Seems useless to me now.
	else if(uDatacenter!=0 && uNode!=0 && uContainer!=0)
		sprintf(gcQuery,"SELECT tNode.uDatacenter,tNode.uNode,tContainer.uContainer FROM tNode,tContainer WHERE"
			" tNode.uNode=tContainer.uNode AND tNode.uDatacenter=%u AND tNode.uNode=%u"
			" AND tContainer.uContainer=%u",uDatacenter,uNode,uContainer);
	//Find the datacenter and the node for a given container
	else if(uDatacenter==0 && uNode==0 && uContainer!=0)
		sprintf(gcQuery,"SELECT tNode.uDatacenter,tNode.uNode,tContainer.uContainer FROM tNode,tContainer WHERE"
			" tNode.uNode=tContainer.uNode"
			" AND tContainer.uContainer=%u",uContainer);
	//Find the datacenter for a given container of a specified node
	else if(uDatacenter==0 && uNode!=0 && uContainer!=0)
		sprintf(gcQuery,"SELECT tNode.uDatacenter,tNode.uNode,tContainer.uContainer FROM tNode,tContainer WHERE"
			" tNode.uNode=tContainer.uNode AND tNode.uNode=%u"
			" AND tContainer.uContainer=%u",uNode,uContainer);
	else if(1)
		htmlPlainTextError("Unexpected CreateConfigurationFileJob() uDatacenter,uNode and uContainer combination");

	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	while((field=mysql_fetch_row(res)))
	{
		sprintf(gcQuery,"INSERT INTO tJob SET cLabel='CreateConfigurationFileJob()',cJobName='InstallConfigFile'"
			",uDatacenter=%s,uNode=%s,uContainer=%s"
			",cJobData='uConfiguration=%u;',uJobDate=UNIX_TIMESTAMP(NOW())+60"
			",uJobStatus=1"
			",uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				field[0],field[1],field[2],
				uConfiguration,guLoginClient,guLoginClient);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		uCount++;
	}
        mysql_free_result(res);

	return(uCount);

}//unsigned CreateConfigurationFileJob()


//cComment must be big enough
void UpdateVZKey(char *cComment)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	FILE *fp;
	char cPrivateKey[256]={"privatekey2"};

	//If no private key file no security by default!
	if((fp=fopen("/etc/unxsvz/unxsvz.key","r")))
	{
		if(fgets(cPrivateKey,255,fp)!=NULL)
			cPrivateKey[strlen(cPrivateKey)-1]=0;//cut off /n
		fclose(fp);
	}

	sprintf(gcQuery,"SELECT SHA1(CONCAT(LEFT('%s',LOCATE('#unxsVZKey=','%s')),'%s'))",
			TextAreaSave(cComment),TextAreaSave(cComment),cPrivateKey);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char cKey[64]={""};
		char *cp;
		sprintf(cKey,"%.63s",field[0]);

		if((cp=strstr(cComment,"#unxsVZKey=")))
			sprintf(cp,"#unxsVZKey=%s",cKey);
	}
        mysql_free_result(res);
}//void UpdateVZKey(char *cComment)
