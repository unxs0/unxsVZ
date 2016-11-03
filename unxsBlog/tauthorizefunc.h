/*
FILE
	svn ID removed
PURPOSE
	Non-schema dependent tauthorize.c expansion.
AUTHOR
	GPL License applies, see www.fsf.org for details
	See LICENSE file in this distribution
	(C) 2001-2007 Gary Wallis.
 
*/


void tAuthorizeNavList(void);

void EncryptPasswdWithSalt(char *cPasswd,char *cSalt);
const char *cUserLevel(unsigned uPermLevel);//tclientfunc.h

void ExtProcesstAuthorizeVars(pentry entries[], int x)
{

	/*
	register int i;
	
	for(i=0;i<x;i++)
	{
	
	}
	*/

}//void ExtProcesstAuthorizeVars(pentry entries[], int x)


void ExttAuthorizeCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tAuthorizeTools"))
	{
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=12)
			{
                	        ProcesstAuthorizeVars(entries,x);
				//Check global conditions for new record here
				guMode=2000;
				tAuthorize(LANG_NB_CONFIRMNEW);
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=12)
			{
				ProcesstAuthorizeVars(entries,x);
				//Check entries here
				uAuthorize=0;
				uCreatedBy=guLoginClient;
				uOwner=guLoginClient;
				uModBy=0;//Never modified
				NewtAuthorize(0);
			}
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstAuthorizeVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=8 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2001;
				tAuthorize(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstAuthorizeVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=8 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uOwner!=1)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=5;
				DeletetAuthorize();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstAuthorizeVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=8 && uOwner==guLoginClient)
				|| (guPermLevel>9 && uCertClient==guLoginClient)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				guMode=2002;
				tAuthorize(LANG_NB_CONFIRMMOD);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstAuthorizeVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if( (guPermLevel>=8 && uOwner==guLoginClient)
				|| (guPermLevel>11)
				|| (guPermLevel>9 && uCertClient==guLoginClient)
				|| (guPermLevel>7 && guReseller==guLoginClient) )
			{
				if(uPerm>guPermLevel) uPerm=guPermLevel;
				if(uPerm<6 || uPerm>12)
				{
					guMode=2002;
					sprintf(gcQuery,"uPerm level error:%u",uPerm);
					tAuthorize(gcQuery);
				}
				if(!cPasswd[0] && !cClrPasswd[0])
				{
					guMode=2002;
					tAuthorize("Must provide a passwd");
				}

				if(cClrPasswd[0])
				{
					sprintf(cPasswd,"%.35s",cClrPasswd);
					EncryptPasswdWithSalt(cPasswd,"..");
				}
				else
				{
					if(strncmp(cPasswd,"..",2))
						EncryptPasswdWithSalt(cPasswd,"..");
				}

				uModBy=guLoginClient;
                        	ModtAuthorize();
			}
                }
	}

}//void ExttAuthorizeCommands(pentry entries[], int x)


void ExttAuthorizeButtons(void)
{
	OpenFieldSet("tAuthorize Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<u>New: Step 1 Tips</u><br>");
			printf("This should only be done by experienced staff.<p>");
			printf("cLabel: This field is the login char string.<p>");
			printf("cIpMask: This field is optionally used to limit the login from IP or IP range.<p>");
			printf("uPerm: This field is a number value of utmost importance. See the tClient [Authorize] process for more information.<p>");
			printf("uCertClient: Is usually the tClient.uClient number of the usage owner of this record, but maybe an alias like the value 1 for the default Root user.<p>");
			printf("Password setting: You can either enter a clear text passwd in cClrPasswd or enter a clear text passwd in cPasswd that will be encrypted into cPasswd and no cClrPasswd will be saved. And finally you can enter a fixed '..' salt DES encrypted passwd into cPasswd..<p>");
                        printf(LANG_NBB_CONFIRMNEW);
			printf("<br>\n");
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
			printf("<br>\n");
                break;

                case 2002:
			printf("<u>Modify: Step 1 Tips</u><br>");
			printf("Password changing: You have several choices for passwd changing: You can either enter a clear text passwd in cClrPasswd or enter a clear text passwd in cPasswd that will be encrypted into cPasswd and no cClrPasswd will be saved. And finally you can enter a fixed '..' salt DES encrypted passwd into cPasswd.<p>\n");
			printf("Other field changes: Unless you are absolutely sure what you need done, have 2nd level support (support@unixservice.com) do it for you.<p>\n");
                        printf(LANG_NBB_CONFIRMMOD);
			printf("<br>\n");
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("Here you can change a passwd for a login of a contact or a non company affiliated login user. Other more complex changes can be done on other fields, but you should seek guidance from experienced users first. Clicking on the modify (new or delete) button will provide more details. All changes are two step operations so there is no danger on clicking on the 'New', 'Modify' or 'Delete' buttons.<p>\n");
			printf("<u>Record Context Info</u><br>");
			if(uCertClient>1 && uOwner>1)
				printf("This login appears to belong to a <a class=darkLink href=unxsBlog.cgi?gcFunction=tClient&uClient=%u>'%s'</a> company contact '<a class=darkLink href=unxsBlog.cgi?gcFunction=tClient&uClient=%u>%s</a>'.<br>The uPerm corresponds to permission level '%s'.",uOwner,ForeignKey("tClient","cLabel",uOwner),uCertClient,ForeignKey("tClient","cLabel",uCertClient),cUserLevel(uPerm));
			else if(uOwner>1)
				printf("This login appears to belong to a '<a class=darkLink href=unxsBlog.cgi?gcFunction=tClient&uClient=%u>%s</a>' company contact, but that has been root aliased to usually run the back-office with complete permissions. <br>The uPerm corresponds to permission level '%s'.",uOwner,ForeignKey("tClient","cLabel",uOwner),cUserLevel(uPerm));
			printf("<p>\n");
			tAuthorizeNavList();
	}

	CloseFieldSet();

}//void ExttAuthorizeButtons(void)


void ExttAuthorizeAuxTable(void)
{

}//void ExttAuthorizeAuxTable(void)


void ExttAuthorizeGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uAuthorize"))
		{
			sscanf(gentries[i].val,"%u",&uAuthorize);
			guMode=6;
		}
	}
	tAuthorize("");

}//void ExttAuthorizeGetHook(entry gentries[], int x)


void ExttAuthorizeSelect(void)
{
        //Set non search query here for tTableName()
	if(guPermLevel>=9)
	sprintf(gcQuery,"SELECT %s FROM tAuthorize ORDER BY uAuthorize",
					VAR_LIST_tAuthorize);
	else
	sprintf(gcQuery,"SELECT %s FROM tAuthorize WHERE uOwner=%u ORDER BY uAuthorize",
					VAR_LIST_tAuthorize,guLoginClient);

}//void ExttAuthorizeSelect(void)


void ExttAuthorizeSelectRow(void)
{
	if(guPermLevel<10)
                sprintf(gcQuery,"SELECT %s FROM tAuthorize,tClient \
                                WHERE tAuthorize.uOwner=tClient.uClient\
                                AND (tClient.uOwner=%u OR tClient.uClient=%u)\
                                AND tAuthorize.uAuthorize=%u",
                        		VAR_LIST_tAuthorize,
					guLoginClient,guLoginClient,uAuthorize);
	else
                sprintf(gcQuery,"SELECT %s FROM tAuthorize WHERE uAuthorize=%u",
			VAR_LIST_tAuthorize,uAuthorize);

}//void ExttAuthorizeSelectRow(void)


void ExttAuthorizeListSelect(void)
{
	char cCat[512];

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT %s FROM tAuthorize,tClient \
		WHERE tAuthorize.uOwner=tClient.uClient \
		AND (tClient.uOwner=%u OR tClient.uClient=%u)",
				VAR_LIST_tAuthorize,
				guLoginClient,
				guLoginClient);
	else
                sprintf(gcQuery,"SELECT %s FROM tAuthorize",
				VAR_LIST_tAuthorize);

	//Changes here must be reflected below in ExttAuthorizeListFilter()
        if(!strcmp(gcFilter,"uAuthorize"))
        {
                sscanf(gcCommand,"%u",&uAuthorize);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tAuthorize.uAuthorize=%u ORDER BY uAuthorize",
						uAuthorize);
		strcat(gcQuery,cCat);
        }
        if(!strcmp(gcFilter,"cLabel"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tAuthorize.cLabel LIKE '%s' ORDER BY cLabel",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uAuthorize");
        }

}//void ExttAuthorizeListSelect(void)


void ExttAuthorizeListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uAuthorize"))
                printf("<option>uAuthorize</option>");
        else
                printf("<option selected>uAuthorize</option>");
        if(strcmp(gcFilter,"cLabel"))
                printf("<option>cLabel</option>");
        else
                printf("<option selected>cLabel</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttAuthorizeListFilter(void)


void ExttAuthorizeNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=12)
		printf(LANG_NBB_NEW);

	if( (guPermLevel>=8 && uOwner==guLoginClient)
		|| (guPermLevel>9 && uCertClient==guLoginClient)
		|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_MODIFY);

	if( (guPermLevel>=8 && uOwner==guLoginClient)
		|| (guPermLevel>9 && uOwner!=1)
		|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_DELETE);

	printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttAuthorizeNavBar(void)


void tAuthorizeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uAuthorize,cLabel,uPerm,uCertClient FROM tAuthorize WHERE uPerm>9");

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
        	printf("<p><u>tAuthorizeNavList Backoffice Only</u><br>\n");
        	while((field=mysql_fetch_row(res)))
        	{
printf("<a class=darkLink href=unxsBlog.cgi?gcFunction=tAuthorize&uAuthorize=%s>%s/%s/%s</a><br>\n",field[0],field[1],field[2],field[3]);
	        }
	}
        mysql_free_result(res);

}//void tAuthorizeNavList(void)

