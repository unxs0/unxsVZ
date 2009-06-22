/*
FILE 
	spam.c
	$Id$
AUTHOR
	(C) 2009 Hugo Urquiza for Unixservice
PURPOSE
	unxsMail User Interface Interface
	program file.
	This module handles the management of the user
	SA preferences
*/

#include "interface.h"


void ProcessSpamSettingsVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
		//if(!strcmp(entries[i].name,"cPassword"))
		//	sprintf(cPasswd,"%.65s",entries[i].val);

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
	htmlSpamSettings();
}//void SpamSettingsGetHook(entry gentries[],int x)


void SpamSettingsCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"SpamSettings"))
	{
		ProcessSpamSettingsVars(entries,x);
		if(!strcmp(gcFunction,"Update your password"))
		{
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

			template.cpName[8]="";

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

