/*
FILE 
	ticket.c
	$Id$
AUTHOR
	(C) 2007-2009 Gary Wallis and Hugo Urquiza for Unixservice
PURPOSE
	ispHelp  Interface
	program file.
*/


#include "interface.h"


void ProcessRegisterVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
	//	if(!strcmp(entries[i].name,"uRegister"))
	}

}//void ProcessUserVars(pentry entries[], int x)


void RegisterGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{
	//	if(!strcmp(gentries[i].name,"uRegister"))
	}

	htmlRegister();

}//void RegisterGetHook(entry gentries[],int x)


void RegisterCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Register"))
	{
		ProcessRegisterVars(entries,x);
		//if(!strcmp(gcFunction,"Submit Comment"))

		htmlRegister();
	}

}//void RegisterCommands(pentry entries[], int x)


void htmlRegister(void)
{
	htmlHeader("unxsISP CRM","Header");
	htmlRegisterPage("","Ticket.Body");
	htmlFooter("Footer");

}//void htmlRegister(void)


void htmlRegisterPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
        	MYSQL_RES *res;
	        MYSQL_ROW field;
		
		TemplateSelect(cTemplateName,guTemplateSet);
		res=mysql_store_result(&gMysql);
		if((field=mysql_fetch_row(res)))
		{
			struct t_template template;
			
			template.cpName[0]="cTitle";
			template.cpValue[0]=cTitle;
			
			template.cpName[1]="cCGI";
			template.cpValue[1]="ispHelp.cgi";
			
			template.cpName[2]="gcLogin";
			template.cpValue[2]=gcLogin;

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

			printf("\n<!-- Start htmlRegisterPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlRegisterPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s</font>\n",cTemplateName);
		}
		mysql_free_result(res);
	}

}//void htmlRegisterPage()

