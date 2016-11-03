/*
FILE 
	registration.c
	svn ID removed
AUTHOR
	(C) 2010 Hugo Urquiza for Unixservice LLC.
PURPOSE
	unxsISP Customer Interface
	program file.
*/


#include "interface.h"

static char cFirstName[33]={""};
static char *cFirstNameStyle="type_fields";

static char cLastName[33]={""};
static char *cLastNameStyle="type_fields";

static char cEmail[101]={""};
static char *cEmailStyle="type_fields";

static char cPhone[101]={""};
static char *cPhoneStyle="type_fields";

static char cUser[101]={""};
static char cPassword[101]={""};

static char cId[65]={""};

static char cLanguage[65]={""};

//
//Local only
unsigned ValidateRegistrationInput(void);
void EmailRegistration(char *cSubject,char *cTemplateName);
void CommitRegistration(void);
void CreateTempRegistration(void);
void GenerateLoginInfo(void);
void CreatetTempClient(void);

//main.c
void SetLanguage(void);


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
			sprintf(cPhone,"%.100s",entries[i].val);
		else if(!strcmp(entries[i].name,"uLanguage"))
			sscanf(entries[i].val,"%u",&guTemplateSet);
	}

}//void ProcessRegistrationVars(pentry entries[], int x)


void RegistrationGetHook(entry gentries[],int x)
{
	register int i;
	
	for(i=0;i<x;i++)
	{

		if(!strcmp(gentries[i].name,"cId"))
			sprintf(cId,"%.32s",gentries[i].val);
	}
	
	if(cId[0]) CommitRegistration();

	htmlRegistration();

}//void RegistrationGetHook(entry gentries[],int x)


void RegistrationCommands(pentry entries[], int x)
{
	if(!strcmp(gcPage,"Registration"))
	{
		ProcessRegistrationVars(entries,x);
		if(!strcmp(gcFunction,"Send") ||
		   !strcmp(gcFunction,"Enviar") ||
		   !strcmp(gcFunction,"Envoyer"))
		{
			CreatetTempClient();
			if(ValidateRegistrationInput())
			{
				CreateTempRegistration();
				htmlHeader("unxsISP Customer Interface","Header");
				htmlRegistrationPage("","RegistrationDone.Body");
				htmlFooter("Footer");
			}
		}

		htmlRegistration();
	}

}//void RegistrationCommands(pentry entries[], int x)


void htmlRegistration(void)
{
	htmlHeader("unxsISP Customer Interface","Header");

	if(!guTemplateSet) guTemplateSet=2;
	htmlRegistrationPage("","Registration.Body");
	htmlFooter("Footer");

}//void htmlRegistration(void)


void htmlRegistrationPage(char *cTitle, char *cTemplateName)
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

			template.cpName[7]="cMessage";
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
		
			template.cpName[16]="";

			printf("\n<!-- Start htmlRegistrationPage(%s) -->\n",cTemplateName); 
			Template(field[0], &template, stdout);
			printf("\n<!-- End htmlRegistrationPage(%s) -->\n",cTemplateName); 
		}
		else
		{
			printf("<hr>");
			printf("<center><font size=1>%s (%u)</font>\n",cTemplateName,guTemplateSet);
		}
	}
}//void htmlRegistrationPage()


void FixCase(char *cString)
{
	register int x;

	for(x=0;x<strlen(cString);x++)
		cString[x]=tolower(cString[x]);
	
	cString[0]=toupper(cString[0]);

}//void FixCase(char *cString)


unsigned ValidateRegistrationInput(void)
{
	MYSQL_RES *res;

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
	FixCase(cLastName);
	FixCase(cFirstName);

	sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cLabel='%s %s'",TextAreaSave(cFirstName),TextAreaSave(cLastName));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);
	if(mysql_num_rows(res))
	{
		cFirstNameStyle="type_fields_req";
		cLastNameStyle="type_fields_req";

		if(guTemplateSet==2)
			gcMessage="<blink>Error: </blink>The entered name is in use";
		else if(guTemplateSet==3)
			gcMessage="<blink>Error: </blink>El nombre ingresado ya est&aacute; en uso";
		else if(guTemplateSet==4)
		 	gcMessage="<blink>Error: </blink>The entered name is in use (french)";

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
		sprintf(gcQuery,"SELECT uTempClient FROM tTempClient WHERE cEmail='%s'",TextAreaSave(cEmail));
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res))
		{
			cEmailStyle="type_fields_req";
			if(guTemplateSet==2)
				gcMessage="<blink>Error: </blink>The email address entered is already in use";
			else if(guTemplateSet==3)
				gcMessage="<blink>Error: </blink>Su direcci&oacute;n de email parece ya esta en uso";
			else if(guTemplateSet==4)
				gcMessage="<blink>Error: </blink>Email has to be a valid email address (french)";

			return(0);
		}
		mysql_free_result(res);

		sprintf(gcQuery,"SELECT uClient FROM tClient WHERE cEmail='%s'",TextAreaSave(cEmail));
		mysql_query(&gMysql,gcQuery);
		if(mysql_errno(&gMysql))
			htmlPlainTextError(mysql_error(&gMysql));
		res=mysql_store_result(&gMysql);
		if(mysql_num_rows(res))
		{
			cEmailStyle="type_fields_req";
			if(guTemplateSet==2)
				gcMessage="<blink>Error: </blink>The email address entered is already in use";
			else if(guTemplateSet==3)
				gcMessage="<blink>Error: </blink>Su direcci&oacute;n de email parece ya esta en uso";
			else if(guTemplateSet==4)
				gcMessage="<blink>Error: </blink>Email has to be a valid email address (french)";

			return(0);
		}
		mysql_free_result(res);
	}
	if(!cPhone[0])//Phone
	{
		cPhoneStyle="type_fields_req";
		if(guTemplateSet==2)
			gcMessage="<blink>Error: </blink>Must enter phone number";
		else if(guTemplateSet==3)
			gcMessage="<blink>Error: </blink>Debe ingresar su n&uacute;mero de tel&eacutefono";
		else if(guTemplateSet==4)
			gcMessage="<blink>Error: </blink>Must enter phone number (french)";

		return(0);
	}
	return(1);
		
}//unsigned ValidateRegistrationInput(void)


void EmailRegistration(char *cSubject,char *cTemplateName)
{
	FILE *fp;
	char cFrom[256]={"root"};
	struct t_template template;
	
	GetConfiguration("cFromEmailAddr",cFrom);
	
	if((fp=popen("/usr/lib/sendmail -t > /dev/null","w")))
	//debug only
	//if((fp=fopen("/tmp/eMailInvoice","w")))
	{
		fprintf(fp,"To: %s\n",cEmail);
		fprintf(fp,"From: %s\n",cFrom);
		fprintf(fp, "Reply-to: %s\n",cFrom);
		fprintf(fp,"Subject: %s\n",cSubject);
		
		template.cpName[0]="cFirstName";
		template.cpValue[0]=cFirstName;

		template.cpName[1]="cUser";
		template.cpValue[1]=cUser;

		template.cpName[2]="cPassword";
		template.cpValue[2]=cPassword;

		template.cpName[3]="cId";
		template.cpValue[3]=cId;

		template.cpName[4]="";

		fpTemplate(fp,cTemplateName,&template);
		pclose(fp);
		//debug only
		//fclose(fp);
	}

}//void eMailInvoice()


void LoadRegistration(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cFirstName,cLastName,cEmail,cPhone,cLanguage FROM tTempClient WHERE cHash='%s'",TextAreaSave(cId));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	if((field=mysql_fetch_row(res)))
	{
		sprintf(cFirstName,"%s",field[0]);
		sprintf(cLastName,"%s",field[1]);
		sprintf(cEmail,"%s",field[2]);
		sprintf(cPhone,"%s",field[3]);
		sprintf(cLanguage,"%s",field[4]);
	}
	else
	{
		htmlHeader("unxsISP Customer Interface","Header");
		htmlRegistrationPage("","RegistrationNotFound.Body");
		htmlFooter("Footer");
	}
}//void LoadRegistration(void)


static char cuCompany[16]={"1"}; //Default Root

void CommitRegistration(void)
{
	//This function actually creates a tClient record from the tTempClient record
	
	if(!guTemplateSet) guTemplateSet=2;

	LoadRegistration();

	GetConfiguration("uRegistrationCompany",cuCompany);
	sprintf(gcQuery,"INSERT INTO tClient SET cLabel='%s %s',cFirstName='%s',cLastName='%s',cEmail='%s',cTelephone='%s',"
			"uCreatedBy=1,uOwner=%s,uCreatedDate=UNIX_TIMESTAMP(NOW()),cLanguage='%s',cCode='NeverLogin'",
			TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cEmail)
			,TextAreaSave(cPhone)
			,cuCompany
			,cLanguage
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	guLoginClient=mysql_insert_id(&gMysql);
	SetLanguage();

	GenerateLoginInfo();	
	
	EmailRegistration("Your Login Information","RegistrationMail1");
	sprintf(gcQuery,"DELETE FROM tTempClient WHERE cHash='%s'",TextAreaSave(cId));
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	
	htmlHeader("unxsISP Customer Interface","Header");
	htmlRegistrationPage("","RegistrationCompleted.Body");
	htmlFooter("Footer");

}//void CommitRegistration(void)


void CreatetTempClient(void)
{
	sprintf(gcQuery,"CREATE TABLE IF NOT EXISTS tTempClient ( uTempClient INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,"
			"uOwner INT UNSIGNED NOT NULL DEFAULT 0,index (uOwner), uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0, "
			"uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0, uModBy INT UNSIGNED NOT NULL DEFAULT 0, uModDate INT "
			"UNSIGNED NOT NULL DEFAULT 0, cFirstName VARCHAR(32) NOT NULL DEFAULT '', cLastName VARCHAR(32) NOT "
			"NULL DEFAULT '', cEmail VARCHAR(100) NOT NULL DEFAULT '',index (cEmail), cAddr1 VARCHAR(100) NOT NULL "
			"DEFAULT '', cAddr2 VARCHAR(100) NOT NULL DEFAULT '', cCity VARCHAR(100) NOT NULL DEFAULT '', "
			"cState VARCHAR(100) NOT NULL DEFAULT '', cZip VARCHAR(32) NOT NULL DEFAULT '', cPhone VARCHAR(100) NOT "
			"NULL DEFAULT '', cHash VARCHAR(65) NOT NULL DEFAULT '', cLanguage VARCHAR(32) NOT NULL DEFAULT 'English')");
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

}//void CreatetTempClient(void)

unsigned uTempClient=0;

void EmailAfterRegistration(void)
{
	MYSQL_RES *res;
	MYSQL_ROW field;

	sprintf(gcQuery,"SELECT cHash FROM tTempClient WHERE uTempClient=%u",uTempClient);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	res=mysql_store_result(&gMysql);

	field=mysql_fetch_row(res);
	sprintf(cId,"%s",field[0]);
	
	EmailRegistration("Please Confirm Your Registration","RegistrationMail0");

}//void EmailAfterRegistration(void)


void CreateTempRegistration(void)
{
	//This function inserts a new tTempClient record
	//Is the first step for registration, if it gets confirmed
	//the tTempClient record is removed and tClient/tAuthorize records
	//are created
	char *cLanguage="";

	if(guTemplateSet==2)
		cLanguage="English";
	else if(guTemplateSet==3)
		cLanguage="Spanish";
	else if(guTemplateSet==4)
		cLanguage="French";

	sprintf(gcQuery,"INSERT INTO tTempClient SET cFirstName='%s',cLastName='%s',cEmail='%s',cPhone='%s',"
			"cHash=MD5(CONCAT(cFirstName,cLastName,NOW())),cLanguage='%s',uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			TextAreaSave(cFirstName)
			,TextAreaSave(cLastName)
			,TextAreaSave(cEmail)
			,TextAreaSave(cPhone)
			,cLanguage
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));
	uTempClient=mysql_insert_id(&gMysql);
	
	EmailAfterRegistration();

}//void CreateTempRegistration(void)

void LowerCase(char *cString)
{
	register int iX;

	for(iX=0;iX<strlen(cString);iX++)
		cString[iX]=tolower(cString[iX]);	

}//void LowerCase(char *cString)


void EncryptPasswd(char *pw);
void to64(register char *s, register long v, register int n);


void GenerateLoginInfo(void)
{
	//This function creates the tAuthorize record
	//with a random password
	char cGenPassword[100]={""};

	(void)srand((int)time((time_t *)NULL));
	to64(&cGenPassword[0],rand(),6);
	cGenPassword[6]=0;
	sprintf(cPassword,"%s",cGenPassword); //Save plain text copy for email ;)
	EncryptPasswd(cGenPassword);
	LowerCase(cFirstName);
	LowerCase(cLastName);
	sprintf(cUser,"%s.%s",cFirstName,cLastName);

	sprintf(gcQuery,"INSERT INTO tAuthorize SET cLabel='%s.%s',cPasswd='%s',cClrPasswd='%s',uPerm=1,uCertClient=%u,uOwner=%s,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW())",
			cFirstName
			,cLastName
			,cGenPassword
			,cPassword
			,guLoginClient
			,cuCompany
			);
	mysql_query(&gMysql,gcQuery);
	if(mysql_errno(&gMysql))
		htmlPlainTextError(mysql_error(&gMysql));

	cFirstName[0]=toupper(cFirstName[0]);
	
}//void GenerateLoginInfo(void)


void funcSelectRegLang(FILE *fp)
{
	fprintf(fp,"<select name=uLanguage class=type_fields onChange=submit()>\n");
	
	fprintf(fp,"<option value=2 ");
	if(guTemplateSet==2) fprintf(fp,"selected");
	fprintf(fp,">English</option>\n");
	
	fprintf(fp,"<option value=3 ");
	if(guTemplateSet==3) fprintf(fp,"selected");
	fprintf(fp,">Espa&ntilde;ol</option>\n");
	
	fprintf(fp,"<option value=4 ");
	if(guTemplateSet==4) fprintf(fp,"selected");
	fprintf(fp,">Francais</option>\n");

	fprintf(fp,"</select>\n");

}//void funcSelectRegLang(FILE *fp)


