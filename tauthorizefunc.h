/*
FILE
	tauthorizationfunc.h
PURPOSE
	tAuthorization aux functions
AUTHOR
	GPL License applies, see www.fsf.org for details
	See LICENSE file in this distribution
	(C) 2001-2009 Gary Wallis and Hugo Urquiza for Unixservice, LLC.
	(C) 2010-2017 Gary Wallis for Unixservice, LLC.
 
*/

#include <liboath/oath.h>

void ExtSelect2(const char *cTable,const char *cVarList,unsigned uMaxResults);
void tAuthorizeNavList(void);

void EncryptPasswd(char *cPasswd);//main.c
const char *cUserLevel(unsigned uPermLevel);//tclientfunc.h


static unsigned uCopyToPermLevel=0;

void ExtProcesstAuthorizeVars(pentry entries[], int x)
{

	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"uCopyToPermLevel"))
		{
			sscanf(entries[i].val,"%u",&uCopyToPermLevel);
		}
	
	}

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
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=2001;
				tAuthorize(LANG_NB_CONFIRMDEL);
			}
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstAuthorizeVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				DeletetAuthorize();
			}
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstAuthorizeVars(entries,x);
			if((uAllowMod(uOwner,uCreatedBy) || uCreatedBy==guLoginClient || guPermLevel>9 ) && (guPermLevel>11 || uPerm<12))
			{
				guMode=2001;


				if(!cOTPSecret[0])
				{
					//generate a secret
					size_t uSecretlen=0;
					char *cSecret;
					char cRandom[32];
					int iRc;
  					int fd=open("/dev/urandom",O_RDONLY);
					if(fd<0)
						tAuthorize("Failed to open \"/dev/urandom\"");
					if(read(fd,cRandom,sizeof(cRandom))!=sizeof(cRandom))
						tAuthorize("Failed to read from \"/dev/urandom\"");

					//oath_base32_encode(const char *in, size_t inlen, char **out, size_t *outlen);
					iRc=oath_base32_encode(cRandom,sizeof(cRandom),&cSecret,&uSecretlen);
					if(iRc!=OATH_OK)
						tAuthorize("cOTPSecret base32 encoding failed");
					sprintf(cOTPSecret,"%.16s",cSecret);
				}

				guMode=2002;
				tAuthorize(LANG_NB_CONFIRMMOD);
			}
			else
				tAuthorize("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstAuthorizeVars(entries,x);
			if((uAllowMod(uOwner,uCreatedBy) || uCreatedBy==guLoginClient || guPermLevel>9 ) && (guPermLevel>11 || uPerm<12))
			{
				//Place limits on what non root users can change.
				if(uPerm>guPermLevel) uPerm=guPermLevel;
				//we should not allow non root uses to change uCertClient
				unsigned uPrevCertClient=0;
				sscanf(ForeignKey("tAuthorize","uCertClient",uAuthorize),"%u",&uPrevCertClient);
				if(guPermLevel<12 && uPrevCertClient) uCertClient=uPrevCertClient;
				if(uPerm<1 || uPerm>12)
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

				//Must clear out encrypted password before replacing
				if(cClrPasswd[0] && strncmp(cPasswd,"$1$",3) && strncmp(cPasswd,"$5$",3) 
							&& strncmp(cPasswd,"$6$",3))
				{
					sprintf(cPasswd,"%.35s",cClrPasswd);
					EncryptPasswd(cPasswd);
				}
				else
				{
					if(strncmp(cPasswd,"$1$",3) && strncmp(cPasswd,"$5$",3) && strncmp(cPasswd,"$6$",3))
						EncryptPasswd(cPasswd);
				}

				if(cOTPSecret[0])
				{
					char *cSecret;
					size_t uSecretlen=0;
					int iRc;

					//validate
					iRc=oath_base32_decode(cOTPSecret,strlen(cOTPSecret),&cSecret,&uSecretlen);
					if(iRc!=OATH_OK)
						tAuthorize("cOTPSecret base32 decoding failed");

					//login system explained:
					//if cOTPSecret and uOTPExpire=0
					//	must login with passwd and OTP
					//if cOTPSecret is empty no login checks.
					// else
					//	every time we get login cookie we check to see
					//	if now>=uOTPExpire;
					//	if it is then we require login again.

					
					//set expire -this is done in main login code now.
					//time_t timeNow;
					//timeNow=time(NULL);
					//uOTPExpire=timeNow+8*3600;//8 hours more

					if(guPermLevel>10 && uCopyToPermLevel>0)
					{
						sprintf(gcQuery,"UPDATE tAuthorize SET cOTPSecret='%s',uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW())"
								" WHERE uPerm=%u AND cOTPSecret=''",
							cOTPSecret,guLoginClient,uCopyToPermLevel);
        					mysql_query(&gMysql,gcQuery);
        					if(mysql_errno(&gMysql))
							tAuthorize("<blink>Error</blink>: UPDATE error");
					}
				}
				else
					uOTPExpire=0;//cancel OTP

				uModBy=guLoginClient;
                        	ModtAuthorize();
			}
			else
				tAuthorize("<blink>Error</blink>: Denied by permissions settings");
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
			printf("Password setting: You can either enter a clear text passwd in cClrPasswd or enter a clear text passwd in cPasswd that will be encrypted into cPasswd and no cClrPasswd will be saved.<p>");
                        printf(LANG_NBB_CONFIRMNEW);
			printf("<br>\n");
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
			printf("<br>\n");
                break;

                case 2002:
			printf("<u>Modify: Step 1 Tips</u><br>");
			printf("Password changing: You have several choices for passwd changing: You can either enter a clear text passwd in cClrPasswd or enter a clear text passwd in cPasswd that will be encrypted into cPasswd and no cClrPasswd will be saved. And finally you can enter an MD5/SHA256/SHA512 $1$/$5$/$6$ prefixed encrypted password.<p>\n");
			printf("Other field changes: Unless you are absolutely sure what you need done, have 2nd level support (support@unixservice.com) do it for you.<p>\n");

			if(cOTPSecret[0] && guPermLevel>10)
				printf("<p><input type=text title='DANGER! Copies current cOTPSecret to all other"
					" tAuthorize records with the given uPerm' jname=uCopyToPermLevel value=%u>"
					" <font color=red>cOTPSecret uCopyToPermLevel</font>",
						uCopyToPermLevel);
			printf("<p>\n");
                        printf(LANG_NBB_CONFIRMMOD);
			printf("<br>\n");
                break;

		default:
			printf("<u>Table Tips (%s)</u><br>",cGitVersion);
			printf("Here you can change a passwd for a login of a contact or a non company affiliated login user. Other more complex changes can be done on other fields, but you should seek guidance from experienced users first. Clicking on the modify (new or delete) button will provide more details. All changes are two step operations so there is no danger on clicking on the 'New', 'Modify' or 'Delete' buttons.<p>\n");

			printf("<u>Record Context Info</u><br>");
			printf("This login appears to belong to a <a class=darkLink href=unxsVZ.cgi?gcFunction=tClient&uClient=%u>'%s'</a> company contact '<a class=darkLink href=unxsVZ.cgi?gcFunction=tClient&uClient=%u>%s</a>'.<br>The uPerm corresponds to permission level '%s'.",uOwner,ForeignKey("tClient","cLabel",uOwner),uCertClient,ForeignKey("tClient","cLabel",uCertClient),cUserLevel(uPerm));
			printf("<p>\n");

			tAuthorizeNavList();
	}

	CloseFieldSet();

}//void ExttAuthorizeButtons(void)


void ExttAuthorizeAuxTable(void)
{
	if(!uAuthorize)
		return;

        MYSQL_RES *res;
        MYSQL_ROW field;

	sprintf(gcQuery,"SELECT uProperty,cName,cValue FROM tProperty WHERE uKey=%u AND uType="PROP_AUTHORIZE
			" ORDER BY cName",uAuthorize);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		sprintf(gcQuery,"%s Property Panel",cLabel);
		OpenFieldSet(gcQuery,100);
		printf("<table cols=2>");
		while((field=mysql_fetch_row(res)))
		{
			printf("<tr>\n");
			printf("<td width=200 valign=top><a class=darkLink href=unxsVZ.cgi?"
					"gcFunction=tProperty&uProperty=%s&cReturn=tClient_%u>"
					"%s</a></td><td valign=top><pre>%s</pre></td>\n",
						field[0],uAuthorize,field[1],field[2]);
			printf("</tr>\n");
		}
		printf("</table>");
		CloseFieldSet();
	}

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
	//New ExtSelect() version requires a 3rd argument, the max row count number
	ExtSelect2("tAuthorize",VAR_LIST_tAuthorize,0);
}//void ExttAuthorizeSelect(void)


void ExttAuthorizeSelectRow(void)
{
	ExtSelectRow("tAuthorize",VAR_LIST_tAuthorize,uAuthorize);
}//void ExttAuthorizeSelectRow(void)


void ExttAuthorizeListSelect(void)
{
	char cCat[512];
	//ACModel
	if(guPermLevel>11)//Root can read access all
	{
		sprintf(gcQuery,"SELECT " VAR_LIST_tAuthorize " FROM tAuthorize");
	}
	else 
	{
		ExtListSelect("tAuthorize",VAR_LIST_tAuthorize);
		strcat(gcQuery," AND tAuthorize.uPerm<12");
	}
	//Changes here must be reflected below in ExttAuthorizeListFilter()
        if(!strcmp(gcFilter,"uAuthorize"))
        {
                sscanf(gcCommand,"%u",&uAuthorize);
		if(guPermLevel<12)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tAuthorize.uAuthorize=%u",uAuthorize);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"uPerm"))
        {
                sscanf(gcCommand,"%u",&uPerm);
		if(guPermLevel<12)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tAuthorize.uPerm=%u ORDER BY tAuthorize.cLabel",uPerm);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cOTPSecret"))
        {
		if(guPermLevel<12)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tAuthorize.cOTPSecret LIKE '%s%%' ORDER BY tAuthorize.cLabel",
				TextAreaSave(gcCommand));
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cLabel"))
        {
		if(guPermLevel<12)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tAuthorize.cLabel LIKE '%s%%' ORDER BY tAuthorize.cLabel",
				TextAreaSave(gcCommand));
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY tAuthorize.uAuthorize");
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
        if(strcmp(gcFilter,"uPerm"))
                printf("<option>uPerm</option>");
        else
                printf("<option selected>uPerm</option>");
        if(strcmp(gcFilter,"cOTPSecret"))
                printf("<option>cOTPSecret</option>");
        else
                printf("<option selected>cOTPSecret</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttAuthorizeListFilter(void)


void ExttAuthorizeNavBar(void)
{
	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=12 && !guListMode)
		printf(LANG_NBB_NEW);

	if((uAllowMod(uOwner,uCreatedBy) || uCreatedBy==guLoginClient || guPermLevel>9 ) && (guPermLevel>11 || uPerm<12))
		printf(LANG_NBB_MODIFY);

	if(uAllowDel(uOwner,uCreatedBy))
		printf(LANG_NBB_DELETE);

	if(uOwner)
		printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttAuthorizeNavBar(void)


void tAuthorizeNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uOwner)
	{
		sprintf(gcQuery,"SELECT uAuthorize,cLabel,uPerm,uCertClient"
			" FROM tAuthorize"
			" WHERE uOwner=%u ORDER BY cLabel",uOwner);
	}
	else
	{
		sprintf(gcQuery,"SELECT uAuthorize,cLabel,uPerm,uCertClient"
			" FROM tAuthorize"
			" WHERE uCertClient=%u ORDER BY cLabel",guLoginClient);
	}

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
        	printf("<p><u>tAuthorizeNavList</u><br>\n");
        	while((field=mysql_fetch_row(res)))
		{
			printf("<a class=darkLink href=unxsVZ.cgi?gcFunction=tAuthorize&uAuthorize=%s>"
				"%s/%s/%s</a><br>\n",field[0],field[1],field[2],field[3]);
		}
	}
        mysql_free_result(res);

}//void tAuthorizeNavList(void)

