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


static char cFirstName[33]={""};
static char *cFirstNameStyle="type_fields";

static char cLastName[33]={""};
static char *cLastNameStyle="type_fields";

static char cEmail[101]={""};
static char *cEmailStyle="type_fields";

static char cTelephone[33]={""};
static char *cTelephoneStyle="type_fields";

static char cMobile[33]={""};
static char *cMobileStyle="type_fields";

static char cFax[33]={""};
static char *cFaxStyle="type_fields";

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
	htmlRegisterPage("","Register.Body");
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
			
			template.cpName[8]="cFirstName";
			template.cpValue[8]=cFirstName;

			template.cpName[9]="cFirstNameStyle";
			template.cpValue[9]=cFirstNameStyle;

			template.cpName[10]="cLastName";
			template.cpValue[10]=cLastName;

			template.cpName[11]="cLastNameStyle";
			template.cpValue[11]=cLastNameStyle;

			template.cpName[12]="cEmail";
			template.cpValue[12]=cEmail;

			template.cpName[13]="cEmailStyle";
			template.cpValue[13]=cEmailStyle;

			template.cpName[14]="cTelephone";
			template.cpValue[14]=cTelephone;

			template.cpName[15]="cMobile";
			template.cpValue[15]=cMobile;

			template.cpName[16]="cMobileStyle";
			template.cpValue[16]=cMobileStyle;

			template.cpName[17]="cFax";
			template.cpValue[17]=cFax;

			template.cpName[18]="cFaxStyle";
			template.cpValue[18]=cFaxStyle;

			template.cpName[19]="cTelephoneStyle";
			template.cpValue[19]=cTelephoneStyle;

			template.cpName[20]="";

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

