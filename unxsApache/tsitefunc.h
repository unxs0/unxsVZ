/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
 
*/

#include <openisp/template.h>
#include <ctype.h>

//This is pure alpha template lib stuff...need to pass buffer size for safety
static char cBuffer[10240]={""};
static char cBuffer2[10240]={""};
static unsigned uOptionalOwner=0;
static unsigned uIP=0;
static char cIPSelected[33]={""};
static char cSSLCert[101]={""};
static char cIPBasedSite[33]={""};
static char cVirtualHostTemplate[33]={""};
static unsigned uVirtualHostTemplate=0;
static char cSSLVirtualHostTemplate[33]={""};
static unsigned uSSLVirtualHostTemplate=0;
static char cForClientPullDown[33]={""};
static char cLogin[33]={""};
static char cPasswd[64]={""};

void tSiteNavList(void);
void tSiteContextInfo(void);
void DropDownVirtualHost(const char *cSelectName, unsigned uSelector, unsigned uSSL);
void DropDownIP(const char *cSelectName,unsigned uSelector);
void DropDownIPBasedSite(const char *cSelectName,unsigned uSelector);
int CheckParkedDomains(char *cParkedDomains);
int CheckDomainServerAvailability(char const *cDomain, unsigned uServer);
int CheckLoginServerAvailability(char const *cDomain, unsigned uServer);
int LocalModtSite(void);
void EscapeDots(char const *cDomain,char *cDomainEscDot,unsigned uMaxLength);
void SetVirtualHostSections(void);
void EncryptPasswdMD5(char *pw);
void DropDownSSLCert(const char *cSelectName,unsigned uSelector);

void tTablePullDownResellers(unsigned uSelector);//tclientfunc.h
//mainfunc.h
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServer,
		unsigned uSite,unsigned uSiteUser,unsigned uOwner,unsigned uCreatedBy);
//apache.c
void GetConfiguration(const char *cName, char *cValue, unsigned uServer, unsigned uHtml);

void ExtProcesstSiteVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cVirtualHostTemplate"))
		{
			sprintf(cVirtualHostTemplate,"%.32s",entries[i].val);
			uVirtualHostTemplate=ReadPullDown("tTemplate","cLabel",
					cVirtualHostTemplate);
		}
		else if(!strcmp(entries[i].name,"cSSLVirtualHostTemplate"))
		{
			sprintf(cSSLVirtualHostTemplate,"%.32s",entries[i].val);
			uSSLVirtualHostTemplate=ReadPullDown("tTemplate","cLabel",
					cSSLVirtualHostTemplate);
		}
		else if(!strcmp(entries[i].name,"cSSLCert"))
		{
			sprintf(cSSLCert,"%.100s",entries[i].val);
			uSSLCert=ReadPullDown("tSSLCert","cDomain",
					cSSLCert);
		}
		else if(!strcmp(entries[i].name,"cIPSelected"))
			sprintf(cIPSelected,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cIPBasedSite"))
			sprintf(cIPBasedSite,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPasswd"))
			sprintf(cPasswd,"%.63s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLogin"))
			sprintf(cLogin,"%.32s",WordToLower(entries[i].val));
		else if(!strcmp(entries[i].name,"cForClientPullDown"))
		{
			strcpy(cForClientPullDown,entries[i].val);
			uOptionalOwner=ReadPullDown("tClient","cLabel",cForClientPullDown);
		}
	}

}//void ExtProcesstSiteVars(pentry entries[], int x)


void ExttSiteCommands(pentry entries[], int x)
{
	char cServer[33]={""};

	if(!strcmp(gcFunction,"tSiteTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstSiteVars(entries,x);
                        	guMode=2000;
				cParkedDomains[0]=0;
				cIP[0]=0;
				uStatus=0;
				cVirtualHost[0]=0;
				cSSLVirtualHost[0]=0;
	                        tSite("New Step 1/3");
			}
			else
				tSite("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,"Step 2"))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstSiteVars(entries,x);
				guMode=2000;
				if(!strchr(cDomain,'.'))
					tSite("<blink>Must enter a valid FQDN cDomain</blink>");
				//Only most common, must enhance this
/*				if(!( strstr(cDomain,".net") || strstr(cDomain,".com")
						|| strstr(cDomain,".co")
						|| strstr(cDomain,".org")
						|| strstr(cDomain,".biz")
						|| strstr(cDomain,".tv")
						|| strstr(cDomain,".gov")
						|| strstr(cDomain,".info") )
										)	
					tSite("<blink>Must enter a valid FQDN cDomain</blink>");*/
				if(!cDomain[0])
					tSite("<blink>Must enter a valid FQDN cDomain</blink>");
				if(!uServer)
					tSite("<blink>Must select a uServer</blink>");
				if(CheckDomainServerAvailability(cDomain,uServer))
					tSite("<blink>cDomain already in use for selected server</blink>");
				if(!cParkedDomains[0] && uNameBased)
				{
					sprintf(cBuffer,"ServerAlias www.%s",cDomain);
					cParkedDomains=cBuffer;
				}
				if(cParkedDomains[0] && uNameBased)
				{
					if(CheckParkedDomains(cParkedDomains))
						tSite("<blink>cParkedDomains: ServerAlias must start each line</blink>");
				}

                        	guMode=3000;
				cIP[0]=0;
				cVirtualHost[0]=0;
				cSSLVirtualHost[0]=0;
	                        tSite("New Step 2/3");
			}
			else
				tSite("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,"Step 3"))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstSiteVars(entries,x);
                        	guMode=3000;
				if(!cLogin[0])
					tSite("<blink>Must enter a login</blink>");
				if(strlen(cPasswd)<6)
					tSite("<blink>Must enter a password with at least 6 characters</blink>");
				if(!isalpha(cLogin[0]))
					tSite("<blink>Login must start with a letter</blink>");
				if(!cVirtualHostTemplate[0] || cVirtualHostTemplate[0]=='-')
					tSite("<blink>Must select a cVirtualHost template</blink>");
				if(CheckLoginServerAvailability(cLogin,uServer))
					tSite("<blink>Login already in use for selected server. Come up with another.</blink>");
                        	guMode=4000;
				cIP[0]=0;
	                        tSite("New Step 3/3");
			}
			else
				tSite("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
				unsigned uSiteUser=0;

                        	ProcesstSiteVars(entries,x);

                        	guMode=2000;//Start over
				if(!cLogin[0])
					tSite("<blink>Must enter a login</blink>");
				if(strlen(cPasswd)<6)
					tSite("<blink>Must enter a password with at least 6 characters</blink>");
				if(!cVirtualHostTemplate[0] || cVirtualHostTemplate[0]=='-')
					tSite("<blink>Must select a cVirtualHost template</blink>");
				if(!strchr(cDomain,'.'))
					tSite("<blink>Must enter a valid FQDN cDomain</blink>");
				//Only most common, must enhance this
/*				if(!( strstr(cDomain,".net") || strstr(cDomain,".com")
						|| strstr(cDomain,".co")
						|| strstr(cDomain,".org")
						|| strstr(cDomain,".biz")
						|| strstr(cDomain,".tv")
						|| strstr(cDomain,".gov")
						|| strstr(cDomain,".info") )
										)	
					tSite("<blink>Must enter a valid FQDN cDomain</blink>");*/
				if(!cDomain[0])
					tSite("<blink>Must enter a valid FQDN cDomain</blink>");
				if(!uServer)
					tSite("<blink>Must select a uServer</blink>");
				if(CheckDomainServerAvailability(cDomain,uServer))
					tSite("<blink>cDomain already in use for selected server</blink>");
				if(CheckLoginServerAvailability(cLogin,uServer))
					tSite("<blink>Login already in use for selected server. Come up with another.</blink>");
				if(uNameBased && !cParkedDomains[0])
					tSite("<blink>Must enter at least one parked domain</blink>");
				if(cParkedDomains[0])
				{
					if(CheckParkedDomains(cParkedDomains))
						tSite("<blink>cParkedDomains: ServerAlias must start each line</blink>");
				}
                        	guMode=4000;
				if(uNameBased)
				{
					char *cp;

					if(!cIPBasedSite[0] || cIPBasedSite[0]=='-')
						tSite("<blink>Must select a valid cIPBasedSite</blink>");
					if((cp=strchr(cIPBasedSite,'/')))
						sprintf(cIP,"%s",IPNumber(cp+1));
					if(!cIP[0])
						tSite("<blink>Must select a valid cIP-2</blink>");
				}
				else
				{
					if(!cIPSelected[0] || cIPSelected[0]=='-')
						tSite("<blink>Must select a valid cIP-1</blink>");
					sprintf(cIP,"%s",IPNumber(cIPSelected));

					if(cSSLVirtualHostTemplate[0] && cSSLVirtualHostTemplate[0]!='-' && !uSSLCert)
						tSite("<blink>Must select a tSSLCert</blink>");
				}
				//if(!strcmp(cIP,"0.0.0.0"))
				//	tSite("<blink>Must select a valid cIP-3</blink>");

				SetVirtualHostSections();

                        	guMode=0;

				uStatus=1;//Pending
				uSite=0;
				uCreatedBy=guLoginClient;
				if(uOptionalOwner)
					uOwner=uOptionalOwner;
				else
					uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtSite(1);//Mode 1 return here

				if(!uSite)
					tSite("<blink>Unexpected error tSite-1a!</blink>");


				if(!uNameBased && cIP[0])
				{
					//Remove IP from pool
					sprintf(gcQuery,"UPDATE tIP SET uAvailable=0,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE cIP='%s'"
							,guLoginClient,cIP);
					mysql_query(&gMysql,gcQuery);
						if(mysql_errno(&gMysql))
					tSite(mysql_error(&gMysql));
				}

				//Create new tSiteUser record
				EncryptPasswdMD5(cPasswd);
				sprintf(gcQuery,"INSERT INTO tSiteUser SET cLogin='%s',cPasswd='%s',uSite=%u,uAdmin=1,uStatus=1,uOwner=%u,"
						"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",cLogin,cPasswd,uSite,uOwner,uCreatedBy);
        			mysql_query(&gMysql,gcQuery);
        			if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));

				uSiteUser=mysql_insert_id(&gMysql);
				if(!uSiteUser)
					tSite("<blink>Unexpected error tSite-1b!</blink>");
				sprintf(cServer,"%s",ForeignKey("tServer","cLabel",uServer));

				//Create new tJob entries
				SubmitJob("NewSiteUser","",cLogin,cServer,uSite,
							uSiteUser,uOwner,uCreatedBy);
				SubmitJob("NewSite",cDomain,"",cServer,uSite,
							0,uOwner,uCreatedBy);

				tSite("Site and user configured, jobs created");
			}
			else
				tSite("<blink>Error</blink>: Denied by permissions settings"); 
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstSiteVars(entries,x);
			if( uStatus==4 && uAllowDel(uOwner,uCreatedBy)) 
			{
				if(!uSite)
					tSite("<blink>No site selected</blink>");
				if(!cDomain[0])
					tSite("<blink>Must enter a valid FQDN cDomain</blink>");
				if(!uServer)
					tSite("<blink>Must select a uServer</blink>");
	                        guMode=2001;
				tSite(LANG_NB_CONFIRMDEL);
			}
			else
				tSite("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstSiteVars(entries,x);
			if( uStatus==4  && uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				if(!uSite)
					tSite("<blink>No site selected</blink>");
				if(!cDomain[0])
					tSite("<blink>No valid FQDN cDomain</blink>");
				if(!uServer)
					tSite("<blink>No valid uServer</blink>");

				guMode=5;

				//Create new tJob entries
				sprintf(cServer,"%s",ForeignKey("tServer","cLabel",uServer));
				SubmitJob("DelSite",cDomain,"",cServer,uSite,
							0,uOwner,uCreatedBy);

				//Pending Del
				sprintf(gcQuery,"UPDATE tSite SET uStatus=3,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uSite=%u",guLoginClient,uSite);
        			mysql_query(&gMysql,gcQuery);
        			if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				sprintf(gcQuery,"UPDATE tSiteUser SET uStatus=3,uModBy=%u,uModDate=UNIX_TIMESTAMP(NOW()) WHERE uSite=%u",guLoginClient,uSite);
        			mysql_query(&gMysql,gcQuery);
        			if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				unxsApacheLog(uSite,"tSite","DelJob");
				tSite("Site (and it's users) scheduled for immeditate deletion. Site files will remain for 30 days.");
			}
			else
				tSite("<blink>Error</blink>: Denied by permissions settings"); 
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstSiteVars(entries,x);
			if( (uStatus==4 || uStatus==2) && uAllowMod(uOwner,uCreatedBy)) 
			{

				guMode=2002;
				//Reload site on certain uncommon work flows
				if(!cVirtualHost[0] || !cDomain[0])
					tSite("");
				tSite("Modify Step 1/2");
			}
			else
				tSite("<blink>Error</blink>: Denied by permissions settings"); 
		}
		else if(!strcmp(gcCommand,"Modify Step 2"))
                {
                        ProcesstSiteVars(entries,x);
			if( (uStatus==4 || uStatus==2) && uAllowMod(uOwner,uCreatedBy))
			{
				//Check input here
				guMode=2002;
				//Unexpected input error section
				//Cross site script by insider(?) damage control
				if(!uSite)
					tSite("<blink>No site selected</blink>");
				if(!cDomain[0])
					tSite("<blink>Must enter a valid FQDN cDomain</blink>");
				if(!uServer)
					tSite("<blink>Must select a uServer</blink>");

				//Mandatory input for modify checking section
				if(!cVirtualHostTemplate[0] || cVirtualHostTemplate[0]=='-')
					tSite("<blink>Must select a template</blink>");
				//cSSLVirtualHost is optional unless already configured.
				if(!uNameBased && cSSLVirtualHost[0])
				{
					if( !cSSLVirtualHostTemplate[0] || cSSLVirtualHostTemplate[0]=='-' )
						tSite("<blink>Must select an SSL template</blink>");
					if( !uSSLCert )
						tSite("<blink>Must select a tSSLCert</blink>");
					sprintf(cSSLCert,"%.100s",ForeignKey("tSSLCert","cDomain",uSSLCert));
				}
				//Fails for more than one line of ServerAlias
				/*
				if(cParkedDomains[0])
				{
					if(CheckParkedDomains(cParkedDomains))
						tSite("<blink>cParkedDomains: ServerAlias must start each line</blink>");
				}
				*/
				SetVirtualHostSections();

				guMode=3002;
				//Reload site on certain uncommon work flows
				if(!cVirtualHost[0] || !cDomain[0])
					tSite("");
				tSite("Modify Step 2/2");
			}
			else
				tSite("<blink>Error</blink>: Denied by permissions settings"); 
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstSiteVars(entries,x);
			if(((uStatus==4 || uStatus==2) && uAllowMod(uOwner,uCreatedBy)))
			{
				unsigned uMySQL_Current=-1;
				unsigned uMySQLJob=0;
				unsigned uWebalizer_Current=-1;
				unsigned uWebalizerJob=0;
				char cExtStatusMessage[64]={"Unexpected execution path"};

                        	guMode=2002;
				//Check entries here
				//We only create jobs for changes to uWebalizer
				//and uMySQL
				sscanf(ForeignKey("tSite","uWebalizer",uSite),"%u",
						&uWebalizer_Current);
				if(uWebalizer_Current!=uWebalizer && uWebalizer_Current!=-1)
					uWebalizerJob=1;
				sscanf(ForeignKey("tSite","uMySQL",uSite),"%u",&uMySQL_Current);
				if(uMySQL_Current!=uMySQL && uMySQL_Current!=-1)
					uMySQLJob=1;

                        	guMode=0;

				uModBy=guLoginClient;
				uStatus=2;//Pending Mod
				sprintf(cServer,"%s",ForeignKey("tServer","cLabel",uServer));

				switch(LocalModtSite())
				{
					case 0:
						SubmitJob("ModSite",cDomain,"",
								cServer,uSite,0,uOwner,uCreatedBy);
						sprintf(cExtStatusMessage,
								"Site modified job(s) created.");
					break;

					case 1:
					case 2:
					case 3:
						tSite(gcQuery);
					break;

					case 4:
						tSite(mysql_error(&gMysql));
					break;

					default:
						tSite("Unexpected result from LocalModtSite()!");
					break;
				}

				//Create jobs
					
				/*
				if(uMySQLJob || uWebalizerJob)
				{
					if(uMySQLJob)
						SubmitJob("SiteMySQLChange",cDomain,"",
								cServer,uSite,0,uOwner,uCreatedBy);
					if(uWebalizerJob)
						SubmitJob("SiteWebalizerChange",cDomain,"",
								cServer,uSite,0,uOwner,uCreatedBy);
				}*/
				tSite(cExtStatusMessage);
			}
			else
				tSite("<blink>Error</blink>: Denied by permissions settings"); 
                }
	}

}//void ExttSiteCommands(pentry entries[], int x)


void ExttSiteButtons(void)
{
	OpenFieldSet("tSite Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<u>New: <i>Step 1</i> Tips</u><br>");
			printf("Frist you must enter a cDomain, decide if it will be an IP or named based site, "
				"select a uServer (and if a name based site set your cParkedDomains.) Much more "
				"information here: <a href=unxsApache.cgi?gcFunction=tGlossary&uGlossary=1>tSite help</a>.\n");
                        printf("<p><input type=submit class=largeButton name=gcCommand value='Step 2'>");
                break;
		case 3000:
			printf("<u>New: <i>Step 2</i> Tips</u><br>");
			printf("Now you must enter a login name and password. This will be used by the site "
				"administrator for uploading content and managing his site with optional "
				"unxsApache interfaces. The login and password should adhere to your security "
				"standards and should never be related to the site or company/contact/user name.\n");
                        printf("<p><input type=text name=cLogin maxlength=32 value='%s'> Login",
					cLogin);
                        printf("<br><input type=text name=cPasswd maxlength=32 value='%s'> Password",cPasswd);
			printf("<p>You must also select your configuration templates. The configuration can "
					"later be changed here with a modifiy operation if needed.<p>\n");
			printf("Required cVirtualHost Template<br>");
			DropDownVirtualHost("cVirtualHostTemplate",uVirtualHostTemplate,1);
			if(!uNameBased)
			{
				printf("<p>Optional cSSLVirtualHost Template<br>");
				DropDownVirtualHost("cSSLVirtualHostTemplate",uSSLVirtualHostTemplate,2);
			}
                        printf("<p><input type=submit class=largeButton name=gcCommand value='Step 3'>");
		break;
		case 4000:
			printf("<u>New: <i>Step 3</i> Tips</u><br>");
			printf("Please review your new site configuration. If everything is ok "
				"proceed to select an IP (directly or from a root site if a name "
				"based site.) If this is not a name based site and you selected a "
				"cSSLVirtualHost template then you must also select an SSL certificate "
				"from a pre-approved list (you can always change it later, so don't worry "
				"if a certificate for your FQDN is not available yet.) Then finalize by "
				"confirming with the button below, creating the site and the site user.\n");
			printf("<p>cLogin/Passwd: %s/%s<br>\n",cLogin,cPasswd);
			printf("<p>cVirtualHostTemplate:<br>%s<br>\n",cVirtualHostTemplate);
			if(!uNameBased)
			{
				printf("<p>cSSLVirtualHostTemplate:<br>%s<br>\n",
							cSSLVirtualHostTemplate);

				if(cSSLVirtualHostTemplate[0] && cSSLVirtualHostTemplate[0]!='-')
				{
					printf("<p>Select a certificate from tSSLCert<br>");
					DropDownSSLCert("cSSLCert",uSSLCert);
				}

				printf("<p>Select an IP from tIP<br>");
				DropDownIP("cIPSelected",uIP);
			}
			else
			{
				printf("<p>Select an IP from an active IP based site<br>");
				DropDownIPBasedSite("cIPBasedSite",uSite);
			}
			printf("<p><u>Optionally create for another tClient</u><br>");
			tTablePullDownResellers(uOwner);
			printf("<p>");
                        printf("<input type=hidden name=cLogin value='%s'>\n",cLogin);
                        printf("<input type=hidden name=cPasswd value='%s'>\n",cPasswd);
                        printf("<input type=hidden name=cVirtualHostTemplate value='%s'>\n",
					cVirtualHostTemplate);
			if(!uNameBased)
                        printf("<input type=hidden name=cSSLVirtualHostTemplate value='%s'>\n",
					cSSLVirtualHostTemplate);
                        printf(LANG_NBB_CONFIRMNEW);
		break;
			

                case 2001:
                        printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<u>Modify Step 1/2</u><br>");
			printf("You can now optionally change things that are open in the right record "
				"data panel and/or change things based on selections and input made here "
				"in this aux panel. You must select template(s) make sure your choice is "
				"correct since for deployed sites it may turn off important features needed. "
				"You will be able to review before comiiting to changes.\n");

			printf("<p>Select configuration template(s)<br>");
			printf("For cVirtualHost<br>");
			DropDownVirtualHost("cVirtualHostTemplate",uVirtualHostTemplate,1);
			if(!uNameBased)
			{
				printf("<br>For cSSLVirtualHost<br>");
				DropDownVirtualHost("cSSLVirtualHostTemplate",
						uSSLVirtualHostTemplate,2);
			}

			printf("<p>Optionally change site ownership<br>");
			tTablePullDownResellers(uOwner);

			printf("<p><u>Review and proceed to next step</u><br>");
                        printf("<input type=submit class=lalertButton name=gcCommand value='Modify Step 2'>");
                break;

		case 3002:
			printf("<u>Modify Step 2/2</u><br>");
			printf("<p><u>Review changes</u><br>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("The tSite table is the basis for all websites. It is where you configure "
				"and deploy your webfarm websites among many other management and report "
				"activities (<a href=unxsApache.cgi?gcFunction=tGlossary&uGlossary=1>tSite help</a>.)\n");
			printf("<p><u>Record Context Info</u><br>");
			if(!uSite)
				printf("No site selected.");
			else
				tSiteContextInfo();

			tSiteNavList();
	}
	CloseFieldSet();

}//void ExttSiteButtons(void)


void ExttSiteAuxTable(void)
{

}//void ExttSiteAuxTable(void)


void ExttSiteGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uSite"))
		{
			sscanf(gentries[i].val,"%u",&uSite);
			guMode=6;
		}
	}
	tSite("");

}//void ExttSiteGetHook(entry gentries[], int x)


void ExttSiteSelect(void)
{
	ExtSelect("tSite",VAR_LIST_tSite,0);
	
}//void ExttSiteSelect(void)


void ExttSiteSelectRow(void)
{
	ExtSelectRow("tSite",VAR_LIST_tSite,uSite);

}//void ExttSiteSelectRow(void)


void ExttSiteListSelect(void)
{
	char cCat[512];

	ExtListSelect("tSite",VAR_LIST_tSite);

	//Changes here must be reflected below in ExttSiteListFilter()
        if(!strcmp(gcFilter,"uSite"))
        {
                sscanf(gcCommand,"%u",&uSite);
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tSite.uSite=%u \
						ORDER BY uSite",
						uSite);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cDomain"))
        {
		if(guPermLevel<10)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tSite.cDomain LIKE '%s%%' ORDER BY tSite.cDomain",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uSite");
        }

}//void ExttSiteListSelect(void)


void ExttSiteListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uSite"))
                printf("<option>uSite</option>");
        else
                printf("<option selected>uSite</option>");
	
	if(strcmp(gcFilter,"cDomain"))
		printf("<option>cDomain</option>");
	else
		printf("<option selected>cDomain</option>");

        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttSiteListFilter(void)


void ExttSiteNavBar(void)
{
	if(uOwner) GetClientOwner(uOwner,&guReseller);

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


}//void ExttSiteNavBar(void)


void tSiteNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	printf("<p><u>tSiteNavList</u><br>\n");

	ExtSelect("tSite","uSite,cDomain",0);

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
			printf("<a class=darkLink href=unxsApache.cgi?gcFunction=tSite"
				"&uSite=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

}//void tSiteNavList(void)


void tSiteContextInfo(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	
	sprintf(gcQuery,"SELECT uSiteUser,cLogin,tStatus.cLabel FROM tSiteUser,tStatus WHERE tSiteUser.uStatus=tStatus.uStatus AND uSite=%u",uSite);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		printf("(tSiteUser.cLogin/uStatus)<br>\n");
		while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsApache.cgi?gcFunction=tSiteUser&uSiteUser=%s>%s/%s</a><br>\n",field[0],field[1],field[2]);
	}
        mysql_free_result(res);

}//void tSiteContextInfo(void)


void DropDownVirtualHost(const char *cSelectName, unsigned uSelector, unsigned uTemplateType)
{
        register int i,n;
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        sprintf(gcQuery,"SELECT uTemplate,cLabel FROM tTemplate WHERE uTemplateType=%u ORDER BY cLabel",uTemplateType);

        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
	mysqlRes=mysql_store_result(&gMysql);
	i=mysql_num_rows(mysqlRes);

        if(i>0)
        {
                printf("<select name=%s>\n",cSelectName);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                             printf("<option>%s</option>\n",mysqlField[1]);
                        else
                             printf("<option selected>%s</option>\n",mysqlField[1]);
                }
        }
        else
        {
		printf("<select name=%s><option title='No selection'>---</option></select>\n",cSelectName);
        }
        printf("</select>\n");

}//void DropDownVirtualHost()


void DropDownIP(const char *cSelectName,unsigned uSelector)
{
        register int i,n;
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        sprintf(gcQuery,"SELECT uIP,cIP FROM tIP WHERE uAvailable=1 AND uServer=%u",uServer);

        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
	mysqlRes=mysql_store_result(&gMysql);
	i=mysql_num_rows(mysqlRes);

        if(i>0)
        {
                printf("<select name=%s>\n",cSelectName);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                             printf("<option>%s</option>\n",mysqlField[1]);
                        else
                             printf("<option selected>%s</option>\n",mysqlField[1]);
                }
        }
        else
        {
		printf("<select name=%s><option title='No selection'>---</option></select>\n",cSelectName);
        }
        printf("</select>\n");

}//void DropDownIP(const char *cSelectName,unsigned uSelector)


void DropDownIPBasedSite(const char *cSelectName,unsigned uSelector)
{
        register int i,n;
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

	//Active
        sprintf(gcQuery,"SELECT uSite,cDomain,cIP FROM tSite WHERE uNameBased=0 AND uStatus=4");

        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
	mysqlRes=mysql_store_result(&gMysql);
	i=mysql_num_rows(mysqlRes);

        if(i>0)
        {
                printf("<select name=%s>\n",cSelectName);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                             printf("<option>%s/%s</option>\n",mysqlField[1],mysqlField[2]);
                        else
                             printf("<option selected>%s/%s</option>\n",
					     mysqlField[1],mysqlField[2]);
                }
        }
        else
        {
		//
		//If no results, the sites shall be named based to the default host
		//which is already running afterwards you install the httpd package
		//in CentOS.
		//
		printf("<select name=%s><option title='Default selection'>0.0.0.0</option></select>\n",cSelectName);
        }
        printf("</select>\n");

}//


//This not used but required by libtemplate
void AppFunctions(FILE *fp,char *cFunction)
{
}


//Sorry this is so convoluted, but it is safe and faster than heck!
int CheckParkedDomains(char *cParkedDomains)
{
	register unsigned i;
	char *cp=cParkedDomains;
	char cSave=0;

	for(i=0;i<(strlen(cParkedDomains)+1);i++)
	{
		if(cParkedDomains[i]=='\n' || cParkedDomains[i]=='\r' || cParkedDomains[i]==0)
		{
			if(cParkedDomains[i]=='\n')
				cSave='\n';
			if(cParkedDomains[i]=='\r')
				cSave='\r';
			else if(cParkedDomains[i]==0)
				cSave=0;

			cParkedDomains[i]=0;
			if(!strstr(cp,"ServerAlias "))
			{
				cParkedDomains[i]=cSave;
				return(1);
			}
			cParkedDomains[i]=cSave;
			cp=cParkedDomains+i+1;
		}
	}
	cParkedDomains[i-1]=0;
	return(0);

}//int CheckParkedDomains(char *cParkedDomains)


int CheckDomainServerAvailability(char const *cDomain, unsigned uServer)
{
        MYSQL_RES *mysqlRes;         

        sprintf(gcQuery,"SELECT uSite FROM tSite WHERE uServer=%u AND cDomain='%s'",
			uServer,cDomain);

        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
                tSite(mysql_error(&gMysql));

	mysqlRes=mysql_store_result(&gMysql);
	if(mysql_num_rows(mysqlRes))
		return(1);
	return(0);

}//int CheckDomainServerAvailability(char const *cDomain, unsigned uServer)


int CheckLoginServerAvailability(char const *cLogin, unsigned uServer)
{
        MYSQL_RES *mysqlRes;         

        sprintf(gcQuery,"SELECT tSite.uSite FROM tSite,tSiteUser WHERE "
			"tSite.uSite=tSiteUser.uSite AND tSite.uServer=%u AND tSiteUser.cLogin='%s'",uServer,cLogin);

        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));

	mysqlRes=mysql_store_result(&gMysql);
	if(mysql_num_rows(mysqlRes))
	{
		mysql_free_result(mysqlRes);
		return(1);
	}
	mysql_free_result(mysqlRes);
	return(0);

}//int CheckLoginServerAvailability(char const *cLogin, unsigned uServer)


int LocalModtSite(void)
{
	register int i=0;
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uPreModDate=0;

	//Mod select gcQuery
	sprintf(gcQuery,"SELECT uSite,uModDate FROM tSite WHERE uSite=%u"
						,uSite);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	i=mysql_num_rows(res);

	if(i<1)
	{
		sprintf(gcQuery,LANG_NBR_RECNOTEXIST);
		mysql_free_result(res);
		return(1);
	}
	if(i>1)
	{
		sprintf(gcQuery,LANG_NBR_MULTRECS);
		mysql_free_result(res);
		return(2);
	}

	field=mysql_fetch_row(res);
	sscanf(field[1],"%u",&uPreModDate);
	if(uPreModDate!=uModDate)
	{
		sprintf(gcQuery,LANG_NBR_EXTMOD);
		mysql_free_result(res);
		return(3);
	}
	Update_tSite(field[0]);
	mysql_free_result(res);

	if(mysql_errno(&gMysql))
		return(4);
	uModDate=luGetModDate("tSite",uSite);
	unxsApacheLog(uSite,"tSite","Mod");

	return(0);

}//int LocalModtSite(void)


void EscapeDots(char const *cInput,char *cOutput,unsigned uMaxLength)
{
    register int x,y,l;
    char cFormat[16]={"%.99s"};

    sprintf(cFormat,"%%.%us",uMaxLength-1);

    sprintf(cOutput,cFormat,cInput);

    l=strlen(cOutput);
    for(x=0;cOutput[x];x++)
    {
        if(cOutput[x]=='.')
	{
            for(y=l+1;y>x;y--)
                cOutput[y] = cOutput[y-1];
            l++; /* length has been increased */
            cOutput[x] = '\\';
            x++; /* skip the character */
        }

	if(x>=uMaxLength)
	{
		cOutput[x]=0;
		return;
	}
    }

}//void EscapeDots(char const *cInput,char *cOutput,unsigned uMaxLength)


void SetVirtualHostSections(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	struct t_template template;
	char cDomainEscDot[100]={""};
	char cWebRoot[256]={"/var/www/unxsapache/sites"};
	char cCGI[256]={"cgi-bin"};
	char cWebmaster[256]={"webmaster"};
	char cCondListen[64]={""};
	//Combined cert and key pem. Installed by apache.c
	char cSSLCertFile[256]={"/var/local/web/myhostingco.com/conf/www.myhostingco.com.pem"};

	GetConfiguration("cWebRoot",cWebRoot,0,1);
	GetConfiguration("cCGI",cCGI,0,1);
	GetConfiguration("cWebmaster",cWebmaster,0,1);
	EscapeDots(cDomain,cDomainEscDot,100);
	if(!uNameBased)
		sprintf(cSSLCertFile,"%.120s/%.64s/conf/%.64s.pem",
						cWebRoot,cDomain,cSSLCert);

	//Setup configuration sections
	TemplateSelect(cVirtualHostTemplate);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		template.cpName[0]="cDomain";
		template.cpValue[0]=cDomain;

		template.cpName[1]="cDomainEscDot";
		template.cpValue[1]=cDomainEscDot;

		template.cpName[2]="cIP";
		template.cpValue[2]=cIP;

		template.cpName[3]="cWebRoot";
		template.cpValue[3]=cWebRoot;

		template.cpName[4]="cCGI";
		template.cpValue[4]=cCGI;

		template.cpName[5]="cWebmaster";
		template.cpValue[5]=cWebmaster;

		if(!uNameBased)
		{
			sprintf(cCondListen,"NameVirtualHost %.15s:80\nListen %.15s:80",
					cIP,cIP);
			template.cpName[6]="cCondListen";
			template.cpValue[6]=cCondListen;

			template.cpName[7]="cSSLCertFile";
			template.cpValue[7]=cSSLCertFile;

			template.cpName[8]="cParkedDomains";
			template.cpValue[8]=cParkedDomains;

			template.cpName[9]="";
		}
		else
		{
			template.cpName[6]="cParkedDomains";
			template.cpValue[6]=cParkedDomains;

			template.cpName[7]="";
		}

		TemplateToBuffer(field[0],&template,cBuffer);
		cVirtualHost=cBuffer;
	}
	mysql_free_result(res);

	if(!uNameBased && cSSLVirtualHostTemplate[0] && cSSLVirtualHostTemplate[0]!='-')
	{
		TemplateSelect(cSSLVirtualHostTemplate);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{

			template.cpName[0]="cDomain";
			template.cpValue[0]=cDomain;

			template.cpName[1]="cIP";
			template.cpValue[1]=cIP;

			template.cpName[2]="cDomainEscDot";
			template.cpValue[2]=cDomainEscDot;

			template.cpName[3]="cWebRoot";
			template.cpValue[3]=cWebRoot;

			template.cpName[4]="cCGI";
			template.cpValue[4]=cCGI;

			template.cpName[5]="cWebmaster";
			template.cpValue[5]=cWebmaster;

			//sprintf(cCondListen,"Listen %s:443",cIP);
			sprintf(cCondListen,"NameVirtualHost %.15s:443\nListen %.15s:443",
					cIP,cIP);
			template.cpName[6]="cCondListen";
			template.cpValue[6]=cCondListen;

			template.cpName[7]="cSSLCertFile";
			template.cpValue[7]=cSSLCertFile;

			template.cpName[8]="";

			TemplateToBuffer(field[0],&template,cBuffer2);
			cSSLVirtualHost=cBuffer2;
		}
		mysql_free_result(res);
	}

}//void SetVirtualHostSections(void)


//Passwd stuff
static unsigned char itoa64[] =         /* 0 ... 63 => ascii - 64 */
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void to64(s, v, n)
  register char *s;
  register long v;
  register int n;
{
    while (--n >= 0) {
        *s++ = itoa64[v&0x3f];
        v >>= 6;
    }
}//void to64(s, v, n)


void EncryptPasswdMD5(char *pw)
{
	char cSalt[] = "$1$01234567$";
        char *cpw;

    	(void)srand((int)time((time_t *)NULL));
    	to64(&cSalt[3],rand(),8);
	
	cpw = crypt(pw,cSalt);
	strcpy(pw,cpw);

}//void EncryptPasswdMD5(char *pw)
//End passwd stuff ;)


void DropDownSSLCert(const char *cSelectName,unsigned uSelector)
{
        register int i,n;
        MYSQL_RES *mysqlRes;         
        MYSQL_ROW mysqlField;

        sprintf(gcQuery,"SELECT uSSLCert,cDomain FROM tSSLCert");

        mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
	mysqlRes=mysql_store_result(&gMysql);
	i=mysql_num_rows(mysqlRes);

        if(i>0)
        {
                printf("<select name=%s>\n",cSelectName);

                //Default no selection
                printf("<option title='No selection'>---</option>\n");

                for(n=0;n<i;n++)
                {
                        int unsigned field0=0;

                        mysqlField=mysql_fetch_row(mysqlRes);
                        sscanf(mysqlField[0],"%u",&field0);

                        if(uSelector != field0)
                             printf("<option>%s</option>\n",mysqlField[1]);
                        else
                             printf("<option selected>%s</option>\n",mysqlField[1]);
                }
        }
        else
        {
		printf("<select name=%s><option title='No selection'>---</option></select>\n",cSelectName);
        }
        printf("</select>\n");

}//void DropDownSSLCert(const char *cSelectName,unsigned uSelector)

