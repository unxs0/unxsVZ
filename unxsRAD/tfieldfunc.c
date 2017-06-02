/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis for Unixservice.
 
*/

//ModuleFunctionProtos()
void DeleteField(unsigned uField);

static unsigned uTargetTable=0;

void tFieldNavList(void);

void ExtProcesstFieldVars(pentry entries[], int x)
{
	register int i;
	unsigned uGroupOp=0;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uTargetTable"))
			sscanf(entries[i].val,"%u",&uTargetTable);
		else if(!strncmp(entries[i].name,"CB",2))
		{
			//insider xss protection
			if(guPermLevel<10)
				continue;

			unsigned uCBField=0;
			sscanf(entries[i].name,"CB%u",&uCBField);
			if(uCBField)
			{
				if(!strcmp(gcFunction,"tFieldTools"))
				{
					if(!strcmp(gcCommand,LANG_NB_DELETE))
					{
						DeleteField(uCBField);
						uGroupOp++;
					}
				}
			}
		}
	}

	if(uGroupOp)
		tField("Group operation done");
	
}//void ExtProcesstFieldVars(pentry entries[], int x)


void ExttFieldCommands(pentry entries[], int x)
{

	if(!strcmp(gcFunction,"tFieldTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=9)
			{
	                        ProcesstFieldVars(entries,x);
                        	guMode=2000;
	                        tField(LANG_NB_CONFIRMNEW);
			}
			else
				tField("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=9)
			{
				unsigned uContactParentCompany=0;
                        	ProcesstFieldVars(entries,x);
				GetClientOwner(guLoginClient,&uContactParentCompany);
				
                        	guMode=2000;
				//Check entries here
				if(!cLabel[0])
					tField("<blink>Error</blink>: cLabel empty");
				if(strchr(cLabel,' '))
					tField("<blink>Error</blink>: Blank in cLabel");
                        	guMode=0;

				uField=0;
				uCreatedBy=guLoginClient;
				uOwner=uContactParentCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtField(0);
			}
			else
				tField("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstFieldVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				guCookieTable=uTable;
				guCookieField=uField;
				SetSessionCookie();
				tField(LANG_NB_CONFIRMDEL);
			}
			else
				tField("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstFieldVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetField();
			}
			else
				tField("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstFieldVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tField(LANG_NB_CONFIRMMOD);
			}
			else
				tField("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstFieldVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=2002;
				//Check entries here
				if(cFKSpec[0])
				{
					char cTable[100]={""};
					char cField[100]={""};
					if(cFKSpec[strlen(cFKSpec)-1]!='"' || sscanf(cFKSpec,"\"%99[a-zA-Z0-9\\.]\",\"%99[a-zA-Z0-9\\.]\"",cTable,cField)!=2)
						tField("cFKSpec not formatted correctly. E.g. \"tClient\",\"cLabel\"");
				}
				
                        	guMode=0;

				uModBy=guLoginClient;
				ModtField();
			}
			else
				tField("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,"Select"))
                {
                        ProcesstFieldVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(uField)
				{
					guCookieField=uField;
					SetSessionCookie();
					tField("This field selected for workflow");
				}
			}
			else
				tField("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,"Copy"))
                {
                        ProcesstFieldVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				if(uTargetTable)
				{
					uField=0;
					uTable=uTargetTable;
					Insert_tField();
					tField("Field has been copied");
				}
			}
			else
				tField("<blink>Error</blink>: Denied by permissions settings");
		}
	}

}//void ExttFieldCommands(pentry entries[], int x)


void ExttFieldButtons(void)
{
	OpenFieldSet("tField Aux Panel",100);
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
			printf("<p><u>Operations</u><br>");
			printf("<input type=submit class=largeButton"
				" title='Select and keep this field marked for current work flow'"
				" name=gcCommand value='Select'>");
			printf("<p><input type=text "
				" title='Enter the uTable value'"
				" name=uTargetTable value='%u'> uTargetTable",uTargetTable);
			printf("<br><input type=submit class=largeButton"
				" title='Copy this field to another table as specified above.'"
				" name=gcCommand value='Copy'>");
			tFieldNavList();
	}
	CloseFieldSet();

}//void ExttFieldButtons(void)


void ExttFieldAuxTable(void)
{

}//void ExttFieldAuxTable(void)


void ExttFieldGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uField"))
		{
			sscanf(gentries[i].val,"%u",&uField);
        		MYSQL_RES *res;
        		MYSQL_ROW field;

			sprintf(gcQuery,"SELECT uTable FROM tField WHERE uField=%u",uField);
        		mysql_query(&gMysql,gcQuery);
        		if(mysql_errno(&gMysql))
                		tTable(mysql_error(&gMysql));
        		res=mysql_store_result(&gMysql);
	        	if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&uTable);
				guCookieTable=uTable;
				guCookieField=uField;
				SetSessionCookie();
			}
			guMode=6;
		}
		else if(guCookieField)
		{
			uField=guCookieField;
			guMode=7;
		}
		else if(guCookieTable)
		{
        		MYSQL_RES *res;
        		MYSQL_ROW field;

			sprintf(gcQuery,"SELECT uField FROM tField WHERE uTable=%u ORDER BY uOrder LIMIT 1",guCookieTable);
        		mysql_query(&gMysql,gcQuery);
        		if(mysql_errno(&gMysql))
                		tTable(mysql_error(&gMysql));
        		res=mysql_store_result(&gMysql);
	        	if((field=mysql_fetch_row(res)))
			{
				sscanf(field[0],"%u",&uField);
				guMode=7;
			}
		}
	}
	tField("");

}//void ExttFieldGetHook(entry gentries[], int x)


void ExttFieldSelect(void)
{
	if(guCookieTable)
	{
		if(guLoginClient==1 && guPermLevel>11)
			sprintf(gcQuery,"SELECT %s FROM tField WHERE uTable=%u ORDER BY uField",VAR_LIST_tField,guCookieTable);
		else
			sprintf(gcQuery,"SELECT %s FROM tField,tClient WHERE tField.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" AND tField.uTable=%u"
				" ORDER BY uField",VAR_LIST_tField,guCookieTable,guCompany,guCompany);
	}
	else
	{
		if(guLoginClient==1 && guPermLevel>11)
			sprintf(gcQuery,"SELECT %s FROM tField ORDER BY uField",VAR_LIST_tField);
		else
			sprintf(gcQuery,"SELECT %s FROM tField,tClient WHERE tField.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY uField",VAR_LIST_tField,guCompany,guCompany);
	}

}//void ExttFieldSelect(void)


void ExttFieldSelectRow(void)
{
	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tField WHERE uField=%u",VAR_LIST_tField,uField);
	else
		sprintf(gcQuery,"SELECT %s FROM tField,tClient"
			" WHERE tField.uOwner=tClient.uClient"
			" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
			" AND tField.uField=%u",VAR_LIST_tField,guCompany,guCompany,uField);
}//void ExttFieldSelectRow(void)


void ExttFieldListSelect(void)
{
	char cCat[512];

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
		sprintf(gcQuery,"SELECT %s FROM tField",
				VAR_LIST_tField);
	else
		sprintf(gcQuery,"SELECT %s FROM tField,tClient"
				" WHERE tField.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)",
				VAR_LIST_tField
				,guCompany,guCompany);

	//Changes here must be reflected below in ExttFieldListFilter()
        if(!strcmp(gcFilter,"uProject"))
        {
                sscanf(gcCommand,"%u",&uProject);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tField.uProject=%u ORDER BY tField.uProject,tField.uTable,tField.uField",uProject);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uTable"))
        {
                sscanf(gcCommand,"%u",&uTable);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tField.uTable=%u ORDER BY tField.uTable,tField.uField",uTable);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uField"))
        {
                sscanf(gcCommand,"%u",&uField);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tField.uField=%u ORDER BY tField.uField",uField);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uField");
        }

	//debug
	//char cBuf[2048];
	//sprintf(cBuf,"%.2047s",gcQuery);
	//tField(cBuf);

}//void ExttFieldListSelect(void)


void ExttFieldListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uProject"))
                printf("<option>uProject</option>");
        else
                printf("<option selected>uProject</option>");
        if(strcmp(gcFilter,"uTable"))
                printf("<option>uTable</option>");
        else
                printf("<option selected>uTable</option>");
        if(strcmp(gcFilter,"uField"))
                printf("<option>uField</option>");
        else
                printf("<option selected>uField</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttFieldListFilter(void)


void ExttFieldNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=7 && !guListMode)
		printf(LANG_NBB_NEW);

	if(uAllowMod(uOwner,uCreatedBy) && !guListMode)
		printf(LANG_NBB_MODIFY);

	if(uAllowDel(uOwner,uCreatedBy) && !guListMode) 
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);
	printf("&nbsp;&nbsp;&nbsp;\n");

}//void ExttFieldNavBar(void)


void tFieldNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(guCookieTable)
	{
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
			sprintf(gcQuery,"SELECT uField,cLabel,uOrder FROM tField WHERE uTable=%u ORDER BY uOrder",guCookieTable);
		else
			sprintf(gcQuery,"SELECT tField.uField,tField.cLabel,tField.uOrder"
				" FROM tField,tClient"
				" WHERE tField.uOwner=tClient.uClient"
				" AND tField.uTable=%u"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY tField.uOrder",
					guCookieTable,guCompany,guLoginClient);
	}
	else
	{
		if(guLoginClient==1 && guPermLevel>11)//Root can read access all
			sprintf(gcQuery,"SELECT uField,cLabel,uOrder FROM tField ORDER BY uOrder");
		else
			sprintf(gcQuery,"SELECT tField.uField,tField.cLabel,tField.uOrder"
				" FROM tField,tClient"
				" WHERE tField.uOwner=tClient.uClient"
				" AND tClient.uOwner IN (SELECT uClient FROM tClient WHERE uOwner=%u OR uClient=%u)"
				" ORDER BY tField.uOrder",
					guCompany,guLoginClient);
	}
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>tFieldNavList</u><br>\n");
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		char *cColor;
        	printf("<p><u>tFieldNavList</u><br>\n");
		printf("<input type=checkbox name=all onClick='checkAll(document.formMain,this)'> Check all<br>\n");
	        while((field=mysql_fetch_row(res)))
		{
			if(atoi(field[0])==uField)
				cColor="blue";
			else
				cColor="black";
			printf("<input type=checkbox name=CB%s >"
				"<a class=darkLink href=unxsRAD.cgi?gcFunction=tField"
				"&uField=%s><font color=%s>%s %s</font></a><br>\n",
					field[0],field[0],cColor,field[1],field[2]);
		}
	}
        mysql_free_result(res);

}//void tFieldNavList(void)


void DeleteField(unsigned uField)
{
	sprintf(gcQuery,"DELETE FROM tField WHERE uField=%u AND ( uOwner=%u OR %u>9 )"
					,uField,guLoginClient,guPermLevel);
	macro_mySQLQueryHTMLError;
	if(mysql_affected_rows(&gMysql)>0)
	{
		unxsRADLog(uField,"tField","Del");
	}
	else
	{
		unxsRADLog(uField,"tField","DelError");
		tField(LANG_NBR_RECNOTDELETED);
	}

}//void DeleteField()

