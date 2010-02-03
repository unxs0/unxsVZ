/*
FILE 
	registration.c
	$Id$
AUTHOR
	(C) 2010 Franco Victorio for Unixservice
PURPOSE
	unxsISP Customer Interface
	program file.
*/


#define VAR_LIST_tClient "cFirstName,cLastName,cEmail,cPhone,uOwner,uCreatedBy,uCreatedDate,uModBy,uModDate"
#include "interface.h"

static char cFirstName[33]={""};
static char *cFirstNameStyle="type_fields_off";

static char cLastName[33]={""};
static char *cLastNameStyle="type_fields_off";

static char cEmail[101]={""};
static char *cEmailStyle="type_fields_off";

static char cPhone[101]={""};
static char *cPhoneStyle="type_fields_off";

static char cUser[101]={""};

extern unsigned uSetupRB;

//
//Local only
unsigned ValidateRegistrationInput(void);
void ModRegistration(void);

void SetRegistrationFieldsOn(void);
void LoadRegistration(unsigned cuClient);


void ProcessRegistrationVars(pentry entries[], int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{

		if(!strcmp(entries[i].name,"cFirstName"))
			sprintf(cFirstName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cLastName"))
			sprintf(cLastName,"%.32s",entries[i].val);
		else if(!strcmp(entries[i].name,"cEmail"))
			sprintf(cEmail,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"cPhone"))
			sprintf(cAddr1,"%.100s",entries[i].val);
	}

}//void ProcessRegistrationVars(pentry entries[], int x)


void RegistrationGetHook(entry gentries[],int x)
{
	if(gcPage[0])
	{
		if(!strcmp(gcPage,"Registration")
			htmlRegistration();
		else if (!strcmp(gcPage,"ConfirmRegistration")
			;//Something (in the way she moves)
	}
}//void RegistrationGetHook(entry gentries[],int x)


void LoadRegistration(unsigned uClient)
{
}//void LoadRegistration(char *uClient)


void RegistrationCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Registration"))
	{
		ProcessRegistrationVars(entries,x);
		if(!strcmp(gcFunction,"Send") ||
		   !strcmp(gcFunction,"Enviar") ||
		   !strcmp(gcFunction,"Envoyer"))
		{
			//Envoyerearla
		}

		htmlRegistration();
	}

}//void RegistrationCommands(pentry entries[], int x)


void htmlRegistration(void)
{
	htmlHeader("unxsISP Registration Interface","Header");
	htmlRegistrationPage("","Registration.Body");
	htmlFooter("Footer");

}//void htmlRegistration(void)


void htmlRegistrationPage(char *cTitle, char *cTemplateName)
{
	if(cTemplateName[0])
	{
		TemplateSelect(cTemplateName,guTemplateSet);

		template.cpName[0]="cTitle";
		template.cpValue[0]=cTitle;
		
		template.cpName[1]="cCGI";
		template.cpValue[1]="ispClient.cgi";
		
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
		
		template.cpName[14]="cPhone";
		template.cpValue[14]=cPhone;
		
		template.cpName[15]="cPhoneStyle";
		template.cpValue[15]=cPhoneStyle;
		
		template.cpName[16]="uOwner";
		template.cpValue[16]=cuOwner;

		template.cpName[17]="uCreatedBy";
		template.cpValue[17]=cuCreatedBy;

		template.cpName[18]="uCreatedDate";
		template.cpValue[18]=cuCreatedDate;

		template.cpName[19]="uModBy";
		template.cpValue[19]=cuModBy;

		template.cpName[20]="uModDate";
		template.cpValue[20]=cuModDate;

		template.cpName[21]="cCustomerName";
		template.cpValue[21]=cCustomerName;

		template.cpName[22]="cLanguage";
		template.cpValue[22]=cLanguage;

		template.cpName[23]="";

		printf("\n<!-- Start htmlRegistrationPage(%s) -->\n",cTemplateName); 
		Template(field[0], &template, stdout);
		printf("\n<!-- End htmlRegistrationPage(%s) -->\n",cTemplateName); 
	}
	else
	{
		printf("<hr>");
		printf("<center><font size=1>%s</font>\n",cTemplateName);
	}
}//void htmlRegistrationPage()


unsigned ValidateRegistrationInput(void)
{
	if(!cFirstName[0])
	{
		cFirstNameStyle="type_fields_req";
		if(guTemplateSet==2)
			gcMessage="<blink>Error: </blink>Must enter first name";
		else if(guTemplateSet==3)
			gcMessage="<blink>Error: </blink>Debe ingresar su nombre";
		else if(guTemplateSet==4)
			gcMessage="<blink>Error: </blink>Must enter first name (french)";

		return(0);
	}
	if(!cLastName[0])
	{
		cLastNameStyle="type_fields_req";
		if(guTemplateSet==2)
			gcMessage="<blink>Error: </blink>Must enter last name";
		else if(guTemplateSet==3)
			gcMessage="<blink>Error: </blink>Debe ingresar su apellido";
		else if(guTemplateSet==4)
		 	gcMessage="<blink>Error: </blink>Must enter last name (french)";

		return(0);
	}

	if(!cEmail[0])
	{
		cEmailStyle="type_fields_req";
		if(guTemplateSet==2)
			gcMessage="<blink>Error: </blink>Must enter email address";
		else if(guTemplateSet==3)
			gcMessage="<blink>Error: </blink>Debe ingresar su direcci&oacute;n de email";
		else if(guTemplateSet==4)
			gcMessage="<blink>Error: </blink>Must enter email address (french)";

		return(0);
	}
	else
	{
		if(strstr(cEmail,"@")==NULL || strstr(cEmail,".")==NULL)
		{
			cEmailStyle="type_fields_req";
			if(guTemplateSet==2)
				gcMessage="<blink>Error: </blink>Email has to be a valid email address";
			else if(guTemplateSet==3)
				gcMessage="<blink>Error: </blink>Su direcci&oacute;n de email parece inv&aacute;lida";
			else if(guTemplateSet==4)
				gcMessage="<blink>Error: </blink>Email has to be a valid email address (french)";

			return(0);
		}
	}
	if(!cPhone[0])//Phone
	{
		cAddr1Style="type_fields_req";
		if(guTemplateSet==2)
			gcMessage="<blink>Error: </blink>Must enter address information";
		else if(guTemplateSet==3)
			gcMessage="<blink>Error: </blink>Debe ingresar su direcci&oacute;n postal";
		else if(guTemplateSet==4)
			gcMessage="<blink>Error: </blink>Must enter address information (french)";

		return(0);
	}
	return(1);
		
}//unsigned ValidateRegistrationInput(void)


void SetRegistrationFieldsOn(void)
{
	cFirstNameStyle="type_fields";
	cLastNameStyle="type_fields";
	cEmailStyle="type_fields";
	cPhoneStyle="type_fields";

}//void SetRegistrationFieldsOn(void)



