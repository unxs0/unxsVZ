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

void tAccountTypeBasicCheck(void);
void tAccountTypeNavList(void);

void ExtProcesstAccountTypeVars(pentry entries[], int x)
{
	/*
	register int i;
	for(i=0;i<x;i++)
	{
	}
	*/
}//void ExtProcesstAccountTypeVars(pentry entries[], int x)


void ExttAccountTypeCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tAccountTypeTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstAccountTypeVars(entries,x);
                        	guMode=2000;
	                        tAccountType(LANG_NB_CONFIRMNEW);
			}
			else
				tAccountType("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
                        	ProcesstAccountTypeVars(entries,x);

                        	guMode=2000;
				tAccountTypeBasicCheck();
                        	guMode=0;

				uAccountType=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtAccountType(0);
			}
			else
				tAccountType("<blink>Error</blink>: Denied by permissions settings"); 
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstAccountTypeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
	                        guMode=2001;
				tAccountType(LANG_NB_CONFIRMDEL);
			}
			else
				tAccountType("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstAccountTypeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))   
			{
				guMode=5;
				DeletetAccountType();
			}
			else
				tAccountType("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstAccountTypeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
				guMode=2002;
				tAccountType(LANG_NB_CONFIRMMOD);
			}
			else
				tAccountType("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstAccountTypeVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))   
			{
                        	guMode=2002;
				tAccountTypeBasicCheck();
                        	guMode=0;

				uModBy=guLoginClient;
				ModtAccountType();
			}
			else
				tAccountType("<blink>Error</blink>: Denied by permissions settings"); 
                }
	}

}//void ExttAccountTypeCommands(pentry entries[], int x)


void ExttAccountTypeButtons(void)
{
	OpenFieldSet("tAccountType Aux Panel",100);
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
			tAccountTypeNavList();
	}
	CloseFieldSet();

}//void ExttAccountTypeButtons(void)


void ExttAccountTypeAuxTable(void)
{

}//void ExttAccountTypeAuxTable(void)


void ExttAccountTypeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uAccountType"))
		{
			sscanf(gentries[i].val,"%u",&uAccountType);
			guMode=6;
		}
	}
	tAccountType("");

}//void ExttAccountTypeGetHook(entry gentries[], int x)


void ExttAccountTypeSelect(void)
{
	ExtSelect("tAccountType",VAR_LIST_tAccountType,0);
	
}//void ExttAccountTypeSelect(void)


void ExttAccountTypeSelectRow(void)
{
	ExtSelectRow("tAccountType",VAR_LIST_tAccountType,uAccountType);

}//void ExttAccountTypeSelectRow(void)


void ExttAccountTypeListSelect(void)
{
	char cCat[512];

	ExtListSelect("tAccountType",VAR_LIST_tAccountType);

	//Changes here must be reflected below in ExttAccountTypeListFilter()
        if(!strcmp(gcFilter,"uAccountType"))
        {
                sscanf(gcCommand,"%u",&uAccountType);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tAccountType.uAccountType=%u \
						ORDER BY uAccountType",
						uAccountType);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uAccountType");
        }

}//void ExttAccountTypeListSelect(void)


void ExttAccountTypeListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uAccountType"))
                printf("<option>uAccountType</option>");
        else
                printf("<option selected>uAccountType</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttAccountTypeListFilter(void)


void ExttAccountTypeNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
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

}//void ExttAccountTypeNavBar(void)


void tAccountTypeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tAccountTypeNavList</u><br>\n");
	
	ExtSelect("tAccountType","tAccountType.uAccountType,tAccountType.cLabel",0);

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
			printf("<a class=darkLink href=unxsISP.cgi?gcFunction=tAccountType"
				"&uAccountType=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tAccountTypeNavList(void)


void tAccountTypeBasicCheck(void)
{
	if(!cLabel[0])
		tAccountType("<blink>Error: </blink> cLabel is required");
	else
	{
		if(guMode==2000)
		{
			MYSQL_RES *res;
			sprintf(gcQuery,"SELECT uAccountType FROM tAccountType WHERE cLabel='%s'",TextAreaSave(cLabel));
			macro_mySQLRunAndStore(res);
			if(mysql_num_rows(res))
				tAccountType("<blink>Error: </blink> cLabel already in use");
		}
	}

}//void tAccountTypeBasicCheck(void)

