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
void SubmitJob(char *cJobName,char *cDomain,char *cLogin,char *cServerGroup,char *cJobData,
		unsigned uJobTarget,unsigned uJobTargetUser,unsigned uOwner,unsigned uCreatedBy);

void tServerConfigNavList(void);
void CreateServerConfigFromTemplate(unsigned uServerConfig, unsigned uServerGroup, unsigned uConfigSpec);
void ChecktServerConfigFields(void);

void ExtProcesstServerConfigVars(pentry entries[], int x)
{
	register int i;
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSearch"))
			sprintf(cSearch,"%.63s",entries[i].val);
	}
}//void ExtProcesstServerConfigVars(pentry entries[], int x)


void ExttServerConfigCommands(pentry entries[], int x)
{
	char cServerGroup[100]={""};;
	char cJobData[100]={""};;

	if(!strcmp(gcFunction,"tServerConfigTools"))
	{
		//ModuleFunctionProcess()

		if(!strcmp(gcCommand,LANG_NB_NEW))
                {
			if(guPermLevel>=10)
			{
	                        ProcesstServerConfigVars(entries,x);
                        	guMode=2000;
				//uConfigSpec=0;
				cLabel[0]=0;
				cConfig[0]=0;
				uServerConfig=0;
	                        tServerConfig(LANG_NB_CONFIRMNEW);
			}
			else
				tServerConfig("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,LANG_NB_CONFIRMNEW))
                {
			if(guPermLevel>=10)
			{
                        	ProcesstServerConfigVars(entries,x);

                        	guMode=2000;
				//Check entries here
				ChecktServerConfigFields();
                        	guMode=0;

				uServerConfig=0;
				uCreatedBy=guLoginClient;
				uOwner=guCompany;
				uModBy=0;//Never modified
				uModDate=0;//Never modified
				NewtServerConfig(1);
				if(!uServerConfig)
					htmlPlainTextError("Unexpected new uServerConfig error");
				CreateServerConfigFromTemplate(uServerConfig,uServerGroup,uConfigSpec);

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uServerConfig=%u;\nuConfigSpec=%u;\n",
						uServerConfig,uConfigSpec);
				SubmitJob("NewServerConfig","AllDomains","AllUsers",
							cServerGroup,cJobData,0,0,
							guCompany,guLoginClient);
				tServerConfig("New record and configuration created from template. Job created");
			}
			else
				tServerConfig("<blink>Error</blink>: Denied by permissions settings");
		}
		else if(!strcmp(gcCommand,LANG_NB_DELETE))
                {
                        ProcesstServerConfigVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
	                        guMode=2001;
				tServerConfig(LANG_NB_CONFIRMDEL);
			}
			else
				tServerConfig("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMDEL))
                {
                        ProcesstServerConfigVars(entries,x);
			if(uAllowDel(uOwner,uCreatedBy))
			{
				guMode=5;

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uServerConfig=%u;\nuConfigSpec=%u;\n",
						uServerConfig,uConfigSpec);
				SubmitJob("DelServerConfig","AllDomains","AllUsers",
						cServerGroup,cJobData,0,0,
						guCompany,guLoginClient);
				uModBy=guLoginClient;
				DeletetServerConfig();
			}
			else
				tServerConfig("<blink>Error</blink>: Denied by permissions settings");
                }

		else if(!strcmp(gcCommand,LANG_NB_MODIFY))
                {
                        ProcesstServerConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				tServerConfig(LANG_NB_CONFIRMMOD);
			}
			else
				tServerConfig("<blink>Error</blink>: Denied by permissions settings");
                }
		else if(!strcmp(gcCommand,"Regenerate cConfig"))
                {
                        ProcesstServerConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=2002;
				//Check
				ChecktServerConfigFields();
				guMode=0;
				CreateServerConfigFromTemplate(uServerConfig,uServerGroup,uConfigSpec);

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uServerConfig=%u;\nuConfigSpec=%u;\n",
						uServerConfig,uConfigSpec);
				SubmitJob("ModServerConfig","AllDomains","AllUsers",
						cServerGroup,cJobData,0,0,
						guCompany,guLoginClient);

				uModBy=guLoginClient;
				ModtServerConfig();
			}
			else
				tServerConfig("<blink>Error</blink>: Denied by permissions settings");
                }

		else if(!strcmp(gcCommand,"Modify cConfig"))
                {
                        ProcesstServerConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
				guMode=3002;
				tServerConfig(LANG_NB_CONFIRMMOD);
			}
			else
				tServerConfig("<blink>Error</blink>: Denied by permissions settings");
                }
                else if(!strcmp(gcCommand,LANG_NB_CONFIRMMOD))
                {
                        ProcesstServerConfigVars(entries,x);
			if(uAllowMod(uOwner,uCreatedBy))
			{
                        	guMode=3002;
				ChecktServerConfigFields();
				if(!cConfig[0])
					tServerConfig("<blink>Error:</blink> cConfig empty!");
                        	guMode=0;

				sprintf(cServerGroup,"%s",ForeignKey("tServerGroup","cLabel",uServerGroup));
				sprintf(cJobData,"uServerConfig=%u;\nuConfigSpec=%u;\n",
						uServerConfig,uConfigSpec);
				SubmitJob("ModServerConfig","AllDomains","AllUsers",
						cServerGroup,cJobData,0,0,
						guCompany,guLoginClient);

				uModBy=guLoginClient;
				ModtServerConfig();
			}
			else
				tServerConfig("<blink>Error</blink>: Denied by permissions settings");
                }
	}

}//void ExttServerConfigCommands(pentry entries[], int x)


void ExttServerConfigButtons(void)
{
	OpenFieldSet("tServerConfig Aux Panel",100);
	switch(guMode)
        {
                case 2000:
			printf("<u>New Step 2/2</u><br>");
			printf("Before proceeding you must make sure that you started with a uServerGroup you wish "
				"to create a config for or if creating for another server you must select it now! "
				"The cConfig and cLabel will be created for you from the uConfigSpec based tTemplate selected.<p>");
                        printf(LANG_NBB_CONFIRMNEW);
                break;

                case 2001:
                        printf("<u>Del Step 2/2</u><br>");
			printf("Make sure you really want this server configuration file or file section deleted. " 
				"Remember it may be deleted across a cluster of servers. Think twice!<p>\n");
                        printf(LANG_NBB_CONFIRMDEL);
                break;

                case 3002:
                        printf("<u>Edit cConfig Step 2/2</u><br>");
			printf("Edit this per server instance of the uConfigSpec related template."
				" Make sure you know what you are doing! Review your work and confirm below when done.<p>");
                        printf(LANG_NBB_CONFIRMMOD);
                break;


                case 2002:
			printf("<u>Mod Fork</u><br>");
			printf("Here you can regenerate a cConfig from the same uConfigSpec and for the "
				"same uServerGroup or actually edit the cConfig itself. Delete and new "
				"operations should be used for other changes. Confirm the selected operation below.");
                        printf("<p><input title='Replace current config' type=submit class=lwarnButton name=gcCommand value='Regenerate cConfig'>");
                        printf("<p><input title='Start an actual cConfig edit mod' type=submit class=largeButton name=gcCommand value='Modify cConfig'>");
                break;

		default:
			printf("<u>Table Tips</u><br>");
			printf("Here are stored all per server configuration files or configuration "
				"file sections (shorthand, just configs.) They are created from "
				"tTemplate sources and can have different sets of template variables "
				"depending on the template name and user config type. Admins can modify "
				"them here in their entirety or recreate them from the template source "
				"(for example when a template source has been updated.) Utmost care must "
				"be taken when modifying them directly since they might not work or work "
				"in a manner not expected, causing all kinds of problems. Be warned!");
			if(uServerGroup)
			{
				printf("<p><input title='Enter a mySQL search pattern for cServerGroup or "
					"a uServerGroup code' type=text name=cSearch value='%s' maxlength=63 size=20> cSearch<br>\n",cSearch);
				tServerConfigNavList();
			}
	}
	CloseFieldSet();

}//void ExttServerConfigButtons(void)


void ExttServerConfigAuxTable(void)
{

}//void ExttServerConfigAuxTable(void)


void ExttServerConfigGetHook(entry gentries[], int x)
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
		else if(!strcmp(gentries[i].name,"uServerConfig"))
		{
			sscanf(gentries[i].val,"%u",&uServerConfig);
			guMode=6;
		}
	}
	tServerConfig("");

}//void ExttServerConfigGetHook(entry gentries[], int x)


void ExttServerConfigSelect(void)
{
	ExtSelect("tServerConfig",VAR_LIST_tServerConfig,0);
	
}//void ExttServerConfigSelect(void)


void ExttServerConfigSelectRow(void)
{
	ExtSelectRow("tServerConfig",VAR_LIST_tServerConfig,uServerConfig);

}//void ExttServerConfigSelectRow(void)


void ExttServerConfigListSelect(void)
{
	char cCat[512];

	ExtListSelect("tServerConfig",VAR_LIST_tServerConfig);
	
	//Changes here must be reflected below in ExttServerConfigListFilter()
        if(!strcmp(gcFilter,"uServerConfig"))
        {
                sscanf(gcCommand,"%u",&uServerConfig);
		if(guLoginClient==1 && guPermLevel>11)
			strcat(gcQuery," AND ");
		else
			strcat(gcQuery," WHERE ");
		sprintf(cCat,"tServerConfig.uServerConfig=%u \
						ORDER BY uServerConfig",
						uServerConfig);
		strcat(gcQuery,cCat);
        }
        else if(1)
        {
                //None NO FILTER
                strcpy(gcFilter,"None");
		strcat(gcQuery," ORDER BY uServerConfig");
        }

}//void ExttServerConfigListSelect(void)


void ExttServerConfigListFilter(void)
{
        //Filter
        printf("&nbsp;&nbsp;&nbsp;Filter on ");
        printf("<select name=gcFilter>");
        if(strcmp(gcFilter,"uServerConfig"))
                printf("<option>uServerConfig</option>");
        else
                printf("<option selected>uServerConfig</option>");
        if(strcmp(gcFilter,"None"))
                printf("<option>None</option>");
        else
                printf("<option selected>None</option>");
        printf("</select>");

}//void ExttServerConfigListFilter(void)


void ExttServerConfigNavBar(void)
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

}//void ExttServerConfigNavBar(void)


void tServerConfigNavList(void)
{
        MYSQL_RES *res;
        MYSQL_ROW field;
	unsigned ucSearch=0;

	printf("<p><u>tServerConfigNavList</u><br>\n");
	
	if(!cSearch[0])
	{
        	printf("Must restrict via cSearch\n");
		return;
	}

	sscanf(cSearch,"%u",&ucSearch);

	if(ucSearch)
	{
		char cExtra[33]={""};

		sprintf(cExtra,"tServerGroup.uServerGroup=%u",ucSearch);
		ExtSelectSearch("tServerConfig","tServerConfig.uServerConfig,tServerConfig.cLabel,"
				"(SELECT tServerGroup.cLabel FROM tServerGroup WHERE tServerConfig.uServerGroup=tServerGroup.uServerGroup)",
				"tServerConfig.cLabel","",cExtra,101);
	}
	else
		ExtSelectSearch("tServerConfig","tServerConfig.uServerConfig,tServerConfig.cLabel,"
				"(SELECT tServerGroup.cLabel FROM tServerGroup WHERE tServerConfig.uServerGroup=tServerGroup.uServerGroup)",
				"tServerConfig.cLabel",cSearch,NULL,101);

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
			printf("<a class=darkLink href=unxsMail.cgi?gcFunction=tServerConfig"
					"&uServerConfig=%s&cSearch=%s>%s/%s</a><br>\n",field[0],cURLEncode(cSearch),field[2],field[1]);
			if( (uCount++) >= 100)
				printf("Restrict cSearch further, only 100 items listed\n");
	        }
		if(uServerGroup)
		printf("<br><a class=darkLink href=unxsMail.cgi?gcFunction=tServerGroup&uServerGroup=%u>Quick link to tServerGroup</a><br>\n",uServerGroup);
	}
        mysql_free_result(res);


}//void tServerConfigNavList(void)



void CreateServerConfigFromTemplate(unsigned uServerConfig, unsigned uServerGroup, unsigned uConfigSpec)
{
	MYSQL_RES *res;
	MYSQL_ROW field;
	struct t_template template;

	char cTemplateName[64]={""};
	char cServerGroup[64]={""};
	char cOwner[33]={""};
	char cCreatedBy[33]={""};

	sprintf(cTemplateName,"%.63s",ForeignKey("tConfigSpec","cLabel",uConfigSpec));

	sprintf(cServerGroup,"%.32s",ForeignKey("tServerGroup","cLabel",uServerGroup));

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

		template.cpName[1]="cLabel";
		template.cpValue[1]=cLabel;

		template.cpName[2]="cConfigSpec";
		template.cpValue[2]=cTemplateName;

		template.cpName[3]="cServerGroup";
		template.cpValue[3]=cServerGroup;

		template.cpName[4]="cOwner";
		template.cpValue[4]=cOwner;

		template.cpName[5]="cCreatedBy";
		template.cpValue[5]=cCreatedBy;

		template.cpName[6]="";

		TemplateToBuffer(field[0],&template,cBuffer);
		cConfig=cBuffer;
		sprintf(cLabel,"%.32s",cTemplateName);
		sprintf(cQuery,"UPDATE tServerConfig SET cConfig='%s',cLabel='%s' WHERE uServerConfig=%u",TextAreaSave(cBuffer),cTemplateName,uServerConfig);
        	mysql_query(&gMysql,cQuery);
        	if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
	}
	mysql_free_result(res);

}//void CreateServerConfigFromTemplate(unsigned uServerConfig, unsigned uServerGroup, unsigned uConfigSpec)


void ChecktServerConfigFields(void)
{
	if(!uServerGroup)
		tServerConfig("<blink>Error:</blink> You must select a valid uServerGroup");
	if(!uConfigSpec)
		tServerConfig("<blink>Error:</blink> You must select a valid uConfigSpec");

}//void ChecktServerConfigFields(void)
