/*
FILE
	svn ID removed
	(Built initially by unixservice.com mysqlRAD2)
PURPOSE
	Non schema-dependent table and application table related functions.
AUTHOR
	(C) 2001-2009 Gary Wallis and Hugo Urquiza.
 
*/

#include <ctype.h>

static char cSearch[64]={""};
static char cVirtualEmail[65]={""};

//External functions
void EncryptPasswdMD5(char *pw);//mainfunc.h
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);
void CreateConfigFromTemplate(unsigned uUserConfig, unsigned uUser, unsigned uConfigSpec);

//Local added beyond RAD3 functions
void tUserNavList(void);
void tUserContextInfo(void);
void BasicUserCheck(void);
void CreateConfigs(void);
void CreateVUTEntry(char *cDomain);

void ExtProcesstUserVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",entries[i].val);
		else if(!strcmp(entries[i].name,"cVirtualEmail"))
			sprintf(cVirtualEmail,"%.63s",entries[i].val);
	}
}//void ExtProcesstUserVars(pentry entries[], int x)


void ExttUserCommands(pentry entries[], int x)
{
	char cDomain[100]={""};;
	char cServerGroup[100]={""};;

	if(!strcmp(gcFunction,"tUserTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=8)
			{
				//Step 1
	                        ProcesstUserVars(entries,x);
                        	guMode=2000;
				cPasswd[0]=0;
	                        tUser("New Step 1/2");
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=8)
			{
                        	ProcesstUserVars(entries,x);

                        	guMode=2000;
				BasicUserCheck();
                        	guMode=0;

				//If MD5 hash prefix assume encrypted
				if(strncmp(cPasswd,"$1$",3))
					EncryptPasswdMD5(cPasswd);
				uStatus=1;//Pending
				uUser=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtUser(1);
				if(!uUser)
					htmlPlainTextError("Unexpected NewtUser(1) error");
				sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				SubmitJob("NewUser",cDomain,cLogin,cServerGroup,"",
						uDomain,uUser,
						guCompany,guLoginClient);
				CreateConfigs();
				if(cVirtualEmail[0])
					CreateVUTEntry(cDomain);

				tUser("User scheduled for addition");
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstUserVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tUser(LANG_NB_CONFIRMDEL);
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstUserVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;
				sprintf(gcQuery,"UPDATE tUser SET uStatus=3 WHERE uUser=%u AND uStatus=4",uUser);
        			mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
					htmlPlainTextError(mysql_error(&gMysql));
				if(mysql_affected_rows(&gMysql))
				{
					sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
					sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
					SubmitJob("DelUser",cDomain,cLogin,cServerGroup,"",
						uDomain,uUser,
						guCompany,guLoginClient);
					uStatus=3;
					unxsMailLog(uUser,"tUser","Pending Del");
					tUser("User scheduled for deletion");
				}
				else
				{
					tUser("<blink>Error:</blink> User status error");
				}
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstUserVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) && uStatus==4)
			{
				guMode=2002;
				tUser(LANG_NB_CONFIRMMOD);
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstUserVars(entries,x);
			if(uOwner) GetClientOwner(uOwner,&guReseller);
			if(uAllowMod(uOwner,uCreatedBy) && uStatus==4)
			{
				char cPrevPasswd[64]={""};
				char cPrevSecretQuestion[256]={""};
				char cPrevSecretAnswer[256]={""};
				
                        	guMode=2002;
				BasicUserCheck();
				sprintf(cPrevPasswd,"%.63s",ForeignKey("tUser","cPasswd",uUser));
				sprintf(cPrevSecretQuestion,".%255s",ForeignKey("tUser","cSecretQuestion",uUser));
				sprintf(cPrevSecretAnswer,".%255s",ForeignKey("tUser","cSecretAnswer",uUser));
				
				if(!strcmp(cPrevPasswd,cPasswd) && !strcmp(cPrevSecretQuestion,cSecretQuestion) && !strcmp(cPrevSecretAnswer,cSecretAnswer))
					tUser("<blink>Error:</blink> Nothing changed");
                        	guMode=0;

				//If MD5 hash prefix assume encrypted
				if(strncmp(cPasswd,"$1$",3))
					EncryptPasswdMD5(cPasswd);
				uStatus=2;//Pending Mod
				//Check entries here
                        	guMode=0;

				sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				SubmitJob("ModUser",cDomain,cLogin,cServerGroup,"",
						uDomain,uUser,
						guCompany,guLoginClient);
					
				uModBy=guLoginClient;
				ModtUser();
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,"Config Regeneration"))
                {
                        ProcesstUserVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy) && uStatus==4)
			{
				char cJobData[32]={""};

                        	guMode=2002;
				BasicUserCheck();
                        	guMode=0;

				CreateConfigs();

				sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"ALL\nuUserType=%u;\n",uUserType);
				SubmitJob("ModUserConfig",cDomain,cLogin,cServerGroup,cJobData,
						uDomain,uUser,
						guCompany,guLoginClient);

				uStatus=2;//Pending Mod
				uModBy=guLoginClient;
				ModtUser();
			}
			else
				tUser("<blink>Error</blink>: Denied by permissions settings");
		}
	}

}//void ExttUserCommands(pentry entries[], int x)


void ExttUserButtons(void)
{
	OpenFieldSet("tUser Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<u>New Step 1/2</u><br>");
			printf("Enter or modify the open input fields in the right panel (don't worry "
				"if previous info is in the open fields, this is for faster addition of "
				"similar accounts). To finish confirm with the button below. Your inputs "
				"will be checked. If any errors are found you will return to this entry step."
				"<p>After you create a new user the new user may have had user configuration "
				"files or (file sections) created (configs for short.) These will be listed "
				"in the record context area in this panel. You may want to customize these "
				"configs further and you can start to do so by just clicking on them."
				"<p>A virtual users table (VUT <i>tVUTEntries</i>) record may also be optionally"
				" created below.<p>\n");
                        printf(LANG_NBB_CONFIRMNEW);
			printf("<br><br><u>Optional virtual user table (VUT) Entry</u><br>\n");
			printf("cVirtualEmail <input type=text name=cVirtualEmail size=20"
				" title='Enter virtual email address without"
				" @FQDN' value='%s'><br>",cVirtualEmail);
                break;

                case 2001:
                        printf("<p><u>Think twice</u><br>");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 2002:
			printf("<u>Mod Fork</u><br>");
			printf("Active user records can either have the cPasswd changed only or have "
				"all their configs regenerated. The later mod option will replace or "
				"add all uUserType based configs to tUserConfig -for this option cPasswd "
				"will not be changed. Confirm the appropiate action below.<p><p>");
                        printf(LANG_NBB_CONFIRMMOD);
			printf("<p><input title='Will create the configs now and the needed jobs' "
				"class=lalertButton type=submit name=gcCommand value='Config Regeneration'>\n");
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("The tUser table is the basis for all email accounts. It is where you"
				" configure and deploy your mailfarm users among other management"
				" and report activities."
				"<p><i>cLogin</i> is the system mailbox user name. For example the pop3 or imap login name."
				" to access the mailbox."
				"<p><i>uDomain</i> is the domain"
				" as configured in <i>tDomain</i> that the mailbox user is associated with."
				"<p>A simple example for <i>tUser.cLogin=johnd</i> and <i>tDomain.cDomain=johnsdomain.com</i>"
				" would be used for an email address like: <i>johnd@johnsdomain.com</i>."
				"<p>A more advanced example would be to use a random mailbox user name like"
				" <i>tUser.cLogin=qazwsx</i> using the same domain as above "
				" in conjuntion with a <i>tVUTEntries</i> record. That record would map"
				" <i>johnd@johnsdomain.com</i> to the mailbox <i>qazwsx</i>."
				"<p><i>cPasswd</i> is entered in plaintext but it is encrypted/hashed and stored that way."
				" All you can do is change or reset the password. Not know what it is later."
				" This is very important for compliance with privacy/security regulations."
				"<p><i>uUserType</i> is system type that is used for specific server configuration."
				" Ask your system administrator about which one to use."
				"<p><i>uServerGroup</i> is the cluster of servers that this account will be deployed for."
				" Ask your system administrator about which one to use."
				"<p><i>cSecretQuestion</i> and <i>cSecretAnswer</i> might be used for end user interfaces"
				" for self service. A common example would be the pair <i>Name of Pet/Pluto</i>."

						);

			printf("<p><u>Record Context Info</u><br>");
			if(!uUser)
				printf("No user selected.");
			else
				tUserContextInfo();

			if(uUser)
			{
				printf("<p><input title='Enter a mySQL search pattern for cLogin or "
					"a uOwner code' type=text name=cSearch value='%s' maxlength=63 size=20> cSearch\n",
						cSearch);
				tUserNavList();

				printf("<p><a class=darkLink title='Add a tBlackList record for the loaded user' "
					"href=unxsMail.cgi?gcFunction=tBlackList&uUser=%u>Edit user blacklist settings</a>"
					"</p>\n",uUser);
				printf("<p><a class=darkLink title='Add a tWhiteList record for the loaded user' "
					"href=unxsMail.cgi?gcFunction=tWhiteList&uUser=%u>Edit user whitelist settings</a>"
					"</p>\n",uUser);
				printf("<p><a class=darkLink title='Manage user vacation mode' "
					"href=unxsMail.cgi?gcFunction=tVacation&uUser=%u>Edit user vacation settings</a>"
					"</p>\n",uUser);

				
			}
	}
	CloseFieldSet();

}//void ExttUserButtons(void)


void ExttUserAuxTable(void)
{

}//void ExttUserAuxTable(void)


void ExttUserGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uUser"))
		{
			sscanf(gentries[i].val,"%u",&uUser);
			guMode=6;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.63s",gentries[i].val);
		}
	}
	tUser("");

}//void ExttUserGetHook(entry gentries[], int x)


void ExttUserSelect(void)
{
	if(cSearch[0])
		ExtSelectSearch("tUser",VAR_LIST_tUser,"cLogin",cSearch,NULL,0);
	else
		ExtSelect("tUser",VAR_LIST_tUser,0);

}//void ExttUserSelect(void)


void ExttUserSelectRow(void)
{
	ExtSelectRow("tUser",VAR_LIST_tUser,uUser);

}//void ExttUserSelectRow(void)


void ExttUserListSelect(void)
{
	char cCat[512];

	ExtListSelect("tUser",VAR_LIST_tUser);

	//Changes here must be reflected below in ExttUserListFilter()
        if(!strcmp(gcFilter,"uUser"))
        {
		sscanf(gcCommand,"%u",&uUser);

		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
			
		sprintf(cCat,"tUser.uUser=%u \
						ORDER BY uUser",
						uUser);
		strcat(gcQuery,cCat);
        }
        else if(!strcmp(gcFilter,"cLogin"))
        {
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," WHERE ");
		else
			strcat(gcQuery," AND ");
		sprintf(cCat,"tUser.cLogin LIKE '%s%%' ORDER BY tUser.cLogin",gcCommand);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uUser");
        }

}//void ExttUserListSelect(void)


void ExttUserListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uUser"))
                printf("<option>uUser</option>");
        else
                printf("<option selected>uUser</option>");
       
       if(strcmp(gcFilter,"cLogin"))
       		 printf("<option>cLogin</option>");
       else
		printf("<option selected>cLogin</option>");
	
	if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttUserListFilter(void)


void ExttUserNavBar(void)
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

}//void ExttUserNavBar(void)


void tUserNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned ucSearch=0;

	printf("<p><u>tUserNavList</u><br>\n");
	
	if(!cSearch[0])
	{
        	printf("Must restrict via cSearch\n");
		return;
	}

	sscanf(cSearch,"%u",&ucSearch);

	if(ucSearch)
	{
		char cExtra[33]={""};

		sprintf(cExtra,"uUser=%u",ucSearch);
		ExtSelectSearch("tUser","uUser,CONCAT(tUser.cLogin,'@',(SELECT tDomain.cDomain FROM tDomain WHERE tDomain.uDomain=tUser.uDomain),'(',"
				"(SELECT tServerGroup.cLabel FROM tServerGroup WHERE tServerGroup.uServerGroup=tUser.uServerGroup),')')",
				"cLogin",cSearch,cExtra,20);
	}
	else
		ExtSelectSearch("tUser","uUser,CONCAT(tUser.cLogin,'@',(SELECT tDomain.cDomain FROM tDomain WHERE tDomain.uDomain=tUser.uDomain),'(',"
				"(SELECT tServerGroup.cLabel FROM tServerGroup WHERE tServerGroup.uServerGroup=tUser.uServerGroup),')')",
				"cLogin",cSearch,NULL,20);
//	printf("%s\n",gcQuery);return;
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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tUser&uUser=%s&cSearch=%s>%s</a><br>\n",
				field[0],cURLEncode(cSearch),field[1]);
	}
        mysql_free_result(res);

}//void tUserNavList(void)


void tUserContextInfo(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;

	//printf("pop/imap login <u>%s..%u</u><br>\n",cLogin,uDomain);

	sprintf(gcQuery,"SELECT tUserConfig.uUserConfig,tUserConfig.cLabel FROM tUserConfig,tConfigSpec "
			"WHERE tUserConfig.uConfigSpec=tConfigSpec.uConfigSpec AND uUser=%u",uUser);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		printf("<u>Configs</u><br>\n");
		while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tUserConfig&"
				"uUserConfig=%s>%s</a><br>\n",field[0],field[1]);
	}
        mysql_free_result(res);

	sprintf(gcQuery,"SELECT tJob.uJob,tJob.cJobName,tJob.cServer FROM tUser,tJob WHERE "
			"tUser.uUser=tJob.uJobTargetUser AND tUser.uUser=%u AND tJob.uJobStatus!=3",uUser);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }
        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{	
		printf("<u>Jobs</u><br>\n");
		while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tJob&uJob=%s>%s/%s</a><br>\n",
				field[0],field[1],field[2]);
	}
        mysql_free_result(res);

	sprintf(gcQuery,"SELECT uVUTEntries,cVirtualEmail,(SELECT tVUT.cDomain FROM tVUT WHERE tVUT.uVUT=tVUTEntries.uVUT) "
			"FROM tVUTEntries WHERE cTargetEmail='%s'",cLogin);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
	{
		printf("%s",mysql_error(&gMysql));
		return;
	}
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		printf("<u>VUT Entries</u><br>\n");
		while((field=mysql_fetch_row(res)))
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tVUTEntries&uVUTEntries=%s>%s@%s</a><br>\n",
				field[0],field[1],field[2]);
	}
	mysql_free_result(res);

}//void tUserContextInfo(void)


void BasicUserCheck(void)
{
	if(!cLogin[0])
		tUser("<blink>Error:</blink> cLogin is required");
	if(!isalpha(cLogin[0]))
		tUser("<blink>Error:</blink> cLogin must not start with a number");
	if(strlen(cLogin)<2)
		tUser("<blink>Error:</blink> cLogin must be at least 2 chars");
	if(strstr(cLogin,".."))
		tUser("<blink>Error:</blink> cLogin may not have two consecutive periods");
	if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
	{
		MYSQL_RES *res;
		sprintf(gcQuery,"SELECT uUser FROM tUser WHERE cLogin='%s' AND uServerGroup=%u",TextAreaSave(cLogin),uServerGroup);
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res))
			tUser("<blink>Error:</blink> cLogin already exists for selected uServerGroup");
	}
	else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
	{
		char cOldcLogin[100]={""};
		sprintf(cOldcLogin,ForeignKey("tUser","cLogin",uUser));
		if(strcmp(cLogin,cOldcLogin))
			tUser("<blink>Error:</blink> You are not allowed to modify cLogin. Delete and re-create user with new cLogin.");
	}
	if(!uDomain)
		tUser("<blink>Error:</blink> uDomain is required");
	if(!cPasswd[0])
		tUser("<blink>Error:</blink> cPasswd is required");
	if(strlen(cPasswd)<6)
		tUser("<blink>Error:</blink> cPasswd must be at least 6 chars");
	if(!uUserType)
		tUser("<blink>Error:</blink> uUserType is required");
	if(!uServerGroup)
		tUser("<blink>Error:</blink> uServerGroup is required");

}//void BasicUserCheck(void)


void CreateConfigs(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
        MYSQL_RES *res2;
        MYSQL_RES *res3;
        MYSQL_ROW field2;

	if(!uUser || !uUserType)
		htmlPlainTextError("Unexpected CreateConfigs() error 1");

	sprintf(gcQuery,"SELECT tConfiguration.cValue FROM tConfiguration,tUserType WHERE "
			"tUserType.cLabel=tConfiguration.cLabel AND tUserType.uUserType=%u",uUserType);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
                htmlPlainTextError(mysql_error(&gMysql));
        res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		char *cp;
		unsigned uConfigSpec;
		unsigned register i=0;

		cp=field[0];
		for(i=0;field[0][i];i++)
		{
			if(field[0][i]==';')
			{
				field[0][i]=0;
				sscanf(cp,"%u",&uConfigSpec);
				cp=field[0]+i+1;
				sprintf(gcQuery,"SELECT cLabel FROM tConfigSpec WHERE uConfigSpec=%u",uConfigSpec);
				mysql_query(&gMysql,gcQuery);
				if(mysql_errno(&gMysql))
                			htmlPlainTextError(mysql_error(&gMysql));
				res2=mysql_store_result(&gMysql);
				if((field2=mysql_fetch_row(res2)))
				{
					unsigned uUserConfig=0;

					sprintf(gcQuery,"SELECT uUserConfig FROM tUserConfig WHERE uUser=%u AND uConfigSpec=%u "
							"AND cConfig!='' LIMIT 1",uUser,uConfigSpec);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
                				htmlPlainTextError(mysql_error(&gMysql));
					res3=mysql_store_result(&gMysql);
					if(mysql_num_rows(res3)==0)
					{

					sprintf(gcQuery,"INSERT INTO tUserConfig SET cLabel='%.32s',uUser=%u,uConfigSpec=%u,uOwner=%u,"
							"uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
							field2[0],
							uUser,
							uConfigSpec,
							guLoginClient,
							guLoginClient);
					mysql_query(&gMysql,gcQuery);
					if(mysql_errno(&gMysql))
						htmlPlainTextError(mysql_error(&gMysql));
					uUserConfig=mysql_insert_id(&gMysql);
					if(!uUserConfig)
						htmlPlainTextError("Unexpected uUserConfig error");
					CreateConfigFromTemplate(uUserConfig,uUser,uConfigSpec);

					}
					mysql_free_result(res3);
				}
				mysql_free_result(res2);
			}
		}
	}
	mysql_free_result(res);

}//void CreateConfigs(void)


void CreateVUTEntry(char *cDomain)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	unsigned uVUT=0;
	unsigned uVUTEntries=0;
	char cServerGroup[33]={""};
	char cJobData[100]={""};

	sprintf(gcQuery,"SELECT uVUT FROM tVUT WHERE cDomain='%s'",cDomain);
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
		sscanf(field[0],"%u",&uVUT);
	
	mysql_free_result(res);
	
	if(!uVUT)
	{
		sprintf(gcQuery,"INSERT INTO tVUT SET cDomain='%s',uServerGroup=%u,uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
				cDomain
				,uServerGroup
				,guCompany
				,guLoginClient
				);
		mysql_query(&gMysql,gcQuery);

		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));

		uVUT=mysql_insert_id(&gMysql);
	}
	sprintf(gcQuery,"INSERT INTO tVUTEntries SET uVUT=%u,cTargetEmail='%s',cVirtualEmail='%s',uOwner=%u,uCreatedBy=%u,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			uVUT
			,TextAreaSave(cLogin)
			,TextAreaSave(cVirtualEmail)
			,guCompany
			,guLoginClient
		);
	
	mysql_query(&gMysql,gcQuery);

	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	

	sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
	sprintf(cJobData,"uVUTEntries=%u;\ncVirtualEmail=%s;\ncTargetEmail=%s;\ncDomain=%s;\n",
			uVUTEntries,cVirtualEmail,cLogin,cDomain);
	SubmitJob("NewVUT",cDomain,cVirtualEmail,cServerGroup,cJobData,0,0,
			guCompany,guLoginClient);
	
}//void CreateVUTEntry(char *cDomain)


