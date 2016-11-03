/*
FILE
	svn ID removed
PURPOSE

AUTHOR
	Template and mysqlRAD2 author: (C) 2001-2009 Gary Wallis and Hugo Urquiza.
	GPL License applies, see www.fsf.org for details

 
*/

static unsigned uParameter=0;
static char cuParameterPullDown[256]={""};

static char cCloneTarget[33]={""};

void tServiceNavList(void);
void tServiceConfigList(unsigned uGroup);
void DeleteFromConfig(unsigned uProduct);
void tServiceBasicCheck();


void ExtProcesstServiceVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cuParameterPullDown"))
		{
			strcpy(cuParameterPullDown,entries[i].val);
			uParameter=ReadPullDown("tParameter","cParameter",
					cuParameterPullDown);
		}
		else if(!strcmp(entries[i].name,"cCloneTarget"))
		{
			sprintf(cCloneTarget,"%.32s",entries[i].val);
		}
	}
}//void ExtProcesstServiceVars(pentry entries[], int x)


void ExttServiceCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tServiceTools"))
	{
		if(!strcmp(gcCommand,"Clone"))
		{
			time_t clock;
			MYSQL_RES *res;
			MYSQL_RES *res2;
			MYSQL_ROW field;
			unsigned uCloneTarget=0;

			if(guPermLevel>=12)
			{
			time(&clock);
                        ProcesstServiceVars(entries,x);

			if(!uService)
				tService("Must select valid source service");
			if(!cCloneTarget[0])
				tService("Must enter valid clone target service label");

			sprintf(gcQuery,"SELECT uService FROM tService WHERE cLabel='%s'",
						cCloneTarget);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tService(mysql_error(&gMysql));
			res2=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res2)))
				sscanf(field[0],"%u",&uCloneTarget);
			mysql_free_result(res2);

			if(!uCloneTarget)
				tService("Must enter valid clone target service label");
	
			
			sprintf(gcQuery,"SELECT tConfig.uGroup,tConfig.uParameter,tConfig.cValue FROM tConfig,tService WHERE tService.uService=tConfig.uGroup AND tService.uService=%u",uService);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tService(mysql_error(&gMysql));
			res2=mysql_store_result(&gMysql);
			while((field=mysql_fetch_row(res2)))
			{	
			sprintf(gcQuery,"SELECT uConfig FROM tConfig WHERE uGroup=%s AND uParameter=%s",field[0],field[1]);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tService(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)<1) tService("No parameters found!");
			mysql_free_result(res);

			if(!field[0][0]) tService("No service found!");
			if(!field[1][0]) tService("No product found!");

			sprintf(gcQuery,"INSERT INTO tConfig SET uGroup=%u,uParameter=%s,cValue='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
					uCloneTarget,
					field[1],
					field[2],
					guLoginClient,
					guLoginClient,
					(long unsigned)clock);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tService(mysql_error(&gMysql));
			}
			mysql_free_result(res2);
			tService("Clone done");
			}//uPerm==root
		}
		else if(!strcmp(gcCommand,"AddRelated"))
		{
			time_t clock;
			char cSubsystem[100]={"X"};
			MYSQL_RES *res;
			MYSQL_ROW field;

			if(guPermLevel>=12)
			{
			time(&clock);
                        ProcesstServiceVars(entries,x);
			
			if(!uService || !uParameter)
				tService("<blink>Error: </blink>Must select valid parameter and service");
			sprintf(gcQuery,"SELECT cParameter FROM tParameter WHERE uParameter=%u",uParameter);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tService(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
			{
				char *cp;

				strcpy(cSubsystem,field[0]);
				if((cp=strchr(cSubsystem,'.'))) *cp=0;


				sprintf(gcQuery,"INSERT INTO tConfig "
						"(uConfig,uGroup,uParameter,uOwner,uCreatedBy,uCreatedDate) "
						"SELECT 0,%u,uParameter,%u,%u,%lu FROM tParameter WHERE cParameter LIKE '%s%%'",
					uService,
					guLoginClient,
					guLoginClient,
					(long unsigned)clock,
					cSubsystem);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tService(mysql_error(&gMysql));
			}
			tService("Parameter(s) added");
			}

		}
		else if(!strcmp(gcCommand,"AddParameter"))
		{
			time_t clock;
			MYSQL_RES *res;

			if(guPermLevel>=12)
			{
			time(&clock);
                        ProcesstServiceVars(entries,x);
			
			if(!uService || !uParameter)
				tService("<blink>Error: </blink>Must select valid parameter and service");
			sprintf(gcQuery,"SELECT uConfig FROM tConfig WHERE uGroup=%u AND uParameter=%u",uService,uParameter);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tService(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			if(mysql_num_rows(res)) tService("");

			sprintf(gcQuery,"INSERT INTO tConfig SET uConfig=0,uGroup=%u,uParameter=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=%lu",
					uService,
					uParameter,
					guLoginClient,
					guLoginClient,
					(long unsigned)clock);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tService(mysql_error(&gMysql));
			tService("Parameter added");
			}

		}
		else if(!strcmp(gcCommand,"DelParameter"))
		{
			if(guPermLevel>=12)
			{
                        ProcesstServiceVars(entries,x);
			if(!uService || !uParameter)
				tService("<blink>Error: </blink>Must select valid parameter and service");
			sprintf(gcQuery,"DELETE FROM tConfig WHERE uGroup=%u AND uParameter=%u",uService,uParameter);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tService(mysql_error(&gMysql));
			tService("Parameter deleted");
			}
		}
		else if(!strcmp(gcCommand,"DelAll"))
		{
			if(guPermLevel>=12)
			{
                        ProcesstServiceVars(entries,x);
			if(!uService)
				tService("Must select valid service");
			sprintf(gcQuery,"DELETE FROM tConfig WHERE uGroup=%u",uService);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				tService(mysql_error(&gMysql));
			tService("Parameters deleted");
			}
		}
		//end custom
		
		else if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstServiceVars(entries,x);
                        	guMode=2000;
	                        tService(LANG_NB_CONFIRMNEW);
			}
			else
				tService("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstServiceVars(entries,x);

                        	guMode=2000;
				tServiceBasicCheck();
                        	guMode=0;

				uService=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtService(0);
			}
			else
				tService("<blink>Error</blink>: Denied by permissions settings"); 
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstServiceVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy)) 
			{
	                        guMode=2001;
				tService(LANG_NB_CONFIRMDEL);
			}
			else
				tService("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstServiceVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy)) 
			{
				guMode=5;
				DeleteFromConfig(uService);
				DeletetService();
			}
			else
				tService("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstServiceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy)) 
			{
				guMode=2002;
				tService(LANG_NB_CONFIRMMOD);
			}
			else
				tService("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstServiceVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy)) 
			{
                        	guMode=2002;
				tServiceBasicCheck();
                        	guMode=0;

				uModBy=guLoginClient;
				ModtService();
			}
			else
				tService("<blink>Error</blink>: Denied by permissions settings"); 
                }
	}

}//void ExttServiceCommands(pentry entries[], int x)


void ExttServiceButtons(void)
{
	OpenFieldSet("tService Aux Panel",100);

	switch(guMode)
        {
                case 2000:
			printf("<p><u>Enter required data</u><br>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<p><u>Review record data</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

	}

	if(uAllowMod(uOwner,uCreatedBy))
	{
		tTablePullDownOwner("tParameter;cuParameterPullDown",
				"cParameter","cParameter",uParameter,1);

		printf("<br><input class=largeButton title='Add parameter to this service' type=submit name=gcCommand value=AddParameter><br>");
		printf("<input class=largeButton title='Add all related parameters to this service' type=submit name=gcCommand value=AddRelated><br>");
		printf("<input class=lwarnButton title='Delete parameter from this service' type=submit name=gcCommand value=DelParameter><br>");
		printf("<input class=lwarnButton title='Del all parameters from this service' type=submit name=gcCommand value=DelAll>");

		//Clone stuff
		printf("<p><input type=text size=20 maxlength=32 title='Input existing service label to clone to' name=cCloneTarget><br>");
		printf("<input class=lwarnButton title='Clone to target above. Must exist.' type=submit name=gcCommand value=Clone><br>");

	}

	tServiceConfigList(uService);
	tServiceNavList();

	CloseFieldSet();
	
}//void ExttServiceButtons(void)


void ExttServiceAuxTable(void)
{

}//void ExttServiceAuxTable(void)


void ExttServiceGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uService"))
		{
			sscanf(gentries[i].val,"%u",&uService);
			guMode=6;
		}
	}
	tService("");

}//void ExttServiceGetHook(entry gentries[], int x)


void ExttServiceSelect(void)
{
	ExtSelect("tService",VAR_LIST_tService,0);
	
}//void ExttServiceSelect(void)


void ExttServiceSelectRow(void)
{
	ExtSelectRow("tService",VAR_LIST_tService,uService);
	
}//void ExttServiceSelectRow(void)


void ExttServiceListSelect(void)
{
	char cCat[512];

	ExtListSelect("tService",VAR_LIST_tService);

	//Changes here must be reflected below in ExttServiceListFilter()
        if(!strcmp(gcFilter,"uService"))
        {
                sscanf(gcCommand,"%u",&uService);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tService.uService=%u \
						ORDER BY uService",
						uService);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uService");
        }

}//void ExttServiceListSelect(void)


void ExttServiceListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uService"))
                printf("<option>uService</option>");
        else
                printf("<option selected>uService</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttServiceListFilter(void)


void ExttServiceNavBar(void)
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

}//void ExttServiceNavBar(void)


void tServiceNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tServiceNavList</u><br>\n");
	
	ExtSelect("tService","tService.uService,tService.cLabel,tService.uAvailable",0);

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
				printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tService&uService=%s>",field[0]);
				if(field[2][0]=='0')
					printf("<font color=gray>%s</font></a><br>\n",field[1]);
				else
				printf("%s</a><br>\n",field[1]);
	        }
	}
        mysql_free_result(res);

}//void tServiceNavList(void)


void tServiceConfigList(unsigned uGroup)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	
	if(!uGroup) return;

	printf("<p><u>tServiceConfigList</u><br>\n");
	
	sprintf(gcQuery,"SELECT tConfig.uConfig,tParameter.cParameter,tConfig.cValue FROM tConfig,tParameter "
			"WHERE tConfig.uParameter=tParameter.uParameter AND tConfig.uGroup=%u ORDER BY tParameter.cParameter",uGroup);
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
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tConfig&uConfig=%s&uService=%u>%s=%s</a><br>\n",
				field[0],uGroup,field[1],field[2]);
	}
	mysql_free_result(res);

}//void tServiceConfigList(unsigned uGroup)


void DeleteFromConfig(unsigned uProduct)
{
	sprintf(gcQuery,"DELETE FROM tConfig WHERE uGroup=%u",uProduct);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		tService(mysql_error(&gMysql));

}//void DeleteFromConfig(unsigned uProduct)


void tServiceBasicCheck()
{
	if(!cLabel[0])
		tService("<blink>Error: </blink>cLabel is required");
	else
	{
		if(guMode==2000)
		{
			MYSQL_RES *res;
			sprintf(gcQuery,"SELECT uService FROM tService WHERE cLabel='%s'",cLabel);
			macro_mySQLRunAndStore(res);
			if(mysql_num_rows(res))
				tService("<blink>Error: </blink>cLabel already in use");
		}
	}
	if(!cServer[0])
		tService("<blink>Error: </blink>cServer is required");
	if(!cJobName[0])
		tService("<blink>Error: </blink>cJobName is required");
	else
	{
		if(!strstr(cJobName,"unxsRadius.") &&
			!strstr(cJobName,"unxsApache.") &&
			!strstr(cJobName,"unxsMail.") &&
			!strstr(cJobName,"iDNS."))
			tService("<blink>Error: </blink>cJobName must include the name of the unxsXXX subsystem.");
	}

}//void tServiceBasicCheck()

