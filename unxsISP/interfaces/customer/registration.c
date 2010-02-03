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
	htmlRegistrationPage("","MyAccount.Body");
	htmlFooter("Footer");

}//void htmlRegistration(void)


void htmlRegistrationPage(char *cTitle, char *cTemplateName)
{
}//void htmlRegistrationPage()


unsigned ValidateCustomerInput(void)
{
	if(!cFirstName[0])
	{
		SetCustomerFieldsOn();
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
		SetCustomerFieldsOn();
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
		SetCustomerFieldsOn();
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
			SetCustomerFieldsOn();
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
	if(!cAddr1[0])//Phone
	{
		SetCustomerFieldsOn();
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
		
}//unsigned ValidateInput(void)


void SetRegistrationFieldsOn(void)
{
	cFirstNameStyle="type_fields";
	cLastNameStyle="type_fields";
	cEmailStyle="type_fields";
	cPhoneStyle="type_fields";

}//void SetRegistrationFieldsOn(void)



