/*
FILE 
	spam.c
	svn ID removed
AUTHOR
	(C) 2009 Hugo Urquiza for Unixservice
PURPOSE
	unxsMail User Interface Interface
	program file.
	This module handles the management of the user
	SA preferences
*/

#include "interface.h"

char *cSpamSettings="";

void LoadSpamSettings(void);
void CreateSpamSettings(void);
void UpdateSpamSettings(void);
unsigned ValidateSpamSettingsInput(void);

void ProcessSpamSettingsVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		if(!strcmp(entries[i].name,"cSpamSettings"))
			cSpamSettings=entries[i].val;

	}

}//void ProcessSpamSettingsVars(pentry entries[], int x)


void SpamSettingsGetHook(entry gentries[],int x)
{
	/*
	register int i;
	
	for(i=0;i<x;i++)
	{
	}
	*/
	LoadSpamSettings();
	htmlSpamSettings();
}//void SpamSettingsGetHook(entry gentries[],int x)


void SpamSettingsCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"SpamSettings"))
	{
		ProcessSpamSettingsVars(entries,x);
		if(!strcmp(gcFunction,"Update Settings"))
		{
			MYSQL_RES *res;
			MYSQL_ROW field;
			char cJobData[100]={""};

			UpdateSpamSettings();
			
			sprintf(gcQuery,"SELECT uUserConfig,uConfigSpec FROM tUserConfig WHERE "
					"cLabel='standard spamassassin file' AND uUser=%u",
					guLoginClient);
			mysql_query(&gMysql,gcQuery);
			if(mysql_errno(&gMysql))
				htmlPlainTextError(mysql_error(&gMysql));
			res=mysql_store_result(&gMysql);
			if((field=mysql_fetch_row(res)))
				sprintf(cJobData,"uUserConfig=%s;\nuConfigSpec=%s;\n",field[0],field[1]);
			MySubmitJob("ModUserConfig",cJobData);

		}
		htmlSpamSettings();
	}

}//void SpamSettingsCommands(pentry entries[], int x)


void htmlSpamSettings(void)
{
	htmlHeader("unxsMail User Interface","Header");
	htmlSpamSettingsPage("unxsMail User Interface","SpamSettings.Body");
	htmlFooter("Footer");

}//void htmlSpamSettings(void)


void htmlSpamSettingsPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;

		TemplateSelect(cTemplateName);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="unxsMailUser.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcUser;

			template.cpName[3]="gcName";
			template.cpValue[3]=gcName;

			template.cpName[4]="gcOrgName";
			template.cpValue[4]=gcOrgName;

			template.cpName[5]="cUserLevel";
			template.cpValue[5]=(char *)cUserLevel(guPermLevel);

			template.cpName[6]="gcHost";
			template.cpValue[6]=gcHost;

			template.cpName[7]="gcMessage";
			template.cpValue[7]=gcMessage;

			template.cpName[8]="cSpamSettings";
			template.cpValue[8]=cSpamSettings;

			template.cpName[9]="";

			printf("\n<!-- Start htmlSpamSettingsPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlSpamSettingsPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlSpamSettingsPage()


unsigned ValidateSpamSettingsInput(void)
{
	return(1);

}//unsigned ValidateInput(void)


void LoadSpamSettings(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	//uConfigSpec=2: std spamassassin file
	sprintf(gcQuery,"SELECT cConfig FROM tUserConfig WHERE uConfigSpec=2 AND uUser=%u",guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if((field=mysql_fetch_row(res)))
	{
		cSpamSettings=field[0];
		gcMessage="Your spam settings were loaded OK";
	}
	else
	{
		CreateSpamSettings();
	}

}//void LoadSpamSettings(void)


void CreateSpamSettings(void)
{
	sprintf(gcQuery,"INSERT INTO tUserConfig SET uConfigSpec=2,uUser=%1$u,"
			"cLabel='standard spamassassin file',"
			"uCreatedBy=(SELECT uCreatedBy FROM tUser WHERE uUser=%1$u),"
			"uOwner=(SELECT uOwner FROM tUser WHERE uUser=%1$u),"
			"cConfig=(SELECT cTemplate FROM tTemplate WHERE cLabel='standard spamassassin file'),"
			"uCreatedDate=UNIX_TIMESTAMP(NOW())",
			guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	LoadSpamSettings();

}//void CreateSpamSettings(void)


void UpdateSpamSettings(void)
{
	sprintf(gcQuery,"UPDATE tUserConfig SET cConfig='%s',uModBy=1,"
			"uModDate=UNIX_TIMESTAMP(NOW()) WHERE "
			"cLabel='standard spamassassin file' AND uUser=%u",
			TextAreaSave(cSpamSettings)
			,guLoginClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	if(mysql_affected_rows(&gMysql))
		gcMessage="Your spam settings were correctly updated";
	else
		gcMessage="Your spam settings were not updated. Contact support ASAP";

}//void UpdateSpamSettings(void)
