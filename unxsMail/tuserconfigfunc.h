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

#include "mail.h"

static char cSearch[64]={""};

//External functions
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServer,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);
void UpdateUserStatus(unsigned uUser,unsigned uStatus);

void tUserConfigNavList(void);
void CreateConfigFromTemplate(unsigned uUserConfig, unsigned uUser, unsigned uConfigSpec);
void ChecktUserConfigFields(void);

//This is not used but required by libtemplate
void AppFunctions(FILE *fp,char *cFunction)
{
}

void ExtProcesstUserConfigVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",entries[i].val);
	}

}//void ExtProcesstUserConfigVars(pentry entries[], int x)


void ExttUserConfigCommands(pentry entries[], int x)
{
	char cDomain[100]={""};;
	char cServer[100]={""};;
	char cLogin[100]={""};;
	char cJobData[100]={""};;
	unsigned uServer=0;
	unsigned uDomain=0;

	if(!strcmp(gcFunction,"tUserConfigTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstUserConfigVars(entries,x);
                        	guMode=2000;
				//uConfigSpec=0;
				cLabel[0]=0;
				cConfig[0]=0;
				uUserConfig=0;
	                        tUserConfig(LANG_NB_CONFIRMNEW);
			}
			else
				tUserConfig("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstUserConfigVars(entries,x);

                        	guMode=2000;
				//Check entries here
				ChecktUserConfigFields();
                        	guMode=0;

				uUserConfig=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtUserConfig(1);
				if(!uUserConfig)
					htmlPlainTextError("Unexpected new uUserConfig error");
				CreateConfigFromTemplate(uUserConfig,uUser,uConfigSpec);

				sscanf(ForeignKey("tUser","uDomain",uUser),"%u",&uDomain);
				sscanf(ForeignKey("tUser","uServer",uUser),"%u",&uServer);
				sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
				sprintf(cServer,"%s",ForeignKey("tServer","cLabel",uServer));
				sprintf(cLogin,"%s",ForeignKey("tUser","cLogin",uUser));
				sprintf(cJobData,"uUserConfig=%u;\nuConfigSpec=%u;\n",
						uUserConfig,uConfigSpec);
				SubmitJob("NewUserConfig",cDomain,cLogin,cServer,cJobData,
						uDomain,uUser,
						guCompany,guLoginClient);
				UpdateUserStatus(uUser,STATUS_PENDING_MOD);
				tUserConfig("New record and configuration created from template. Job created");
			}
			else
				tUserConfig("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstUserConfigVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tUserConfig(LANG_NB_CONFIRMDEL);
			}
			else
				tUserConfig("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstUserConfigVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;

				sscanf(ForeignKey("tUser","uDomain",uUser),"%u",&uDomain);
				sscanf(ForeignKey("tUser","uServer",uUser),"%u",&uServer);
				sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
				sprintf(cServer,"%s",ForeignKey("tServer","cLabel",uServer));
				sprintf(cLogin,"%s",ForeignKey("tUser","cLogin",uUser));
				sprintf(cJobData,"uUserConfig=%u;\nuConfigSpec=%u;\n",
						uUserConfig,uConfigSpec);
				SubmitJob("DelUserConfig",cDomain,cLogin,cServer,cJobData,
						uDomain,uUser,
						guCompany,guLoginClient);
				UpdateUserStatus(uUser,STATUS_PENDING_MOD);
				uModBy=guLoginClient;
				DeletetUserConfig();
			}
			else
				tUserConfig("<blink>Error</blink>: Denied by permissions settings");
                }

		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstUserConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tUserConfig(LANG_NB_CONFIRMMOD);
			}
			else
				tUserConfig("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,"Regenerate cConfig"))
                {
                        ProcesstUserConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				//Check
				ChecktUserConfigFields();
				guMode=0;
				CreateConfigFromTemplate(uUserConfig,uUser,uConfigSpec);

				sscanf(ForeignKey("tUser","uDomain",uUser),"%u",&uDomain);
				sscanf(ForeignKey("tUser","uServer",uUser),"%u",&uServer);
				sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
				sprintf(cServer,"%s",ForeignKey("tServer","cLabel",uServer));
				sprintf(cLogin,"%s",ForeignKey("tUser","cLogin",uUser));
				sprintf(cJobData,"uUserConfig=%u;\nuConfigSpec=%u;\n",
						uUserConfig,uConfigSpec);
				SubmitJob("ModUserConfig",cDomain,cLogin,cServer,cJobData,
						uDomain,uUser,
						guCompany,guLoginClient);
				UpdateUserStatus(uUser,STATUS_PENDING_MOD);

				uModBy=guLoginClient;
				ModtUserConfig();
			}
			else
				tUserConfig("<blink>Error</blink>: Denied by permissions settings");
                }

		else if(!strcmp(gcCommand,"Modify cConfig"))
                {
                        ProcesstUserConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=3002;
				tUserConfig(LANG_NB_CONFIRMMOD);
			}
			else
				tUserConfig("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstUserConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=3002;
				ChecktUserConfigFields();
				if(!cConfig[0])
					tUserConfig("<blink>Error:</blink> cConfig empty!");
                        	guMode=0;

				sscanf(ForeignKey("tUser","uDomain",uUser),"%u",&uDomain);
				sscanf(ForeignKey("tUser","uServer",uUser),"%u",&uServer);
				sprintf(cDomain,"%s",ForeignKey("tDomain","cDomain",uDomain));
				sprintf(cServer,"%s",ForeignKey("tServer","cLabel",uServer));
				sprintf(cLogin,"%s",ForeignKey("tUser","cLogin",uUser));
				sprintf(cJobData,"uUserConfig=%u;\nuConfigSpec=%u;\n",
						uUserConfig,uConfigSpec);
				SubmitJob("ModUserConfig",cDomain,cLogin,cServer,cJobData,
						uDomain,uUser,
						guCompany,guLoginClient);
				UpdateUserStatus(uUser,STATUS_PENDING_MOD);

				uModBy=guLoginClient;
				ModtUserConfig();
			}
			else
				tUserConfig("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttUserConfigCommands(pentry entries[], int x)


void ExttUserConfigButtons(void)
{
	OpenFieldSet("tUserConfig Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<u>New Step 2/2</u><br>");
			printf("Before proceeding you must make sure that you started with a uUser you wish to "
				"create a config for or if creating for another user you must know the uUser code! "
				"The cConfig and cLabel will be created for you from the uConfigSpec based tTemplate selected.<p>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<u>Del Step 2/2</u><br>");
			printf("Make sure you really want this user configuration file or file section deleted. "
				"Remember it may be deleted across a cluster of servers. Think twice!<p>\n");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 3002:
                        printf("<u>Edit cConfig Step 2/2</u><br>");
			printf("Edit this per user instance of the uConfigSpec related template. Make sure "
				"you know what you are doing! Review your work and confirm below when done.<p>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;


                case 2002:
			printf("<u>Mod Fork</u><br>");
			printf("Here you can regenerate a cConfig from the same uConfigSpec and for the "
				"same uUser or actually edit the cConfig itself. Delete and new operations "
				"should be used for other changes. Confirm the selected operation below.");
                        printf("<p><input title='Replace current config' type=submit class=lwarnButton"
				" name=gcCommand value='Regenerate cConfig'>");
                        printf("<p><input title='Start an actual cConfig edit mod' type=submit class=largeButton"
				" name=gcCommand value='Modify cConfig'>");
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("Here are stored all per user configuration files or configuration "
				"file sections (shorthand, just configs.) They are created from "
				"tTemplate sources and can have different sets of template variables "
				"depending on the template name and user config type. Admins can modify "
				"them here in their entirety or recreate them from the template source "
				"(for example when a template source has been updated.) Utmost care must "
				"be taken when modifying them directly since they might not work or work "
				"in a manner not expected, causing all kinds of problems. Be warned!");
			if(uUser)
			{
				printf("<p><input title='Enter a mySQL search pattern for cLogin "
					"or a uUser code' type=text name=cSearch value='%s' maxlength=63"
					" size=20> cSearch<br>\n",cSearch);
				tUserConfigNavList();
			}
	}
	CloseFieldSet();

}//void ExttUserConfigButtons(void)


void ExttUserConfigAuxTable(void)
{

}//void ExttUserConfigAuxTable(void)


void ExttUserConfigGetHook(entry gentries[], int x)
{
	register int i;

	for(i=0;i<x;i++)
	{
		if(!strcmp(gentries[i].name,"uConfigSpec"))
		{
			sscanf(gentries[i].val,"%u",&uConfigSpec);
			guMode=2000;
		}
		else if(!strcmp(gentries[i].name,"cSearch"))
		{
			sprintf(cSearch,"%.63s",gentries[i].val);
		}
		else if(!strcmp(gentries[i].name,"uUserConfig"))
		{
			sscanf(gentries[i].val,"%u",&uUserConfig);
			guMode=6;
		}
	}
	tUserConfig("");

}//void ExttUserConfigGetHook(entry gentries[], int x)


void ExttUserConfigSelect(void)
{
	ExtSelect("tUserConfig",VAR_LIST_tUserConfig,0);
	
}//void ExttUserConfigSelect(void)


void ExttUserConfigSelectRow(void)
{
	ExtSelectRow("tUserConfig",VAR_LIST_tUserConfig,uUserConfig);

}//void ExttUserConfigSelectRow(void)


void ExttUserConfigListSelect(void)
{
	char cCat[512];

	ExtListSelect("tUserConfig",VAR_LIST_tUserConfig);
	
	//Changes here must be reflected below in ExttUserConfigListFilter()
        if(!strcmp(gcFilter,"uUserConfig"))
        {
                sscanf(gcCommand,"%u",&uUserConfig);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tUserConfig.uUserConfig=%u \
						ORDER BY uUserConfig",
						uUserConfig);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uUserConfig");
        }

}//void ExttUserConfigListSelect(void)


void ExttUserConfigListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uUserConfig"))
                printf("<option>uUserConfig</option>");
        else
                printf("<option selected>uUserConfig</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttUserConfigListFilter(void)


void ExttUserConfigNavBar(void)
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

}//void ExttUserConfigNavBar(void)


void tUserConfigNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned ucSearch=0;

	printf("<p><u>tUserConfigNavList</u><br>\n");
	
	if(!cSearch[0])
	{
        	printf("Must restrict via cSearch\n");
		return;
	}

	sscanf(cSearch,"%u",&ucSearch);

	if(ucSearch)
	{
		char cExtra[33]={""};

		sprintf(cExtra,"tUser.uUser=%u",ucSearch);
		
		ExtSelectSearch("tUserConfig","tUserConfig.uUserConfig,tUserConfig.cLabel,"
				"(SELECT tUser.cLogin FROM tUser,tUserConfig WHERE tUserConfig.uUser=tUser.uUser)",
				"tUserConfig.cLabel","",cExtra,101);
	}
	else
		ExtSelectSearch("tUserConfig","tUserConfig.uUserConfig,tUserConfig.cLabel,"
				"(SELECT tUser.cLogin FROM tUser,tUserConfig WHERE tUserConfig.uUser=tUser.uUser)",
				"tUserConfig.cLabel",cSearch,NULL,101);

	//printf("Debug:\n%s\n",gcQuery);
        mysql_query(&gMysql,gcQuery);
        if(mysql_errno(&gMysql))
        {
                printf("%s",mysql_error(&gMysql));
                return;
        }

        res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		unsigned uCount=1;

	        while((field=mysql_fetch_row(res)))
		{
			if(field[2]==NULL) continue; // Skip records like: (null)/standard procmail file, 
							//can be done using MySQL IF, but it might slow dwn the query.

			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tUserConfig&uUserConfig=%s&cSearch=%s>"
				"%s/%s</a><br>\n",field[0],cURLEncode(cSearch),field[2],field[1]);
			if( (uCount++) >= 100)
				printf("Restrict cSearch further, only 100 items listed\n");
	        }
		if(uUser)
			printf("<br><a class=darkLink href=unxsMail.cgi?gcFunction=tUser&uUser=%u>Quick link to tUser"
				"</a><br>\n",uUser);
	}
	else
		printf("No records found");
		
        mysql_free_result(res);


}//void tUserConfigNavList(void)



void CreateConfigFromTemplate(unsigned uUserConfig, unsigned uUser, unsigned uConfigSpec)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	struct t_template template;

	char cTemplateName[64]={""};
	char cLogin[64]={""};
	unsigned uDomain=0;
	char cDomain[100]={""};
	unsigned uUserType=0;
	char cUserType[33]={""};
	unsigned uServer=0;
	char cServer[64]={""};
	unsigned uStatus=0;
	char cStatus[64]={""};
	char cOwner[33]={""};
	char cCreatedBy[33]={""};

	sprintf(cTemplateName,"%.63s",ForeignKey("tConfigSpec","cLabel",uConfigSpec));
	sprintf(cLogin,"%.63s",ForeignKey("tUser","cLogin",uUser));

	sscanf(ForeignKey("tUser","uDomain",uUser),"%u",&uDomain);
	sprintf(cDomain,"%.99s",ForeignKey("tDomain","cDomain",uDomain));

	sscanf(ForeignKey("tUser","uUserType",uUser),"%u",&uUserType);
	sprintf(cUserType,"%.32s",ForeignKey("tUserType","cLabel",uUserType));

	sscanf(ForeignKey("tUser","uServer",uUser),"%u",&uServer);
	sprintf(cServer,"%.32s",ForeignKey("tServer","cLabel",uServer));

	sscanf(ForeignKey("tUser","uStatus",uUser),"%u",&uStatus);
	sprintf(cStatus,"%.32s",ForeignKey("tStatus","cLabel",uStatus));

	sprintf(cOwner,"%.32s",ForeignKey("tClient","cLabel",uOwner));
	sprintf(cCreatedBy,"%.32s",ForeignKey("tClient","cLabel",uCreatedBy));

	//Setup configuration sections
	TemplateSelect(cTemplateName);
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		//This is pure alpha template lib stuff...need to pass buffer size for safety
		char cBuffer[10000]={""};
		char cQuery[10240]={""};

		template.cpName[0]="cLogin";
		template.cpValue[0]=cLogin;

		template.cpName[1]="cDomain";
		template.cpValue[1]=cDomain;

		template.cpName[2]="cUserType";
		template.cpValue[2]=cUserType;

		template.cpName[3]="cServer";
		template.cpValue[3]=cServer;

		template.cpName[4]="cStatus";
		template.cpValue[4]=cStatus;

		template.cpName[5]="cOwner";
		template.cpValue[5]=cOwner;

		template.cpName[6]="cCreatedBy";
		template.cpValue[6]=cCreatedBy;

		template.cpName[7]="";

		TemplateToBuffer(field[0],&template,cBuffer);
		cConfig=cBuffer;
		sprintf(cLabel,"%.32s",cTemplateName);
		sprintf(cQuery,"UPDATE tUserConfig SET cConfig='%s',cLabel='%s' WHERE uUserConfig=%u",TextAreaSave(cBuffer),cTemplateName,uUserConfig);
        	mysql_query(&gMysql,cQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

}//void CreateConfigFromTemplate(unsigned uUserConfig, unsigned uUser, unsigned uConfigSpec)


void ChecktUserConfigFields(void)
{
	char const *cuUser;
	cuUser=ForeignKey("tUser","cLogin",uUser);
	if(isdigit(cuUser[0]))
		tUserConfig("<blink>Error:</blink> You must enter a uUser code for an existing user");
	if(!uUser)
		tUserConfig("<blink>Error:</blink> You must enter a valid uUser code");
	if(!uConfigSpec)
		tUserConfig("<blink>Error:</blink> You must select a valid uConfigSpec");
}//void ChecktUserConfigFields(void)
