/*
FILE
	$Id: tclientfunc.h 1953 2012-05-22 15:03:17Z Colin $
PURPOSE
	Non-schema dependent tclient.c expansion.
AUTHOR
	GPL License applies, see www.fsf.org for details
	See LICENSE file in this distribution
	(C) 2001-2007 Gary Wallis.
*/

#define BO_CUSTOMER	"Back-Office Customer"
#define BO_RESELLER	"Back-Office Reseller"
#define BO_ADMIN 	"Back-Office Admin"
#define BO_ROOT 	"Back-Office Root"

#define ORG_CUSTOMER	"Organization Customer"
#define ORG_WEBMASTER	"Organization Webmaster"
#define ORG_SALES	"Organization Sales Force"
#define ORG_SERVICE	"Organization Customer Service"
#define ORG_ACCT	"Organization Bookkeeper"
#define ORG_ADMIN	"Organization Admin"

static char cPasswd[36]={""};
static char cLogin[33]={""};
static char cExtLabel[16]={""};
static char cuPerm[33]={ORG_ADMIN};
static unsigned uPerm=0;
static unsigned uOnlyOrgs=0;
static char cSearch[100]={""};

//Aux drop/pull downs
static char cForClientPullDown[256]={""};
static unsigned uForClient=0;


unsigned IsAuthUser(char *cLabel, unsigned uOwner, unsigned uCertClient);
void PermLevelDropDown(char *cuPerm);

void EncryptPasswdWithSalt(char *cPasswd,char *cSalt);
void EncryptPasswd(char *cPasswd);
void GetClientMaxParams(unsigned uClient,unsigned *uMaxSites,unsigned *uMaxIPs);
const char *cUserLevel(unsigned uPermLevel);
unsigned uMaxClientsReached(unsigned uClient);
void tTablePullDownResellers(int unsigned uSelector);
void ContactsNavList(void);
void htmlRecordContext(void);


void ExtProcesstClientVars(pentry entries[], int x)
{

	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.35s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLogin"))
			sprintf(cLogin,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cExtLabel"))
			sprintf(cExtLabel,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cuPerm"))
		{
			sprintf(cuPerm,"%.32s",entries[i].val);
			sprintf(cuPerm,"%.32s",entries[i].val);
			if(!strcmp(cuPerm,ORG_ADMIN))
				uPerm=6;
			else if(!strcmp(cuPerm,BO_ADMIN))
				uPerm=10;
			else if(!strcmp(cuPerm,BO_ROOT))
				uPerm=12;
			else if(!strcmp(cuPerm,BO_CUSTOMER))
				uPerm=7;
			else if(!strcmp(cuPerm,BO_RESELLER))
				uPerm=8;
		}
		else if(!strcmp(entries[i].name,"cForClientPullDown"))
		{
			strcpy(cForClientPullDown,entries[i].val);
			uForClient=ReadPullDown("tClient","cLabel",
					cForClientPullDown);
		}
		else if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.99s",entries[i].val);
		else if(!strcmp(entries[i].name,"uOnlyOrgs"))
			uOnlyOrgs=1;
	}

}//void ExtProcesstClientVars(pentry entries[], int x)


void ExttClientCommands(pentry entries[], int x)
{
	if(!strcmp(gcFunction,"tClientTools"))
	{
		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=8)
			{
				ProcesstClientVars(entries,x);
				if(guPermLevel==8 && uMaxClientsReached(guLoginClient))
				{
					guMode=0;
					tClient("Your reseller maximum of customers has been reached");
				}
				guMode=2000;
				tClient(LANG_NB_CONFIRMNEW);
			}
			else
				tClient("<blink>Error</blink>: Denied by permissions settings");
				
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=8)
			{
				ProcesstClientVars(entries,x);
				if(guPermLevel==8 && uMaxClientsReached(guLoginClient))
				{
					guMode=0;
					tClient("Your reseller maximum of customers has been reached");
				}
				if(!uForClient)
					uOwner=guLoginClient;
				else
					uOwner=uForClient;
				uClient=0;
				uCreatedBy=guLoginClient;
				uModBy=0;//Never modified
				NewtClient(0);
			}
			else
				tClient("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstClientVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
                        	guMode=2001;
                        	tClient(LANG_NB_CONFIRMDEL);
			}
			else
				tClient("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstClientVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
                        	guMode=2001;
				sprintf(gcQuery,"DELETE FROM tAuthorize WHERE uCertClient=%u"
						" AND (uOwner=%u OR uOwner=%u OR uCreatedBy=%u)",
							uClient,uClient,guLoginClient,guLoginClient);
				mysql_query(&gMysql,gcQuery);
        			if(mysql_errno(&gMysql))
                			tClient(mysql_error(&gMysql));
				{{cProject}}Log(uClient,"tAuthorize","Del attempt via uClient=uCertClient");
                        	guMode=5;
                        	DeletetClient();
			}
			else
				tClient("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstClientVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tClient(LANG_NB_CONFIRMMOD);
			}
			else
				tClient("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstClientVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				uModBy=guLoginClient;
				if(uForClient && uClient>1)
				{
					sprintf(gcQuery,"UPDATE tClient SET uOwner=%u WHERE uClient=%u",uForClient,uClient);
					mysql_query(&gMysql,gcQuery);
        				if(mysql_errno(&gMysql))
					{
						printf("Content-type: text/plain\n\n");
                				printf("mysql_error: %s\n",mysql_error(&gMysql));
						exit(0);
					}
					uOwner=uForClient;
				}
				ModtClient();
			}
			else
				tClient("<blink>Error</blink>: Denied by permissions settings");
                }

                else if(!strcmp(gcCommand,"Authorize"))
                {
			//Level 8!
                        ProcesstClientVars(entries,x);
			if( uClient && guPermLevel>9)
			{
				guMode=3000;
				tClient("Enter login, passwd, user level and confirm. To authorize a new user to use an interface or this back-office.");
			}
			else
				tClient("<blink>Error</blink>: Denied by permissions settings");
		}
                else if(!strcmp(gcCommand,"Confirm Authorize"))
                {
			//Changes only for admin or root back office use
                        ProcesstClientVars(entries,x);
			if( uClient && guPermLevel>9 )
			{
				time_t clock;
				char cClrPasswd[33];

			        time(&clock);
				
				if(strlen(cLogin)<6)
				{
					guMode=3000;
					tClient("Login must be at least 6 chars!");
				}

				if(strlen(cPasswd)<4)
				{
					guMode=3000;
					tClient("Passwd must be at least 4 chars!");
				}

				if(uPerm<1 || uPerm>12)
				{
					guMode=3000;
					sprintf(gcQuery,"uPerm level error:%u",uPerm);
					tClient(gcQuery);
				}
		
				sprintf(cClrPasswd,"%.32s",cPasswd);
				EncryptPasswd(cPasswd);
				//if(uPerm==12) uClient=1;//uCertClient root alias temp hack
				sprintf(gcQuery,"INSERT INTO tAuthorize SET cLabel='%s',uPerm=%u,uCertClient=%u,cPasswd='%s',uOwner=%u,uCreatedBy=%u,"
						"uCreatedDate=UNIX_TIMESTAMP(NOW()),cIPMask='0.0.0.0',cClrPasswd='%s'",
						cLogin
						,uPerm
						,uClient
						,cPasswd
						,uOwner
						,guLoginClient
						,cClrPasswd);

				mysql_query(&gMysql,gcQuery);
        			if(mysql_errno(&gMysql))
				{
					printf("Content-type: text/plain\n\n");
                			printf("mysql_error: %s\n",mysql_error(&gMysql));
					exit(0);
				}
				tClient("Contact Authorized");
			}
			else
				tClient("<blink>Error</blink>: Denied by permissions settings");
				

		}//Confirm auth

	}

}//void ExttClientCommands(pentry entries[], int x)


void ExttClientButtons(void)
{
	unsigned uDefault=0;

	OpenFieldSet("tClient Aux Panel",100);
	 
	switch(guMode)
        {
                case 2000:
			printf("<u>New: Step 1 Tips</u><br>");
			printf("Here you would usually enter a new company name into cLabel. "
			"Optionally some standardized company info in cInfo, like addresses phone numbers and such. "
			"A main company email is usually helpful, and cCode can be used for easy matching with other "
			"databases you may have for your customers like a CRM or accounting software etc. <br>"
			"If you are creating a contact for an existing company select that company from the drop down "
			"select below and use cLabel for the contact name (Ex. Anne Flechter) and the cInfo would be the "
			"contacts personal phone numbers and or address etc.");
			if(guPermLevel>7)
			{
				printf("<p><u>Create as Contact for Company</u><br>");
				if(uClient==1) uClient=0;
				tTablePullDownResellers(uClient);
			}
                        printf(LANG_NBB_CONFIRMNEW);
			printf("<br>\n");
                break;

                case 2001:
                        printf(LANG_NBB_CONFIRMDEL);
			printf("<br>Note: Will also delete tAuthorize entries related to this uClient. "
				"Probably not a good idea to delete Root owned tClient records this way, even if you have the permissions to do so.\n");
                break;

                case 2002:
			printf("<u>Modify: Step 1 Tips</u><br>");
			printf("Here you can modify the contact or company name. In the later case still keeping all "
				"associated contacts. You can update the cInfo text area. Add an email or company or contact code.<br>"
				"A much more advanced operation and one that must be done with care is the use of the 'Change uOwner' "
				"drop down select: It is meant primarily to associate or change the association of contacts or other orphan "
				"tClient records with a given company. It can also be used to do the reverse selecting 'Root' as the company. "
				"In all cases the 'Change uOwner' feature must be used with caution since it may affect many other tClient and "
				"tAuthorize records indirectly, especially if used on a company record.");

			htmlRecordContext();
			if(guPermLevel>7)
			{
				printf("<p><u>Change uOwner</u><br>");
				tTablePullDownResellers(uForClient);
			}
                        printf("<p>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

                case 3000:
			printf("<u>Authorize: Step 1 Tips</u><br>Depending on the user level you may authorize a contact to access "
				"interfaces (like the organization/contact portal.) Or even to use this back-office. In the 'Login' you would "
				"enter a login (that can be the same as the tClient.cLabel) for this contact and a password. The most common "
				"user permission level is 'Organization Admin' that would allow this contact to login to the idnsOrg.cgi "
				"interface and have full control over the companies DNS resource records. The second most common user level is "
				"'Back-Office Root' that will allow the user full access to this back-office  interface.<p>\n");
			if(guPermLevel>7)
				PermLevelDropDown(cuPerm);
			printf("<br>Login <input type=text title='Login to use' name=cLogin size=20 maxlength=32>\n");
			printf("<br>Passwd <input type=text title='Login passwd' name=cPasswd size=20 maxlength=15>");
			printf("<input class=largeButton title='Confirm authorization' type=submit name=gcCommand value='Confirm Authorize'>");
                break;

		default:
			if(guPermLevel>9)
			{
				uDefault=1;
				printf("<u>Table Tips</u><br>");
				printf("Here you can create new companies or add new contacts to existing companies. "
					"Once a new contact is made you can authorize her to use the interfaces or even this backoffice web interface.");
				printf("<p><u>Search Tools</u><br>");
				printf("Enter the complete or the first part of a company or contact name below. Not case sensitive. You can use %% and "
					"_ SQL LIKE matching chars. The check box further limits your search.<br>");
				printf("<input type=text title='cLabel search. Use %% . and _ for pattern matching.' name=cSearch value=\"%s\" maxlength=99"
					"size=20><br>",cSearch);
				printf("Only Orgs/Companies <input title='Limit search to Root owned tClient records that in this model are companies' "
					"type=checkbox name=uOnlyOrgs ");
				if(uOnlyOrgs)
					printf("checked><br>");
				else
					printf("><br>");

				htmlRecordContext();
			}

			if(uClient && guPermLevel>9 && uClient!=guLoginClient 
				&& !IsAuthUser(cLabel,uOwner,uClient) &&guMode!=5 && uOwner!=1)
			{
				printf("<p><input class=largeButton title='Authorize %s to manage his company resources' type=submit name=gcCommand "
					"value='Authorize'>",cLabel);
			}

			ContactsNavList();

	}

	if(!uDefault)
	{
		if(cSearch[0])
			printf("<input type=hidden name=cSearch value=\"%s\">",cSearch);
		if(uOnlyOrgs)
			printf("<input type=hidden name=uOnlyOrgs value=1>");
	}
	CloseFieldSet();


}//void ExttClientButtons(void)


void ExttClientAuxTable(void)
{

}//void ExttClientAuxTable(void)


void ExttClientGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uClient"))
		{
			sscanf(gentries[i].val,"%u",&uClient);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"uOnlyOrgs"))
		{
			sscanf(gentries[i].val,"%u",&uOnlyOrgs);
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.99s",gentries[i].val);
		}
	}
	tClient("");

}//void ExttClientGetHook(entry gentries[], int x)


void ExttClientSelect(void)
{
	unsigned uContactParentCompany=0;

	GetClientOwner(guLoginClient,&uContactParentCompany);
	GetClientOwner(uContactParentCompany,&guReseller);//Get owner of your owner...
	if(guReseller==1) guReseller=0;//...except Root companies

	if(guLoginClient==1 && guPermLevel>11)//Root can read access all
	{
		if(cSearch[0] && uOnlyOrgs)
			sprintf(gcQuery,"SELECT %s FROM tClient WHERE cLabel LIKE '%s%%' AND uOwner=1 AND uClient!=1 ORDER BY cLabel",
					VAR_LIST_tClient
					,cSearch);//Depends on Root client being uClient=1
		else if(uOnlyOrgs && !cSearch[0])
			sprintf(gcQuery,"SELECT %s FROM tClient WHERE uOwner=1 AND uClient!=1 ORDER BY cLabel",VAR_LIST_tClient);
		else if(cSearch[0])
			sprintf(gcQuery,"SELECT %s FROM tClient WHERE cLabel LIKE '%s%%' ORDER BY cLabel",VAR_LIST_tClient,cSearch);
		else if(1)
			sprintf(gcQuery,"SELECT %s FROM tClient ORDER BY uClient",VAR_LIST_tClient);
	}
	else //If you own it, the company you work for owns the company that owns it,
		//you created it, or your company owns it you can at least read access it
	{
		if(cSearch[0] && uOnlyOrgs)
			sprintf(gcQuery,"SELECT %s FROM tClient WHERE cLabel LIKE '%s%%' AND (uOwner=%u OR uOwner=%u OR uOwner=%u"
					" OR uCreatedBy=%u) AND uOwner=1 AND uClient!=1 ORDER BY cLabel",
						VAR_LIST_tClient,cSearch,guLoginClient,
						uContactParentCompany,guReseller,guLoginClient);
		else if(uOnlyOrgs && !cSearch[0])
			sprintf(gcQuery,"SELECT %s FROM tClient WHERE (uOwner=%u OR uOwner=%u OR uOwner=%u"
					" OR uCreatedBy=%u) AND uOwner=1 AND uClient!=1 ORDER BY cLabel",
						VAR_LIST_tClient,guLoginClient,
						uContactParentCompany,guReseller,guLoginClient);
		else if(cSearch[0])
			sprintf(gcQuery,"SELECT %s FROM tClient WHERE cLabel LIKE '%s%%' AND (uOwner=%u OR uOwner=%u OR uOwner=%u"
					" OR uCreatedBy=%u) ORDER BY cLabel",
						VAR_LIST_tClient,cSearch,guLoginClient,
						uContactParentCompany,guReseller,guLoginClient);
		else if(1)
			sprintf(gcQuery,"SELECT %s FROM tClientWHERE (uOwner=%u OR uOwner=%u OR uOwner=%u"
					" OR uCreatedBy=%u) ORDER BY uClient",
					VAR_LIST_tClient,guLoginClient,
					uContactParentCompany,guReseller,guLoginClient);
					
	}

}//void ExttClientSelect(void)


void ExttClientSelectRow(void)
{
	if(guPermLevel<10)
                sprintf(gcQuery,"SELECT %s FROM tClient WHERE (uOwner=%u OR uClient=%u) AND uClient=%u", VAR_LIST_tClient, guLoginClient,guLoginClient,uClient);
	else
                sprintf(gcQuery,"SELECT %s FROM tClient WHERE uClient=%u",
			VAR_LIST_tClient,uClient);

}//void ExttClientSelectRow(void)


void ExttClientListSelect(void)
{
	char cCat[512];

	if(guPermLevel<10)
		sprintf(gcQuery,"SELECT %s FROM tClient WHERE (tClient.uOwner=%u OR tClient.uClient=%u)",
				VAR_LIST_tClient,
				guLoginClient,
				guLoginClient);
	else
                sprintf(gcQuery,"SELECT %s FROM tClient",
				VAR_LIST_tClient);

	//Changes here must be reflected below in ExttClientListFilter()
        if(!strcmp(gcFilter,"uClient"))
        {
                sscanf(gcCommand,"%u",&uClient);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"uClient=%u ORDER BY uClient",
						uClient);
		strcat(gcQuery,cCat);
        }
	else if(!strcmp(gcFilter,"cLabel"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"cLabel LIKE '%s' ORDER BY cLabel",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uClient");
        }

}//void ExttClientListSelect(void)


void ExttClientListFilter(void)
{
        //Filter
        printf("<td align=right >Select ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uClient"))
                printf("<option>uClient</option>");
        else
                printf("<option selected>uClient</option>");
        if(strcmp(gcFilter,"cLabel"))
                printf("<option>cLabel</option>");
        else
                printf("<option selected>cLabel</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttClientListFilter(void)


void ExttClientNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

	printf(LANG_NBB_SKIPFIRST);
	printf(LANG_NBB_SKIPBACK);
	printf(LANG_NBB_SEARCH);

	if(guPermLevel>=8)
		printf(LANG_NBB_NEW);

	if( (guPermLevel>=8 && uOwner==guLoginClient)
			|| (guPermLevel>9 && uOwner!=1)
			|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_MODIFY);

	if( (guPermLevel>=8 && uOwner==guLoginClient)
			|| (guPermLevel>9 && uOwner!=1)
			|| (guPermLevel>7 && guReseller==guLoginClient) )
		printf(LANG_NBB_DELETE);

	printf(LANG_NBB_LIST);

	printf(LANG_NBB_SKIPNEXT);
	printf(LANG_NBB_SKIPLAST);

}//void ExttClientNavBar(void)


unsigned IsAuthUser(char *cLabel, unsigned uOwner, unsigned uCertClient)
{
	unsigned uRetVal=0;
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(uOwner==1)
		return(0);

        sprintf(gcQuery,"SELECT uPerm FROM tAuthorize WHERE (cLabel='%s' OR uCertClient=%u ) AND uOwner=%u ",cLabel,uCertClient,uOwner);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return(0);
        }
        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uRetVal);
        mysql_free_result(res);
	
	return(uRetVal);

}//unsigned IsAuthUser()


unsigned uMaxClientsReached(unsigned uClient)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned uRetVal=1;
	unsigned uMaxClients=0;
	unsigned uClients=0;

	sprintf(gcQuery,"SELECT cInfo FROM tClient WHERE uClient=%u",uClient);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                tClient(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
        {
		char *cp;

		if((cp=strstr(field[0],"uMaxClients=")))
			sscanf(cp+12,"%u",&uMaxClients);
	}
	mysql_free_result(res);

	sprintf(gcQuery,"SELECT COUNT(uClient) FROM tClient WHERE uOwner=%u",uClient);

        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                tClient(mysql_error(&gMysql));

        res=mysql_store_result(&gMysql);
        if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uClients);
	mysql_free_result(res);

	if(uClients<uMaxClients)
		uRetVal=0;

	return(uRetVal);

}//unsigned uMaxClientsReached(unsigned uClient)


void tTablePullDownResellers(int unsigned uSelector)
{
        MYSQL_RES *res;         
        MYSQL_ROW field;

        register int i,n;
    
	//8 is BO reseller 6 is Org admin
	if(guPermLevel>9)
	sprintf(gcQuery,"SELECT tClient.uClient,tClient.cLabel FROM tClient WHERE uOwner=1 ORDER BY cLabel");
	else
	sprintf(gcQuery,"SELECT tClient.uClient,tClient.cLabel FROM tClient,tAuthorize WHERE tClient.uClient=tAuthorize.uCertClient AND (tAuthorize.uPerm=8 OR tAuthorize.uPerm=6) AND (tClient.uOwner=%u OR tClient.uClient=%u) ORDER BY cLabel",guLoginClient,guLoginClient);


        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

        if(i>0)
        {
                printf("<select name=cForClientPullDown>\n");

                //Default no selection
                printf("<option>---</option>\n");

                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        field=mysql_fetch_row(res);
                        sscanf(field[0],"%u",&field0);

                        if(uSelector != field0)
                        {
                                printf("<option>%s</option>\n",field[1]);
                        }
                        else
                        {
                                printf("<option selected>%s</option>\n",field[1]);
                        }
                }
        }
        else
        {
        printf("<select name=cForClientPullDown><option>---</option></select>\n");
        }
        printf("</select>\n");

}//tTablePullDownResellers()


void PermLevelDropDown(char *cuPerm)
{
	printf("User Level ");

	printf("<select name=cuPerm>\n");

	//Allow resellers to add their customers
	if(guPermLevel>7)
	{
		printf("<option ");
		if(!strcmp(cuPerm,BO_CUSTOMER))
			printf("selected>");
		else
			printf(">");
		printf("%s</option>\n",BO_CUSTOMER);
	}

	//Allow admins to add resellers
	//and organization admins
	if(guPermLevel>9)
	{
		printf("<option ");
		if(!strcmp(cuPerm,BO_RESELLER))
			printf("selected>");
		else
			printf(">");
		printf("%s</option>\n",BO_RESELLER);

		printf("<option ");
		if(!strcmp(cuPerm,ORG_ADMIN))
			printf("selected>");
		else
			printf(">");
		printf("%s</option>\n",ORG_ADMIN);
	}

	//Allow root to add admin and other root users
	if(guPermLevel>11)
	{
		printf("<option ");
		if(!strcmp(cuPerm,BO_ADMIN))
			printf("selected>");
		else
			printf(">");
		printf("%s</option>\n",BO_ADMIN);

		printf("<option ");
		if(!strcmp(cuPerm,BO_ROOT))
			printf("selected>");
		else
			printf(">");
		printf("%s</option>\n",BO_ROOT);
	}

	printf("</select>\n");

}//void PermLevelDropDown(char *cuPerm)


const char *cUserLevel(unsigned uPermLevel)
{
	switch(uPermLevel)
	{
		case 12:
			return(BO_ROOT);
		break;
		case 10:
			return(BO_ADMIN);
		break;
		case 8:
			return(BO_RESELLER);
		break;
		case 7:
			return(BO_CUSTOMER);
		break;
		case 6:
			return(ORG_ADMIN);
		break;
		case 5:
			return(ORG_ACCT);
		break;
		case 4:
			return(ORG_SERVICE);
		break;
		case 3:
			return(ORG_SALES);
		break;
		case 2:
			return(ORG_WEBMASTER);
		break;
		case 1:
			return(ORG_CUSTOMER);
		break;
		default:
			return("---");
		break;
	}

}//const char *cUserLevel(unsigned uPermLevel)


void OrganizationDropDown(char *cuOrg)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cQuery[128];

        sprintf(cQuery,"SELECT cLabel FROM tOrganization");
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
		return;
	}
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		printf("Set Owner ");
		printf("<select name=cuOrg>\n");
		printf("<option>---</option>\n");
        	while((field=mysql_fetch_row(res)))
        	{
			printf("<option ");
			if(!strcmp(cuPerm,field[0]))
				printf("selected>");
			else
				printf(">");
			printf("%s</option>\n",field[0]);
		}
	}
	mysql_free_result(res);

	printf("</select>\n");

}//void OrganizationDropDown(char *cuOrg)


unsigned uGetOrganization(char *cLabel)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	char cQuery[128];
	unsigned uOrg=0;

        sprintf(cQuery,"SELECT uClient FROM tClient WHERE cLabel='%s'",cLabel);
        mysql_query(&gMysql,cQuery);
        if(mysql_errno(&gMysql))
        {
                fprintf(stderr,"%s",mysql_error(&gMysql));
		exit(2);
	}
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uOrg);
	mysql_free_result(res);

	return(uOrg);

}//unsigned uGetOrganization(char *cLabel)


void ContactsNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	if(guPermLevel<10 || !uClient)
		return;

	//Login info
	if(uOwner!=1)
	{
		//This covers both root alias backoffice-root level users and normal users
		//Users concept covers both contact and non contact login 'users' for any interface.
		sprintf(gcQuery,"SELECT cLabel,uAuthorize FROM tAuthorize WHERE uCertClient=%u OR (uCertClient=1 AND uOwner=%u)",uClient,uOwner);
        	mysql_query(&gMysql,gcQuery);
        	if(mysql_errno(&gMysql))
        	{
        		printf("<p><u>tAuthorize LoginNavList</u><br>\n");
			printf("%s<br>\n",mysql_error(&gMysql));
			return;
		}
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res))
		{
        		printf("<p><u>tAuthorize LoginNavList</u><br>\n");
			while((field=mysql_fetch_row(res)))
				printf("<a class=darkLink href=?gcFunction=tAuthorize&uAuthorize=%s>%s<a><br>",field[1],field[0]);
		}
        	mysql_free_result(res);
		printf("\n");
	}

	//NavList proper
	sprintf(gcQuery,"SELECT uClient,cLabel FROM tClient WHERE uOwner=%u AND uOwner!=1",uClient);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
        	printf("<p><u>ContactsNavList</u><br>\n");
                printf("%s<br>\n",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
        	printf("<p><u>ContactsNavList</u><br>\n");
		while((field=mysql_fetch_row(res)))
        	{
			printf("<a class=darkLink href=?gcFunction=tClient&uClient=%s&uOnlyOrgs=%u",field[0],uOnlyOrgs);
			if(cSearch[0])
			{
				spacetoplus(cSearch);
				printf("&cSearch=%s",cSearch);
			}
			printf(">%s</a><br>",field[1]);
        	}
	}
        mysql_free_result(res);

}//void ContactsNavList(void)


//perlSAR patch1

void htmlRecordContext(void)
{
	printf("<p><u>Record Context Info</u><br>");
	if(uOwner>1)
		printf("'%s' appears to be a contact of <a class=darkLink href=?gcFunction=tClient&uClient=%u>'%s'</a>",cLabel,uOwner,ForeignKey("tClient","cLabel",uOwner));
	else if(uOwner==1 && strcmp(cLabel,"Root"))
		printf("'%s' appears to be a company",cLabel);
	else if(uOwner==1)
		printf("'%s' appears to be a root level back-office user with no company affiliacion",cLabel);
}//void htmlRecordContext(void)
