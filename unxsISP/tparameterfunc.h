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

void tParameterBasicCheck(void);
void tParameterNavList(void);

void ExtProcesstParameterVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstParameterVars(pentry entries[], int x)


void ExttParameterCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tParameterTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstParameterVars(entries,x);
                        	guMode=2000;
	                        tParameter(LANG_NB_CONFIRMNEW);
			}
			else
				tParameter("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstParameterVars(entries,x);

                        	guMode=2000;
				tParameterBasicCheck();
                        	guMode=0;

				uParameter=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtParameter(0);
			}
			else
				tParameter("<blink>Error</blink>: Denied by permissions settings");   
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstParameterVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
	                        guMode=2001;
				tParameter(LANG_NB_CONFIRMDEL);
			}
			else
				tParameter("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstParameterVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
				guMode=5;
				DeletetParameter();
			}
			else
				tParameter("<blink>Error</blink>: Denied by permissions settings");   
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstParameterVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tParameter(LANG_NB_CONFIRMMOD);
			}
			else
				tParameter("<blink>Error</blink>: Denied by permissions settings");   
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstParameterVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				tParameterBasicCheck();
                        	guMode=0;

				uModBy=guLoginClient;
				ModtParameter();
			}
			else
				tParameter("<blink>Error</blink>: Denied by permissions settings");   
                }
	}

}//void ExttParameterCommands(pentry entries[], int x)


void ExttParameterButtons(void)
{
	OpenFieldSet("tParameter Aux Panel",100);
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
			printf("<p><u>Record Context Info</u><br>");
			tParameterNavList();
	}
	CloseFieldSet();

}//void ExttParameterButtons(void)


void ExttParameterAuxTable(void)
{

}//void ExttParameterAuxTable(void)


void ExttParameterGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uParameter"))
		{
			sscanf(gentries[i].val,"%u",&uParameter);
			guMode=6;
		}
	}
	tParameter("");

}//void ExttParameterGetHook(entry gentries[], int x)


void ExttParameterSelect(void)
{
	ExtSelect("tParameter",VAR_LIST_tParameter,0);
	
}//void ExttParameterSelect(void)


void ExttParameterSelectRow(void)
{
	ExtSelectRow("tParameter",VAR_LIST_tParameter,uParameter);

}//void ExttParameterSelectRow(void)


void ExttParameterListSelect(void)
{
	char cCat[512];

	ExtListSelect("tParameter",VAR_LIST_tParameter);

	//Changes here must be reflected below in ExttParameterListFilter()
        if(!strcmp(gcFilter,"uParameter"))
        {
                sscanf(gcCommand,"%u",&uParameter);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tParameter.uParameter=%u \
						ORDER BY uParameter",
						uParameter);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uParameter");
        }

}//void ExttParameterListSelect(void)


void ExttParameterListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uParameter"))
                printf("<option>uParameter</option>");
        else
                printf("<option selected>uParameter</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttParameterListFilter(void)


void ExttParameterNavBar(void)
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

}//void ExttParameterNavBar(void)



void tParameterNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
//char *cForeignKey(const char *cTableName, const char *cFieldName, unsigned uKey);
	char cProdType[16]={""};

	printf("<p><u>tParameterNavList</u><br>\n");

	ExtSelectSearch("tParameter","tParameter.uParameter,tParameter.cParameter,tParameter.uISMOrder,tParameter.uProductType",
			"tParameter.uProductType","%",NULL,0);

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
			if(strcmp(cProdType,field[3]))
			{
				printf("<br><u>%s</u><br>\n",ForeignKey("tProductType","cLabel",strtoul(field[3],NULL,10)));
				sprintf(cProdType,"%.15s",field[3]);
			}
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tParameter"
				"&uParameter=%s>%s (%s)</a><br>\n",field[0],field[1],field[2]);
		}
	        
	}
        mysql_free_result(res);

}//void tParameterNavList(void)


void tParameterBasicCheck(void)
{
	if(!cParameter[0])
		tParameter("<blink>Error: </blink> cParameter is required");
	else
	{
		if(guMode==2000)
		{
			MYSQL_RES *res;
			sprintf(gcQuery,"SELECT uParameter FROM tParameter WHERE cParameter='%s'",TextAreaSave(cParameter));
			macro_mySQLRunAndStore(res);
			if(mysql_num_rows(res))
				tParameter("<blink>Error: </blink> cParameter already in use");
		}
	}
	if(!uParamType)
		tParameter("<blink>Error: </blink> uParamType is required");
	if(!cRange[0])
		tParameter("<blink>Error: </blink> cRange is required");
	if(cExtVerify[0])
	{
		if(!strstr(cExtVerify,"SELECT")&&!strstr(cExtVerify,"select"))
			tParameter("<blink>Error: </blink> Your cExtVerify value doesn't make sense");
		if(!strstr(cExtVerify,"unxsRadius:") &&
			!strstr(cExtVerify,"iDNS:") &&
			!strstr(cExtVerify,"unxsMail:") &&
			!strstr(cExtVerify,"unxsApache:")
			)
			tParameter("<blink>Error: </blink> cExtVerify format is <unxsXXX>:<SELECT query>");
	}
	if(!uISMOrder)
		tParameter("<blink>Error: </blink> uISMOrder is required");
	if(!cISMName[0])
		tParameter("<blink>Error: </blink> cISMName is required");
	if(!uProductType)
		tParameter("<blink>Error: </blink> uProductType is required");
	
	if(cExtQuery[0] && uParamType==13)
	{
		if(!strstr(cExtVerify,"unxsRadius:") &&
			!strstr(cExtVerify,"iDNS:") &&
			!strstr(cExtVerify,"unxsMail:") &&
			!strstr(cExtVerify,"unxsApache:")
			)
			tParameter("<blink>Error: </blink> cExtQuery must specify unxsXXX subsystem, e.g.: unxsRadius:");
	}
		
		
}//void tParameterBasicCheck(void)

